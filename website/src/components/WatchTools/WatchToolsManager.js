import UpdateMCUManager from "../FirmwareUpdate/utils/UpdateMCUManager";
import { validateBackground } from "./imageProcessing";
import { MAX_CRASH_BYTES, parseCrash } from "./diagnostics";

export const BACKGROUND_PATH = "/user/bg_new.bin";

export default class WatchToolsManager extends UpdateMCUManager {
  constructor(options) {
    super(options);
    this.connectionGeneration = 0;
    this.logState = "off";
    this._watchOptions = {
      onLog: (text) => {
        if (!this._logRequested) return;
        clearTimeout(this._logTimer);
        this.setLogState("streaming");
        this._logCallback?.(text);
      },
      onTraffic: (entry) => this._trafficCallback?.(entry),
    };
  }

  onFirmwareLog(callback) { this._logCallback = callback; return this; }
  onTraffic(callback) { this._trafficCallback = callback; return this; }
  onLogState(callback) { this._logStateCallback = callback; return this; }
  setLogState(state) { this.logState = state; this._logStateCallback?.(state); }

  async setLogCapture(enabled) {
    if (!this.isConnected() || !this._watch?.write) throw new Error("Connect to a watch that supports Bluetooth log commands first.");
    clearTimeout(this._logTimer);
    this._logRequested = enabled;
    this.setLogState(enabled ? "waiting" : "off");
    try {
      await this._watch.send({ t: "log", status: enabled });
      if (enabled && this._logRequested && this.logState === "waiting") this._logTimer = setTimeout(() => {
        if (this._logRequested && this.logState === "waiting") this.setLogState("no-data");
      }, 10000);
    } catch (error) { this._logRequested = false; this.setLogState("off"); throw error; }
  }

  async _writeRaw(packet) {
    await super._writeRaw(packet);
    this._trafficCallback?.({ direction: "TX", channel: "SMP", bytes: packet.slice() });
  }

  _notification(event) {
    const value = event.target.value;
    this._trafficCallback?.({ direction: "RX", channel: "SMP", bytes: new Uint8Array(value.buffer, value.byteOffset, value.byteLength).slice() });
    return super._notification(event);
  }

  async _openBle(device) {
    await super._openBle(device);
    this.connectionGeneration++;
    await this._watch.send({ t: "smp", status: false });
    this.status("Connected. Watch tools are ready.");
  }

  async ensureSmp() {
    if (!this.isConnected()) throw new Error("Connect to the watch first.");
    if (this._logRequested) await this.setLogCapture(false);
    const device = this._device;
    try {
      await this._watch.prepare(device.gatt);
      await this._watch.enableSmp();
      // Disabling SMP unregisters its GATT service. A cached characteristic can
      // still be discoverable while its notification subscription no longer
      // exists on the watch. Reconnect before subscribing to the new service.
      this._rediscovering = true;
      try {
        device.gatt.disconnect();
        await this.sleep(500);
        const server = await device.gatt.connect();
        this.connectionGeneration++;
        await this._watch.prepare(server);
        await this._watch.enableSmp();
        await this._discoverSmp(server);
      } finally { this._rediscovering = false; }
    } catch (error) {
      if (this.isConnected()) {
        try { await this._watch?.send({ t: "smp", status: false }); } catch { /* Already disconnected. */ }
      } else await this._disconnected();
      throw error;
    }
  }

  async backgroundCommand(action) {
    const result = await this._watch.request(
      { t: "watchface_bg", action },
      (message) => message.t === "watchface_bg" && message.action === `${action}_result`,
    );
    if (result.ok !== true) throw new Error(`The watch could not ${action === "apply" ? "apply" : "restore"} the background: ${result.error || "operation failed"}${result.rc == null ? "" : ` (${result.rc})`}.`);
    return result;
  }

  async applyBackground(bytes, { signal, onProgress = () => {}, onApplying = () => {} } = {}) {
    validateBackground(bytes);
    const checkCancelled = () => { if (signal?.aborted) throw new Error("Background upload cancelled."); };
    checkCancelled();
    await this.ensureSmp();
    const generation = this.connectionGeneration;
    const watch = this._watch;
    const cancel = () => this.cancelUpload();
    try {
      checkCancelled();
      signal?.addEventListener("abort", cancel);
      const data = bytes.buffer.slice(bytes.byteOffset, bytes.byteOffset + bytes.byteLength);
      await this.upload(data, 0, true, onProgress, BACKGROUND_PATH);
      checkCancelled();
      onApplying();
      await this.backgroundCommand("apply");
    } finally {
      signal?.removeEventListener("abort", cancel);
      if (generation === this.connectionGeneration && watch === this._watch && this.isConnected()) {
        try { await watch.send({ t: "smp", status: false }); } catch { /* Already disconnected. */ }
      }
    }
  }

