const { test } = require("node:test");
const assert = require("node:assert/strict");
const fs = require("node:fs");
const path = require("node:path");
globalThis.crypto ??= require("node:crypto").webcrypto;
const babel = require("@babel/core");
const loader = require.extensions[".js"];
const sourceRoot = path.resolve(__dirname, "../src") + path.sep;
require.extensions[".js"] = (module, filename) => {
  if (!filename.startsWith(sourceRoot)) return loader(module, filename);
  module._compile(babel.transformSync(fs.readFileSync(filename, "utf8"), { filename, configFile: false, babelrc: false,
    plugins: [require.resolve("@babel/plugin-transform-modules-commonjs")] }).code, filename);
};
const { rgbaToLvgl, validateBackground, cropRectangle, drawBackground, BACKGROUND_SIZE } = require("../src/components/WatchTools/imageProcessing");
const WatchToolsManager = require("../src/components/WatchTools/WatchToolsManager").default;
const WatchBluetooth = require("../src/components/FirmwareUpdate/utils/watchBluetooth").default;
const { NUS_SERVICE, NUS_WRITE } = require("../src/components/FirmwareUpdate/utils/watchBluetooth");
const CBOR = require("../src/mcumgr-web/js/cbor");
const catalog = require("../src/components/WatchTools/backgrounds.json");

function sampleBackground() {
  const rgba = new Uint8Array(240 * 240 * 4);
  for (let i = 0; i < rgba.length; i += 4) { rgba[i] = 255; rgba[i + 3] = 255; }
  return rgbaToLvgl(rgba);
}

test("LVGL bytes match the app's header, RGB565 byte order and circular boundary", () => {
  const rgba = new Uint8Array(240 * 240 * 4);
  const pixel = (x, y, r, g, b, alpha = 255) => rgba.set([r, g, b, alpha], (y * 240 + x) * 4);
  pixel(120, 120, 255, 0, 0); pixel(121, 120, 0, 255, 0); pixel(122, 120, 0, 0, 255);
  pixel(123, 120, 255, 255, 255, 0); pixel(124, 120, 255, 255, 255, 128);
  pixel(0, 0, 255, 255, 255); pixel(120, 0, 255, 255, 255);
  const bytes = rgbaToLvgl(rgba), view = new DataView(bytes.buffer);
  assert.equal(bytes.length, 115212);
  assert.deepEqual(Array.from(bytes.slice(0, 12)), [0x19, 0x12, 0, 0, 240, 0, 240, 0, 224, 1, 0, 0]);
  const value = (x, y) => view.getUint16(12 + (y * 240 + x) * 2, true);
  assert.equal(value(120, 120), 0xf800); assert.equal(value(121, 120), 0x07e0); assert.equal(value(122, 120), 0x001f);
  assert.equal(value(123, 120), 0); assert.equal(value(124, 120), 0x8410);
  assert.equal(value(0, 0), 0); assert.equal(value(120, 0), 0xffff);
  assert.throws(() => rgbaToLvgl(new Uint8Array(4)), /240/);
});

test("crop positions stay within landscape and portrait images at all zoom levels", () => {
  assert.deepEqual(cropRectangle(800, 400), { x: 200, y: 0, size: 400 });
  assert.deepEqual(cropRectangle(400, 800, 2, -1, 1), { x: 0, y: 600, size: 200 });
  assert.deepEqual(cropRectangle(400, 400, 8, -10, 10), { x: 0, y: 300, size: 100 });
  assert.throws(() => cropRectangle(0, 300), /dimensions/);
  assert.throws(() => cropRectangle(100, 300, NaN), /dimensions/);
});

