import MCUManager from "../../../mcumgr-web/js/mcumgr";
import CBOR from "../../../mcumgr-web/js/cbor";
import WatchBluetooth, { NUS_SERVICE } from "./watchBluetooth";

// Keep the shared transport implementation, but expose acknowledged transfers.
// The upstream cmdUpload promise only waits for the transfer to start.
export default class UpdateMCUManager extends MCUManager {
  constructor(options = {}) {
    super(options);
    this.deviceInfo = {};
    this._smpNotification = this._notification.bind(this);
  }

  onDeviceInfo(callback) { this._deviceInfoCallback = callback; return this; }
  onStatus(callback) { this._statusCallback = callback; return this; }
  status(message) { this._statusCallback?.(message); }

  get bluetoothServices() { return [NUS_SERVICE, 0x180a, 0x180f, this.SERVICE_UUID]; }

  async _requestDevice(filters) {
    this.status("Choose your watch in the Bluetooth device list.");
    const options = { optionalServices: this.bluetoothServices };
    if (filters?.length) options.filters = filters;
    else options.acceptAllDevices = true;
    return navigator.bluetooth.requestDevice(options);
  }

  async _connectBle(filters) {
    this._connectingCallback?.();
    this.deviceInfo = {};
    this._deviceInfoCallback?.({});
    try {
      const device = await this._requestDevice(filters);
      await this._openBle(device);
    } catch (error) {
      if (this._device?.gatt?.connected) this._device.gatt.disconnect();
      await this._disconnected();
      throw error;
    }
  }

  async _openBle(device, reconnecting = false) {
    this._device = device;
    this._deviceName = device.name || "ZSWatch";
    this._userRequestedDisconnect = false;
    this._watch = new WatchBluetooth((info) => {
      this.deviceInfo = info;
      this._deviceInfoCallback?.(info);
    }, (ms) => this.sleep(ms), this._watchOptions);
    this._bleDisconnectListener = () => {
      if (!this._rediscovering) this._disconnected();
    };
    device.addEventListener("gattserverdisconnected", this._bleDisconnectListener);
    this.status(`Opening Bluetooth connection to ${this._deviceName}...`);
    let server = await device.gatt.connect();
    this._bleReadyAt = Date.now();
    this.status("Reading watch hardware and battery...");
    await this._watch.prepare(server, { enableUpdates: true });
    this.status("Enabling Bluetooth firmware updates...");
    const sentEnable = Boolean(this._watch.write);
    try {
      await this._discoverSmp(server);
    } catch (error) {
      if (!sentEnable) throw new Error("Firmware update service unavailable. Enable Bluetooth in System → Update App on the watch, then reconnect.");
      // Chrome may retain the service list from before dynamic SMP registration.
      this.status("Rediscovering firmware update service...");
      this._rediscovering = true;
      try {
        device.gatt.disconnect();
        await this.sleep(500);
        server = await device.gatt.connect();
        this._bleReadyAt = Date.now();
        await this._watch.prepare(server, { enableUpdates: true, readInfo: false });
        await this._discoverSmp(server);
      } catch {
        throw new Error("The watch did not expose its firmware update service. Enable Bluetooth in System → Update App and reconnect; older firmware may require this manual step.");
      } finally { this._rediscovering = false; }
    }
    this.status(sentEnable ? "Bluetooth DFU enabled. Reading installed images..." : "Firmware update service ready. Reading installed images...");
    await this._connectCallback?.({ reconnecting });
  }

  async _discoverSmp(server) {
    this._characteristic?.removeEventListener("characteristicvaluechanged", this._smpNotification);
    this._service = await server.getPrimaryService(this.SERVICE_UUID);
    this._characteristic = await this._service.getCharacteristic(this.CHARACTERISTIC_UUID);
    this._characteristic.addEventListener("characteristicvaluechanged", this._smpNotification);
    if (this._characteristic.stopNotifications) await this._characteristic.stopNotifications();
    await this._characteristic.startNotifications();
    this._buffer = new Uint8Array();
    this._mtu = server.mtu || 244;
  }

  async ensureUpdateReady() {
    if (!this.isConnected()) throw new Error("Connect to the watch first.");
    if (this._transport === "ble") {
      this.status("Enabling Bluetooth firmware updates...");
      await this._watch.prepare(this._device.gatt);
      await this._watch.enableSmp(); // Firmware automatically disables SMP after idle time.
      await this._discoverSmp(this._device.gatt);
      await this._watch.readBattery(this._device.gatt);
    }
  }

  // Await BLE writes too: the upstream transport starts them without waiting,
  // so asynchronous write failures otherwise escape the transfer's error path.
  async _writeRaw(packet) {
    if (this._transport === "serial") return this._writeSerial(packet);
    for (let offset = 0; offset < packet.length; offset += this._mtu) {
      await this._characteristic.writeValueWithoutResponse(packet.slice(offset, offset + this._mtu));
      await this.sleep(5);
    }
    if (this._watch?.smpEnabled) this._watch.smpActivityAt = Date.now();
  }

