export const NUS_SERVICE = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";
export const NUS_WRITE = "6e400002-b5a3-f393-e0a9-e50e24dcca9e";
export const NUS_NOTIFY = "6e400003-b5a3-f393-e0a9-e50e24dcca9e";

// Robust GATT caching may reject the first ATT operation after a dynamic
// service change. Retry only that explicit rejection, never a lost response.
async function afterServiceChange(operation) {
  try { return await operation(); }
  catch (error) {
    if (!/database out of sync|ATT error: 0x12/i.test(error.message)) throw error;
    return operation();
  }
}

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
    this.pushText(this.decoder.decode(value, { stream: true }));
  }

  pushText(text) {
    this.buffer += text;
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

// Logs and protocol messages share NUS. Keep framed log content out of the JSON
// parser, including when a tag or a UTF-8 character spans BLE notifications.
export class WatchStreamReader {
  constructor(onMessage, onLog) {
    this.messages = new WatchMessageReader(onMessage);
    this.onLog = onLog;
    this.reset();
  }

  reset() {
    this.decoder = new TextDecoder();
    this.buffer = "";
    this.inLog = false;
    this.messages.buffer = "";
  }

  push(value) {
    this.buffer += this.decoder.decode(value, { stream: true });
    for (;;) {
      const tag = this.inLog ? "</BLELOG>" : "<BLELOG>";
      const index = this.buffer.indexOf(tag);
      const restart = this.inLog ? this.buffer.indexOf("<BLELOG>") : -1;
      // A dropped closing frame (or overlapping firmware log writes) must not
      // swallow every subsequent log and protocol response indefinitely.
      if (restart >= 0 && (index < 0 || restart < index)) {
        this.buffer = this.buffer.slice(restart + "<BLELOG>".length);
        continue;
      }
      if (index >= 0) {
        const text = this.buffer.slice(0, index);
        if (this.inLog) { if (text.length <= 65536) this.onLog?.(text); }
        else this.messages.pushText(text);
        this.buffer = this.buffer.slice(index + tag.length);
        this.inLog = !this.inLog;
      } else if (this.inLog) {
        if (this.buffer.length > 65536) {
          this.buffer = "";
          this.inLog = false;
        }
        return;
      } else {
        let tail = 0;
        for (let length = 1; length < tag.length; length++) {
          if (this.buffer.endsWith(tag.slice(0, length))) tail = length;
        }
        this.messages.pushText(this.buffer.slice(0, this.buffer.length - tail));
        this.buffer = tail ? this.buffer.slice(-tail) : "";
        return;
      }
    }
  }
}

export default class WatchBluetooth {
  constructor(onInfo, delay, options = {}) {
    this.onInfo = onInfo;
    this.delay = delay;
    this.options = options;
    this.info = {};
    this.smpEnabled = false;
    this.smpActivityAt = 0;
    this.commandGeneration = 0;
    this.commandQueue = Promise.resolve();
    this.reader = new WatchStreamReader((message) => {
      if (this.pendingMessage?.match(message)) this.finishMessage(null, message);
      options.onMessage?.(message);
      if (message.t === "ver") {
        this.updateInfo({ hardwareVersion: message.hw, firmwareVersion: message.fw, firmwareSha: message.sha, debugBuild: message.dbg });
        this.versionReceived?.();
      } else if (message.t === "status" && Number.isFinite(message.bat)) {
        this.updateInfo({ batteryLevel: message.bat });
      }
    }, options.onLog);
    this.onNotification = (event) => {
      const value = event.target.value;
      options.onTraffic?.({ direction: "RX", channel: "NUS", bytes: new Uint8Array(value.buffer, value.byteOffset, value.byteLength).slice() });
      this.reader.push(value);
    };
    this.onBattery = (event) => this.updateInfo({ batteryLevel: event.target.value.getUint8(0) });
  }

  updateInfo(info) {
    this.info = { ...this.info, ...Object.fromEntries(Object.entries(info).filter(([, value]) => value !== undefined)) };
    this.onInfo(this.info);
  }

  cleanup() {
    this.commandGeneration++;
    // A disconnected GATT write may take time to reject. A new connection must
    // not wait behind that old write; queued old commands still fail their token.
    this.commandQueue = Promise.resolve();
    this.smpEnabled = false;
    this.finishMessage(new Error("Watch disconnected before confirming the background change."));
    this.notify?.removeEventListener("characteristicvaluechanged", this.onNotification);
    this.battery?.removeEventListener("characteristicvaluechanged", this.onBattery);
    this.notify = this.battery = this.write = null;
    this.reader.reset();
  }

  async subscribeCommands() {
    // A cached characteristic can still report "notifying" after a GATT
    // reconnect or service change, although the watch has lost its CCC state.
    // Explicitly renew the subscription instead of trusting start's idempotence.
    if (this.notify.stopNotifications) await afterServiceChange(() => this.notify.stopNotifications());
    this.reader.reset();
    await afterServiceChange(() => this.notify.startNotifications());
  }

  async prepare(server, { enableUpdates = false, readInfo = true } = {}) {
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
      await this.subscribeCommands();
    } catch (error) {
      this.cleanup();
      throw new Error("Could not access the watch's Bluetooth commands. Open Settings → Bluetooth on the watch and enable Pairable, then reconnect and accept pairing on your computer.", { cause: error });
    }
    // SMP's existing enable command also requests the fast connection interval.
    // Send it before optional reads, especially on an already-connected OS link.
    if (enableUpdates) await this.enableSmp();
    if (!readInfo) return;
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

  send(message) {
    const generation = this.commandGeneration;
    const operation = this.commandQueue.then(() => this.writeCommand(message, generation));
    // Keepalive and UI commands share a fragmented NUS stream. Serializing the
    // complete command prevents their fragments being mixed by the watch parser.
    this.commandQueue = operation.catch(() => {});
    return operation;
  }

  async writeCommand(message, generation) {
    if (generation !== this.commandGeneration || !this.write) throw new Error("Watch disconnected before sending the command.");
    const write = this.write;
    const bytes = new TextEncoder().encode(`GB(${JSON.stringify(message)})`);
    // Web Bluetooth doesn't expose negotiated MTU. 20-byte chunks work even
    // on the minimum ATT MTU, and the watch parser accepts fragmented commands.
    for (let offset = 0; offset < bytes.length; offset += 20) {
      if (generation !== this.commandGeneration) throw new Error("Watch disconnected before sending the command.");
      const chunk = bytes.slice(offset, offset + 20);
      if (write.properties?.write || !write.properties?.writeWithoutResponse) await afterServiceChange(() => write.writeValueWithResponse(chunk));
      else await afterServiceChange(() => write.writeValueWithoutResponse(chunk));
      this.options.onTraffic?.({ direction: "TX", channel: "NUS", bytes: chunk });
    }
    if (generation === this.commandGeneration && message.t === "smp" && typeof message.status === "boolean") {
      this.smpEnabled = message.status;
      if (message.status) this.smpActivityAt = Date.now();
    }
  }

  request(message, match, timeout = 10000) {
    if (this.pendingMessage) return Promise.reject(new Error("Another watch command is in progress."));
    if (!this.write) return Promise.reject(new Error("Connect to the watch first."));
    return new Promise((resolve, reject) => {
      const pending = { match, resolve, reject };
      this.pendingMessage = pending;
      pending.timer = setTimeout(() => {
        if (this.pendingMessage === pending) this.finishMessage(Object.assign(new Error("The watch did not confirm the background change."), { code: "WATCH_RESPONSE_TIMEOUT" }));
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
    return true;
  }
}