test("every imported app background has a valid 240×240 binary and preview", () => {
  const items = catalog.flatMap(group => group.backgrounds);
  assert.equal(items.length, 33);
  assert.equal(new Set(items.map(item => item.binary)).size, items.length);
  for (const item of items) {
    const dir = path.resolve(__dirname, "../static/watchface-backgrounds");
    validateBackground(fs.readFileSync(path.join(dir, item.binary)));
    assert.deepEqual(Array.from(fs.readFileSync(path.join(dir, item.image)).slice(0, 8)), [137, 80, 78, 71, 13, 10, 26, 10]);
  }
  assert.throws(() => validateBackground(new Uint8Array(12)), /size/);
  const bad = sampleBackground(); bad[1] = 0;
  assert.throws(() => validateBackground(bad), /format/);
});

test("preview drawing and binary export use the same crop and round mask", () => {
  const data = new Uint8ClampedArray(240 * 240 * 4).fill(255);
  let drawArgs, rendered;
  const ctx = { fillRect() {}, drawImage(...args) { drawArgs = args; },
    getImageData: () => ({ data }), putImageData: pixels => { rendered = pixels; } };
  const canvas = { getContext: () => ctx };
  const image = { naturalWidth: 800, naturalHeight: 400 };
  const binary = drawBackground(canvas, image, { zoom: 2, x: 1, y: -1 });
  assert.equal(canvas.width, 240); assert.equal(canvas.height, 240);
  assert.deepEqual(drawArgs, [image, 600, 0, 200, 200, 0, 0, 240, 240]);
  assert.deepEqual(Array.from(rendered.data.slice(0, 4)), [0, 0, 0, 255]);
  assert.equal(new DataView(binary.buffer).getUint16(12, true), 0);
  assert.equal(new DataView(binary.buffer).getUint16(12 + (120 * 240 + 120) * 2, true), 0xffff);
});