  async readDeviceInfo(recovery) {
    if (this._transport === "ble" || recovery) return;
    // This optional SMP command is absent on older builds and in MCUBoot.
    try {
      const info = await this.request(0, 0, 7, { format: "i" }, 2500);
      if (typeof info.output === "string") {
        this.deviceInfo = { ...this.deviceInfo, hardwareVersion: info.output };
        this._deviceInfoCallback?.(this.deviceInfo);
      }
    } catch { /* Keep manual hardware selection available when unsupported. */ }
  }

  async _connectSerial() {
    this.deviceInfo = {};
    this._deviceInfoCallback?.({});
    return super._connectSerial();
  }

  _startSerialReadLoop() {
    const port = this._serialPort;
    super._startSerialReadLoop();
    this._serialReadLoopPromise?.then(() => {
      if (this._serialPort === port && this._serialReadLoopActive) this._disconnectSerial();
    });
  }

  async _disconnectSerial() {
    const port = this._serialPort;
    this._serialReadLoopActive = false;
    try { await this._serialReader?.cancel(); } catch { /* The port may already be gone. */ }
    try { await this._serialReadLoopPromise; } catch { /* Reader releases its lock in finally. */ }
    // Web Serial requires both stream locks released before port.close().
    this._serialWriter?.releaseLock();
    this._serialWriter = null;
    if (this._serialDisconnectListener) navigator.serial.removeEventListener("disconnect", this._serialDisconnectListener);
    this._serialDisconnectListener = null;
    this._serialPort = null;
    try { await port?.close(); } catch { /* Unplugging also closes the port. */ }
    await this._disconnected();
  }

  async restartAndReconnect(onStage = () => {}) {
    const device = this._device;
    const bluetooth = this._transport === "ble";
    this._restarting = true;
    try {
      onStage("Restarting watch");
      try { await this.request(2, 0, 5); }
      catch (error) {
        // A reboot may drop the link before the reset acknowledgement arrives.
        if (error.code !== "WATCH_DISCONNECTED") throw error;
      }
      if (!bluetooth) {
        await this._disconnectSerial();
        return { verified: false, message: "Update sent; reconnect over USB to verify the installed images." };
      }
      onStage("Reconnecting to watch");
      if (device.gatt.connected) { device.gatt.disconnect(); await this._disconnected(); }
      await this.sleep(3000);
      for (let attempt = 0; attempt < 5; attempt++) {
        try {
          this._connectingCallback?.();
          await this._openBle(device, true);
          onStage("Verifying installed images");
          const result = await this.request(0, 1, 0);
          return { verified: true, images: result.images || [] };
        } catch {
          if (device.gatt.connected) device.gatt.disconnect();
          await this._disconnected();
          if (attempt < 4) await this.sleep(2000);
        }
      }
      return { verified: false, message: "Update sent, but automatic reconnection failed. Reconnect to verify the installed images." };
    } finally { this._restarting = false; }
  }

  async _sendMessage(op, group, id, data) {
    const payload = data === undefined ? new Uint8Array() : new Uint8Array(CBOR.encode(data));
    const sequence = this._seq;
    this._seq = (sequence + 1) % 256;
    const packet = Uint8Array.from([op, 0, payload.length >> 8, payload.length & 255, group >> 8, group & 255, sequence, id, ...payload]);
    await this._writeRaw(packet);
  }

  upload(image, imageNumber = 0, filesystem = false, onProgress = () => {}, path = "/S/full_fs") {
    if (this._transfer || this._request || this._uploadIsInProgress) return Promise.reject(new Error("Another device operation is in progress."));
    if (!this.isConnected()) return Promise.reject(new Error("Connect to the watch first."));
    if (!image.byteLength) return Promise.reject(new Error("Cannot upload an empty image."));
    return new Promise((resolve, reject) => {
      this._uploadIsInProgress = true;
      this._transfer = { image, imageNumber, filesystem, path, onProgress, resolve, reject, offset: 0, retries: 0 };
      this._sendChunk(this._transfer);
    });
  }

