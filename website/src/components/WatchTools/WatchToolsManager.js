import UpdateMCUManager from "../FirmwareUpdate/utils/UpdateMCUManager";
import { validateBackground } from "./imageProcessing";

export const BACKGROUND_PATH = "/user/bg_new.bin";

export default class WatchToolsManager extends UpdateMCUManager {
  constructor(options) {
    super(options);
    this.connectionGeneration = 0;
  }

  async _openBle(device) {
    await super._openBle(device);
    this.connectionGeneration++;
    await this._watch.send({ t: "smp", status: false });
    this.status("Connected. Choose a background to get started.");
  }

  async ensureSmp() {
    if (!this.isConnected()) throw new Error("Connect to the watch first.");
    const device = this._device;
    try {
      await this._watch.prepare(device.gatt);
      await this._watch.enableSmp();
      try { await this._discoverSmp(device.gatt); }
      catch {
        // SMP is registered dynamically; browsers may still have a stale GATT list.
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
      }
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

  async _disconnected() {
    this.connectionGeneration++;
    return super._disconnected();
  }
}
