export const LAST_WATCH_KEY = "zswatch-last-bluetooth-device";

const defaultFilters = [{ namePrefix: "ZSWatch" }];
function matches(device, filters) {
  return (filters?.length ? filters : defaultFilters).some((filter) => {
    // Cached grants do not expose advertised services/manufacturer data.
    // Use the chooser for filters that cannot be verified from the device name.
    if (Object.keys(filter).some((key) => !["name", "namePrefix"].includes(key))) return false;
    if (filter.name !== undefined && device.name !== filter.name) return false;
    if (filter.namePrefix !== undefined && !device.name?.startsWith(filter.namePrefix)) return false;
    return true;
  });
}

// getDevices reads this origin's existing grants; it does not scan for new
// devices. Advertisement watching is an optional optimization for one known
// watch. The Connect button never waits for it to become available.
export default class BluetoothDiscovery {
  constructor({ bluetooth = () => globalThis.navigator?.bluetooth, storage = () => globalThis.localStorage, onChange = () => {} } = {}) {
    this.bluetooth = bluetooth;
    this.storage = storage;
    this.onChange = onChange;
    this.devices = [];
    this.filters = defaultFilters;
    this.generation = 0;
  }

  selected(filters = this.filters) {
    let last = this.lastDeviceId;
    try { last = this.storage()?.getItem(LAST_WATCH_KEY) || last; } catch { /* Private browsing may block storage. */ }
    const remembered = last && this.devices.find((device) => device.id === last && (!filters?.length || matches(device, filters)));
    if (remembered) return remembered;
    const candidates = this.devices.filter((device) => matches(device, filters));
    return candidates.length === 1 ? candidates[0] : null;
  }

  snapshot() {
    const device = this.selected();
    return { savedName: device?.name || (device ? "ZSWatch" : ""), recentlySeen: Boolean(this.seenAt && Date.now() - this.seenAt < 10000) };
  }
  emit() { this.onChange(this.snapshot()); }

  async prepare(filters = defaultFilters) {
    this.stop();
    this.filters = filters;
    this.devices = [];
    this.emit();
    const generation = this.generation;
    const api = this.bluetooth();
    if (!api?.getDevices) { this.devices = []; this.emit(); return; }
    try {
      const devices = await api.getDevices();
      if (generation !== this.generation) return;
      this.devices = devices;
      this.emit();
      const device = this.selected();
      if (!device?.watchAdvertisements) return;
      const controller = new AbortController();
      const received = () => {
        if (generation !== this.generation) return;
        this.seenAt = Date.now();
        this.cancelWatch(); // One packet is enough to warm discovery.
        this.emit();
        this.expiry = setTimeout(() => { this.seenAt = 0; this.emit(); }, 10000);
      };
      this.watching = { controller, device, received };
      device.addEventListener("advertisementreceived", received);
      try { await device.watchAdvertisements({ signal: controller.signal }); }
      catch { if (this.watching?.controller === controller) this.cancelWatch(); }
    } catch {
      // Missing/denied getDevices must retain the ordinary click-based chooser.
      if (generation === this.generation) { this.devices = []; this.emit(); }
    }
  }

  remember(device) {
    this.lastDeviceId = device.id;
    if (device.id) try { this.storage()?.setItem(LAST_WATCH_KEY, device.id); } catch { /* Reuse within this page still works. */ }
    this.devices = [...this.devices.filter((item) => item.id !== device.id), device];
    this.emit();
  }

  cancelWatch() {
    const watch = this.watching;
    this.watching = null;
    if (!watch) return;
    watch.device.removeEventListener("advertisementreceived", watch.received);
    watch.controller.abort();
  }

  stop() {
    this.generation++;
    this.cancelWatch();
    clearTimeout(this.expiry);
    this.seenAt = 0;
  }
}