function realProtocolMock({ uploadError = false, applyError = false, resetError = false, noReply = false, corruptApplied = false } = {}) {
  const manager = new WatchToolsManager({ logger: { info() {}, error() {} } });
  manager.sleep = async () => {};
  const listeners = new Set(), commands = [], uploads = [], files = new Map();
  let connectedWithSmp = false, smp = false, connects = 0, nusBuffer = "", packetBuffer = [];
  let registration = 0, notificationRegistration = -1, notificationConnection = -1;
  const makeChar = (extra = {}) => {
    const callbacks = new Set();
    return { properties: { notify: true, writeWithoutResponse: true },
      addEventListener: (_, fn) => callbacks.add(fn), removeEventListener: (_, fn) => callbacks.delete(fn),
      startNotifications: async () => {}, emit: bytes => { const data = Uint8Array.from(bytes); for (const fn of [...callbacks]) fn({ target: { value: new DataView(data.buffer) } }); }, ...extra };
  };
  const notify = makeChar();
  const nus = makeChar({ async writeValueWithoutResponse(bytes) {
    assert.ok(bytes.length <= 20);
    nusBuffer += new TextDecoder().decode(bytes);
    if (!nusBuffer.endsWith(")")) return;
    const command = JSON.parse(nusBuffer.slice(3, -1)); nusBuffer = ""; commands.push(command);
    if (command.t === "smp") {
      if (command.status && !smp) registration++;
      smp = command.status;
    }
    else if (command.t === "ver") notify.emit(new TextEncoder().encode('{"t":"ver","hw":"watchdk@1/nrf5340/cpuapp","fw":"0.8.1"}'));
    else if (command.t === "watchface_bg") {
      const error = command.action === "apply" ? applyError : resetError;
      if (!error && command.action === "apply") {
        const bytes = files.get("/user/bg_new.bin").slice();
        if (corruptApplied) bytes[bytes.length - 1] ^= 1;
        files.set("/user/bg.bin", bytes); files.delete("/user/bg_new.bin");
      } else if (!error && command.action === "reset") files.delete("/user/bg.bin");
      if (noReply) return;
      // An unrelated result and a fragmented real result must be handled correctly.
      notify.emit(new TextEncoder().encode('{"t":"watchface_bg","action":"unrelated_result","ok":true}'));
      const response = new TextEncoder().encode(JSON.stringify({ t: "watchface_bg", action: `${command.action}_result`, ok: !error, ...(error ? { error: "operation_failed", rc: -22 } : {}) }));
      notify.emit(response.slice(0, 18)); notify.emit(response.slice(18));
    }
  } });
  const smpChar = makeChar({
    async startNotifications() {
      // Cached startNotifications is idempotent on this connection, even when
      // the watch has unregistered and recreated the service underneath it.
      if (notificationConnection !== connects) {
        notificationConnection = connects;
        notificationRegistration = registration;
      }
    },
    async writeValueWithoutResponse(bytes) {
    assert.equal(smp, true); packetBuffer.push(...bytes);
    if (packetBuffer.length < 8 || packetBuffer.length < 8 + packetBuffer[2] * 256 + packetBuffer[3]) return;
    const packet = Uint8Array.from(packetBuffer); packetBuffer = [];
    assert.equal(packet[5], 8);
    const body = CBOR.decode(packet.slice(8).buffer);
    let result;
    if (packet[0] === 2) {
      assert.equal(packet[7], 0); uploads.push(body);
      if (body.off === 0) files.set(body.name, new Uint8Array(body.len));
      if (!uploadError) files.get(body.name).set(body.data, body.off);
      result = uploadError ? { rc: 9 } : { off: body.off + body.data.length };
    } else {
      assert.equal(packet[0], 0);
      const bytes = files.get(body.name);
      if (!bytes) result = { rc: 5 };
      else if (packet[7] === 1) result = { len: bytes.length };
      else {
        assert.equal(packet[7], 2); assert.equal(body.type, "sha256");
        result = { len: bytes.length, output: new Uint8Array(await crypto.subtle.digest("SHA-256", bytes)) };
      }
    }
    const response = new Uint8Array(CBOR.encode(result));
    if (notificationRegistration !== registration) return;
    smpChar.emit([packet[0] + 1, 0, response.length >> 8, response.length & 255, 0, 8, packet[6], packet[7], ...response]);
  } });
  const battery = makeChar({ readValue: async () => new DataView(Uint8Array.of(88).buffer) });
  const device = { name: "ZSWatch", addEventListener: (_, fn) => listeners.add(fn), removeEventListener: (_, fn) => listeners.delete(fn),
    gatt: { connected: false,
      async connect() { connects++; this.connected = true; connectedWithSmp = smp; return this; },
      disconnect() { this.connected = false; for (const fn of [...listeners]) fn(); },
      async getPrimaryService(uuid) {
        if (uuid === NUS_SERVICE) return { getCharacteristic: async id => id === NUS_WRITE ? nus : notify };
        if (uuid === 0x180f) return { getCharacteristic: async () => battery };
        if (uuid === manager.SERVICE_UUID && smp && connectedWithSmp) return { getCharacteristic: async () => smpChar };
        throw new DOMException("Missing service", "NotFoundError");
      }
    } };
  manager._requestDevice = async () => device;
  return { manager, device, commands, uploads, notify, get smp() { return smp; }, get connects() { return connects; } };
}

test("connect discovers hardware and automatically enables SMP, then releases it while editing", async () => {
  const mock = realProtocolMock();
  try {
    await mock.manager.connect({ transport: "ble" });
    assert.equal(mock.manager.deviceInfo.hardwareVersion, "watchdk@1/nrf5340/cpuapp");
    assert.equal(mock.manager.deviceInfo.batteryLevel, 88);
    assert.equal(mock.connects, 2); assert.equal(mock.smp, false);
  } finally { await mock.manager.disconnect(); }
});

