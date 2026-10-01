export const NUS_SERVICE = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";
export const NUS_WRITE = "6e400002-b5a3-f393-e0a9-e50e24dcca9e";
export const NUS_NOTIFY = "6e400003-b5a3-f393-e0a9-e50e24dcca9e";

export function boardFromHardwareVersion(hardware) {
  return typeof hardware === "string" ? hardware.trim().split("/")[0] : "";
}

// NUS notifications can split a JSON message anywhere, including within UTF-8.
// Other GadgetBridge messages may arrive between version/battery responses.
export class WatchMessageReader {
  constructor(onMessage) {
    this.onMessage = onMessage;
    this.decoder = new TextDecoder();
    this.buffer = "";
  }

  push(value) {
    this.buffer += this.decoder.decode(value, { stream: true });
    for (;;) {
      const start = this.buffer.indexOf("{");
      if (start === -1) { this.buffer = ""; return; }
      this.buffer = this.buffer.slice(start);
      let depth = 0, quoted = false, escaped = false, end = -1;
      for (let i = 0; i < this.buffer.length; i++) {
        const char = this.buffer[i];
        if (quoted) {
          if (escaped) escaped = false;
          else if (char === "\\") escaped = true;
          else if (char === '"') quoted = false;
        } else if (char === '"') quoted = true;
        else if (char === "{") depth++;
        else if (char === "}" && --depth === 0) { end = i + 1; break; }
      }
      if (end === -1) {
        if (this.buffer.length > 8192) this.buffer = "";
        return;
      }
      const message = this.buffer.slice(0, end);
      this.buffer = this.buffer.slice(end);
      try { this.onMessage(JSON.parse(message)); } catch { /* Ignore unrelated/malformed notifications. */ }
    }
  }
}

export default class WatchBluetooth {
  constructor(onInfo, delay) {
    this.onInfo = onInfo;
    this.delay = delay;
    this.info = {};
    this.reader = new WatchMessageReader((message) => {
      if (this.pendingMessage?.match(message)) this.finishMessage(null, message);
      if (message.t === "ver") {
        this.updateInfo({ hardwareVersion: message.hw, firmwareVersion: message.fw });
        this.versionReceived?.();
      } else if (message.t === "status" && Number.isFinite(message.bat)) {
        this.updateInfo({ batteryLevel: message.bat });
      }
    });
    this.onNotification = (event) => this.reader.push(event.target.value);
    this.onBattery = (event) => this.updateInfo({ batteryLevel: event.target.value.getUint8(0) });
  }

  updateInfo(info) {
    this.info = { ...this.info, ...Object.fromEntries(Object.entries(info).filter(([, value]) => value !== undefined)) };
    this.onInfo(this.info);
  }

  cleanup() {
    this.finishMessage(new Error("Watch disconnected before confirming the background change."));
    this.notify?.removeEventListener("characteristicvaluechanged", this.onNotification);
    this.battery?.removeEventListener("characteristicvaluechanged", this.onBattery);
    this.notify = this.battery = this.write = null;
    this.reader.buffer = "";
    this.reader.decoder = new TextDecoder();
  }

  async prepare(server) {
    this.cleanup();
    // Version responses contain CONFIG_BOARD_TARGET, the same identity used by
    // the Android app. DIS hardware revisions alone may just be "1.0".
    let nus;
    try { nus = await server.getPrimaryService(NUS_SERVICE); }
    catch (error) {
      // Only an absent service means this is older SMP-only firmware. Pairing
      // failures must not silently become a misleading manual-DFU instruction.
      if (error.name !== "NotFoundError") throw error;
    }
    if (nus) try {
      this.write = await nus.getCharacteristic(NUS_WRITE);
      this.notify = await nus.getCharacteristic(NUS_NOTIFY);
      this.notify.addEventListener("characteristicvaluechanged", this.onNotification);
      await this.notify.startNotifications();
    } catch (error) {
      this.cleanup();
      throw new Error("Could not access the watch's Bluetooth commands. Open Settings → Bluetooth on the watch and enable Pairable, then reconnect and accept pairing on your computer.", { cause: error });
    }
    try {
      const dis = await server.getPrimaryService(0x180a);
      for (const [uuid, key] of [[0x2a27, "hardwareVersion"], [0x2a26, "firmwareVersion"]]) {
        try {
          const value = await (await dis.getCharacteristic(uuid)).readValue();
          const text = new TextDecoder().decode(value).replace(/\0/g, "").trim();
          if (key !== "hardwareVersion" || /^(watchdk|zswatch(?:_legacy)?)@/i.test(text)) this.updateInfo({ [key]: text });
        } catch { /* Optional DIS characteristics. */ }
      }
    } catch { /* Older firmware may not expose DIS. */ }
    await this.readBattery(server);
    if (this.write) {
      let finish;
      const response = new Promise((resolve) => { finish = resolve; this.versionReceived = resolve; });
      const timer = setTimeout(finish, 2000);
      try { await this.send({ t: "ver" }); await response; }
      finally { clearTimeout(timer); this.versionReceived = null; }
    }
  }

  async readBattery(server) {
    try {
      if (!this.battery) this.battery = await (await server.getPrimaryService(0x180f)).getCharacteristic(0x2a19);
      const value = await this.battery.readValue();
      this.updateInfo({ batteryLevel: value.getUint8(0) });
      this.battery.removeEventListener("characteristicvaluechanged", this.onBattery);
      this.battery.addEventListener("characteristicvaluechanged", this.onBattery);
      if (this.battery.properties?.notify) await this.battery.startNotifications();
    } catch { /* Battery information must not prevent updating. */ }
  }

  async send(message) {
    const bytes = new TextEncoder().encode(`GB(${JSON.stringify(message)})`);
    // Web Bluetooth doesn't expose negotiated MTU. 20-byte chunks work even
    // on the minimum ATT MTU, and the watch parser accepts fragmented commands.
    for (let offset = 0; offset < bytes.length; offset += 20) {
      const chunk = bytes.slice(offset, offset + 20);
      if (this.write.properties?.writeWithoutResponse) await this.write.writeValueWithoutResponse(chunk);
      else await this.write.writeValueWithResponse(chunk);
    }
  }

  request(message, match, timeout = 10000) {
    if (this.pendingMessage) return Promise.reject(new Error("Another watch command is in progress."));
    if (!this.write) return Promise.reject(new Error("Connect to the watch first."));
    return new Promise((resolve, reject) => {
      const pending = { match, resolve, reject };
      this.pendingMessage = pending;
      pending.timer = setTimeout(() => {
        if (this.pendingMessage === pending) this.finishMessage(new Error("The watch did not confirm the background change. Check that its firmware supports custom backgrounds."));
      }, timeout);
      this.send(message).catch((error) => { if (this.pendingMessage === pending) this.finishMessage(error); });
    });
  }

  finishMessage(error, message) {
    const pending = this.pendingMessage;
    if (!pending) return;
    this.pendingMessage = null;
    clearTimeout(pending.timer);
    if (error) pending.reject(error);
    else pending.resolve(message);
  }

  async enableSmp() {
    if (!this.write) return false; // Existing SMP-only firmware remains supported.
    await this.send({ t: "smp", status: true });
    await this.delay(2000); // Matches the app's service registration wait.
    return true;
  }
}