  async restoreDefault() {
    if (!this.isConnected()) throw new Error("Connect to the watch first.");
    await this._watch.prepare(this._device.gatt);
    return this.backgroundCommand("reset");
  }

  cancelUpload() { this._finishTransfer(new Error("Background upload cancelled. Your current background was kept.")); }

  async withSmp(action, signal) {
    const cancelled = () => { if (signal?.aborted) throw new Error("Operation cancelled."); };
    cancelled();
    await this.ensureSmp();
    const generation = this.connectionGeneration, watch = this._watch;
    const abort = () => this._finishRequest(new Error("Operation cancelled."));
    try {
      cancelled();
      signal?.addEventListener("abort", abort);
      return await action(cancelled);
    } finally {
      signal?.removeEventListener("abort", abort);
      if (generation === this.connectionGeneration && watch === this._watch && this.isConnected()) {
        try { await watch.send({ t: "smp", status: false }); } catch { /* Already disconnected. */ }
      }
    }
  }

  async statCrash(path) {
    try {
      const result = await this.request(0, 8, 1, { name: path });
      if (!Number.isInteger(result.len) || result.len <= 0 || result.len > MAX_CRASH_BYTES) throw new Error("The watch reported an invalid crash dump size.");
      return { path, size: result.len };
    } catch (error) {
      // SMP v1 ENOENT and SMP v2 filesystem FILE_NOT_FOUND. Transport errors and
      // unsupported commands must never be presented as an empty crash list.
      if ((error.smpError?.rc === 5 && error.smpError.group === undefined) || (error.smpError?.group === 8 && error.smpError.rc === 3)) return null;
      throw error;
    }
  }

  async findCrash() {
    for (const path of ["/user/coredump.txt", "/lvgl_lfs/coredump.txt"]) {
      const file = await this.statCrash(path);
      if (file) return file;
    }
    return null;
  }

  async readCrash(file, cancelled, onProgress = () => {}) {
    const bytes = new Uint8Array(file.size);
    let offset = 0;
    while (offset < bytes.length) {
      cancelled();
      const result = await this.request(0, 8, 0, { name: file.path, off: offset });
      cancelled();
      const chunk = result.data;
      if (result.off !== offset || !(chunk instanceof Uint8Array) || !chunk.length || offset + chunk.length > bytes.length || (result.len !== undefined && result.len !== bytes.length)) throw new Error("The watch returned inconsistent crash dump data. Try checking the watch again.");
      bytes.set(chunk, offset); offset += chunk.length;
      onProgress({ bytes: offset, total: bytes.length, percentage: Math.floor(offset / bytes.length * 100) });
    }
    parseCrash(bytes);
    return bytes;
  }

  downloadCrash({ signal, onProgress } = {}) {
    return this.withSmp(async (cancelled) => {
      cancelled();
      const file = await this.findCrash();
      if (!file) return null;
      return { ...file, bytes: await this.readCrash(file, cancelled, onProgress) };
    }, signal);
  }

  eraseCrash(expected, { signal } = {}) {
    return this.withSmp(async (cancelled) => {
      cancelled();
      const file = await this.findCrash();
      if (!file) throw new Error("The crash dump is already absent. Check the watch again.");
      const bytes = await this.readCrash(file, cancelled);
      if (file.path !== expected.path || bytes.length !== expected.bytes.length || bytes.some((byte, i) => byte !== expected.bytes[i])) throw new Error("The crash dump has changed. Check and download it again before erasing.");
      cancelled();
      const result = await this.request(2, 9, 0, { argv: ["coredump", "erase", "0"] });
      if (result.ret !== 0) throw new Error(`The watch could not erase the crash dump (${result.ret ?? "unsupported command"}).`);
      cancelled();
      if (await this.statCrash(file.path)) throw new Error("The watch still has a crash dump. Erase could not be confirmed.");
    }, signal);
  }

  async disconnect() {
    if (this._logRequested && this.isConnected()) {
      try { await this.setLogCapture(false); } catch { /* Disconnect still proceeds. */ }
    }
    return super.disconnect();
  }

  async _disconnected() {
    this.connectionGeneration++;
    clearTimeout(this._logTimer);
    this._logRequested = false;
    this.setLogState("off");
    return super._disconnected();
  }
}