test("background upload stages every acknowledged byte before applying, without rebooting", async () => {
  const mock = realProtocolMock(), bytes = sampleBackground(), progress = [];
  try {
    await mock.manager.connect({ transport: "ble" });
    await mock.manager.applyBackground(bytes, { onProgress: value => progress.push(value.percentage) });
    assert.ok(mock.uploads.length > 1);
    assert.equal(mock.uploads[0].len, BACKGROUND_SIZE);
    assert.ok(mock.uploads.every(body => body.name === "/user/bg_new.bin"));
    assert.deepEqual(Uint8Array.from(mock.uploads.flatMap(body => Array.from(body.data))), bytes);
    assert.equal(progress.at(-1), 100);
    assert.equal(mock.commands.filter(command => command.t === "watchface_bg" && command.action === "apply").length, 1);
    assert.equal(mock.commands.some(command => command.t === "reset"), false);
    assert.equal(mock.smp, false);
  } finally { await mock.manager.disconnect(); }
});

test("failed upload and cancellation never apply a staged background", async () => {
  for (const uploadError of [true, false]) {
    const mock = realProtocolMock({ uploadError }), controller = new AbortController();
    try {
      await mock.manager.connect({ transport: "ble" });
      await assert.rejects(mock.manager.applyBackground(sampleBackground(), {
        signal: controller.signal, onProgress: () => controller.abort(),
      }), uploadError ? /rejected/ : /cancelled/);
      assert.equal(mock.commands.some(command => command.t === "watchface_bg"), false);
      assert.equal(mock.smp, false);
    } finally { await mock.manager.disconnect(); }
  }
});

test("apply and restore report the watch's rejected commands rather than claiming success", async () => {
  const mock = realProtocolMock({ applyError: true, resetError: true });
  try {
    await mock.manager.connect({ transport: "ble" });
    await assert.rejects(mock.manager.applyBackground(sampleBackground()), /operation_failed \(-22\)/);
    assert.equal(mock.smp, false);
    const count = mock.uploads.length;
    await assert.rejects(mock.manager.restoreDefault(), /restore.*operation_failed/);
    assert.equal(mock.uploads.length, count);
  } finally { await mock.manager.disconnect(); }
});

test("restore default works without uploading a file", async () => {
  const mock = realProtocolMock();
  try {
    await mock.manager.connect({ transport: "ble" });
    assert.equal((await mock.manager.restoreDefault()).ok, true);
    assert.equal(mock.uploads.length, 0);
    assert.ok(mock.commands.some(command => command.t === "watchface_bg" && command.action === "reset"));
  } finally { await mock.manager.disconnect(); }
});

test("failed SMP preparation cleans up; stale operations cannot disable a new connection's SMP", async () => {
  const failed = realProtocolMock();
  try {
    await failed.manager.connect({ transport: "ble" });
    failed.manager._discoverSmp = async () => { throw new Error("SMP unavailable"); };
    await assert.rejects(failed.manager.applyBackground(sampleBackground()), /SMP unavailable/);
    assert.equal(failed.smp, false);
    assert.equal(failed.uploads.length, 0);
  } finally { await failed.manager.disconnect(); }
  const changed = realProtocolMock();
  try {
    await changed.manager.connect({ transport: "ble" });
    const before = changed.commands.filter(command => command.t === "smp" && !command.status).length;
    await changed.manager.applyBackground(sampleBackground(), { onApplying: () => { changed.manager.connectionGeneration++; } });
    assert.equal(changed.commands.filter(command => command.t === "smp" && !command.status).length, before);
  } finally { await changed.manager.disconnect(); }
});

test("background command timeouts and disconnects reject and release pending state", async () => {
  const watch = new WatchBluetooth(() => {}, async () => {});
  watch.write = { properties: { writeWithoutResponse: true }, writeValueWithoutResponse: async () => {} };
  await assert.rejects(watch.request({ t: "watchface_bg", action: "apply" }, () => false, 10), /did not confirm/);
  const pending = watch.request({ t: "watchface_bg", action: "reset" }, () => false);
  watch.cleanup(); await assert.rejects(pending, /disconnected/);
  assert.equal(watch.pendingMessage, null);
});