  async _sendChunk(transfer) {
    if (this._transfer !== transfer) return;
    clearTimeout(transfer.timer);
    try {
      const data = transfer.filesystem
        ? { data: new Uint8Array(), off: transfer.offset, name: transfer.path }
        : { data: new Uint8Array(), off: transfer.offset, image: transfer.imageNumber };
      if (transfer.offset === 0) {
        data.len = transfer.image.byteLength;
        if (!transfer.filesystem) data.sha = new Uint8Array(await this._hash(transfer.image));
      }
      if (this._transfer !== transfer) return;
      let length = (transfer.filesystem ? this._netbuf_size : this._mtu) - CBOR.encode(data).byteLength - (transfer.filesystem ? 20 : 8);
      if (transfer.filesystem) length -= length % 4;
      if (length <= 0) throw new Error("Device packet size is too small for this transfer.");
      data.data = new Uint8Array(transfer.image.slice(transfer.offset, transfer.offset + length));
      transfer.sentEnd = transfer.offset + data.data.length;
      transfer.sequence = this._seq;
      transfer.timer = setTimeout(() => {
        if (this._transfer !== transfer) return;
        if (++transfer.retries > 3) this._finishTransfer(new Error("The watch stopped responding. Reconnect and retry the update."));
        else this._sendChunk(transfer);
      }, transfer.offset === 0 ? Math.max(this._firstChunkTimeout, 10000) : this._chunkTimeout);
      await this._sendMessage(2, transfer.filesystem ? 8 : 1, transfer.filesystem ? 0 : 1, data);
    } catch (error) {
      if (this._transfer === transfer) this._finishTransfer(error);
    }
  }

  _finishTransfer(error) {
    const transfer = this._transfer;
    if (!transfer) return;
    clearTimeout(transfer.timer);
    this._transfer = null;
    this._uploadIsInProgress = false;
    if (error) transfer.reject(error);
    else transfer.resolve();
  }

  cancelUpload() {
    this._finishTransfer(new Error("Update cancelled. You can retry with the selected package."));
  }

  // Base BLE reconnection must not restart its own legacy upload loop.
  _uploadNext() {}
  _uploadNextFileSystem() {}

  request(op, group, id, data, timeout = 10000) {
    if (this._transfer || this._request) return Promise.reject(new Error("Another device operation is in progress."));
    return new Promise((resolve, reject) => {
      const request = { sequence: this._seq, op: op + 1, group, id, resolve, reject };
      this._request = request;
      request.timer = setTimeout(() => this._finishRequest(new Error("The watch did not acknowledge the command.")), timeout);
      this._sendMessage(op, group, id, data).catch((error) => {
        if (this._request === request) this._finishRequest(error);
      });
    });
  }

  _finishRequest(error, data) {
    const request = this._request;
    if (!request) return;
    clearTimeout(request.timer);
    this._request = null;
    if (error) request.reject(error);
    else request.resolve(data);
  }

  _processMessage(message) {
    const group = message[4] * 256 + message[5];
    const id = message[7];
    const sequence = message[6];
    const data = CBOR.decode(message.slice(8).buffer);
    const error = data.err || (data.rc ? { rc: data.rc } : null);
    const transfer = this._transfer;
    if (message[0] === 3 && ((group === 1 && id === 1) || (group === 8 && id === 0))) {
      if (!transfer || sequence !== transfer.sequence || group !== (transfer.filesystem ? 8 : 1)) return;
      clearTimeout(transfer.timer);
      if (error) return this._finishTransfer(new Error(`Upload rejected by watch: ${JSON.stringify(error)}`));
      // The first response may resume a matching image already on the watch.
      if (!Number.isInteger(data.off) || data.off < 0 || data.off > transfer.image.byteLength || (transfer.offset !== 0 && data.off > transfer.sentEnd)) {
        return this._finishTransfer(new Error("Invalid upload offset received from watch."));
      }
      if (data.off <= transfer.offset) {
        if (++transfer.retries > 3) return this._finishTransfer(new Error("The watch is not accepting upload data."));
      } else transfer.retries = 0;
      transfer.offset = data.off;
      transfer.onProgress({ bytes: data.off, total: transfer.image.byteLength, percentage: Math.floor(data.off / transfer.image.byteLength * 100) });
      if (data.off === transfer.image.byteLength) this._finishTransfer();
      else this._sendChunk(transfer);
      return;
    }
    const request = this._request;
    if (request && message[0] === request.op && sequence === request.sequence && group === request.group && id === request.id) {
      const failure = error ? Object.assign(new Error(`Command rejected by watch: ${JSON.stringify(error)}`), { smpError: error }) : null;
      this._finishRequest(failure, data);
      // Image/OS replies also update the firmware UI through its message callback.
      // Filesystem replies must not reach the legacy upload state machine.
      if (group !== 8) super._processMessage(message);
      return;
    }
    // Ignore late file replies instead of passing them into the legacy upload loop.
    if (group === 8 && id === 0) return;
    super._processMessage(message);
  }

  async _disconnected() {
    if (this._transport === "serial" && this._serialPort) return this._disconnectSerial();
    this._finishTransfer(new Error("Watch disconnected. Reconnect and retry the update."));
    const error = new Error("Watch disconnected before acknowledging the command.");
    error.code = "WATCH_DISCONNECTED";
    this._finishRequest(error);
    if (this._bleDisconnectListener && this._transport === "ble") this._device?.removeEventListener("gattserverdisconnected", this._bleDisconnectListener);
    this._characteristic?.removeEventListener("characteristicvaluechanged", this._smpNotification);
    this._watch?.cleanup();
    this._watch = null;
    return super._disconnected();
  }
}