test("invalid images and an already-cancelled operation never touch the watch", async () => {
  const mock = realProtocolMock(), controller = new AbortController(); controller.abort();
  await assert.rejects(mock.manager.applyBackground(new Uint8Array(1)), /size/);
  await assert.rejects(mock.manager.applyBackground(sampleBackground(), { signal: controller.signal }), /cancelled/);
  assert.equal(mock.commands.length, 0);
});


test("consecutive background uploads renew notifications after dynamic SMP re-registration", async () => {
  const mock = realProtocolMock(), bytes = sampleBackground();
  try {
    await mock.manager.connect({ transport: "ble" });
    const connected = mock.connects;
    await mock.manager.applyBackground(bytes);
    await mock.manager.applyBackground(bytes);
    assert.equal(mock.connects, connected + 2);
    assert.equal(mock.commands.filter(command => command.t === "watchface_bg" && command.action === "apply").length, 2);
    assert.equal(mock.smp, false);
  } finally { await mock.manager.disconnect(); }
});


test("missing background acknowledgements verify applied bytes and default restoration over SMP", async () => {
  const mock = realProtocolMock({ noReply: true });
  try {
    await mock.manager.connect({ transport: "ble" });
    const request = mock.manager._watch.request.bind(mock.manager._watch);
    mock.manager._watch.request = (message, match) => request(message, match, 10);
    await mock.manager.applyBackground(sampleBackground());
    assert.equal(mock.smp, false);
    assert.equal((await mock.manager.restoreDefault()).ok, true);
    assert.equal(mock.smp, false);
  } finally { await mock.manager.disconnect(); }
});

test("a missing acknowledgement cannot hide an unapplied or corrupted background", async () => {
  for (const options of [{ applyError: true }, { corruptApplied: true }]) {
    const mock = realProtocolMock({ noReply: true, ...options });
    try {
      await mock.manager.connect({ transport: "ble" });
      const request = mock.manager._watch.request.bind(mock.manager._watch);
      mock.manager._watch.request = (message, match) => request(message, match, 10);
      await assert.rejects(mock.manager.applyBackground(sampleBackground()), /did not confirm/);
      assert.equal(mock.smp, false);
    } finally { await mock.manager.disconnect(); }
  }
});

test("both managers filter the Bluetooth picker by prefix and preserve service permissions", async () => {
  const MCUManager = require("../src/components/FirmwareUpdate/utils/UpdateMCUManager").default;
  const descriptor = Object.getOwnPropertyDescriptor(globalThis, "navigator");
  const device = { name: "ZSWatch AA:BB:CC" };
  let requested;
  Object.defineProperty(globalThis, "navigator", { configurable: true, value: { bluetooth: {
    requestDevice: async (options) => { requested = options; return device; },
  } } });
  try {
    for (const manager of [new MCUManager(), new WatchToolsManager()]) {
      assert.equal(await manager._requestDevice([{ namePrefix: "ZSWatch" }]), device);
      assert.deepEqual(requested.filters, [{ namePrefix: "ZSWatch" }]);
      assert.equal(requested.acceptAllDevices, undefined);
      assert.ok(requested.optionalServices.includes(manager.SERVICE_UUID));
      assert.ok(requested.optionalServices.includes(NUS_SERVICE));
      await manager._requestDevice(null);
      assert.equal(requested.acceptAllDevices, true);
      assert.equal(requested.filters, undefined);
    }
  } finally {
    if (descriptor) Object.defineProperty(globalThis, "navigator", descriptor);
    else delete globalThis.navigator;
  }
});


test("missing restore acknowledgement fails when the active background remains", async () => {
  const mock = realProtocolMock({ noReply: true, resetError: true });
  try {
    await mock.manager.connect({ transport: "ble" });
    const request = mock.manager._watch.request.bind(mock.manager._watch);
    mock.manager._watch.request = (message, match) => request(message, match, 10);
    await mock.manager.applyBackground(sampleBackground());
    await assert.rejects(mock.manager.restoreDefault(), /did not confirm/);
    assert.equal(mock.smp, false);
  } finally { await mock.manager.disconnect(); }
});
