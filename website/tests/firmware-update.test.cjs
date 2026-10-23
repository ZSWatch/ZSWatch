const { test } = require("node:test");
const assert = require("node:assert/strict");
const fs = require("node:fs");
const path = require("node:path");
const babel = require("@babel/core");
// The website's ES modules use Webpack-style extensionless imports. Compile
// only local source for Node's test runner using Docusaurus' bundled Babel.
const originalLoader = require.extensions[".js"];
const sourceRoot = path.resolve(__dirname, "../src") + path.sep;
require.extensions[".js"] = (module, filename) => {
  if (!filename.startsWith(sourceRoot)) return originalLoader(module, filename);
  const { code } = babel.transformSync(fs.readFileSync(filename, "utf8"), {
    filename, configFile: false, babelrc: false,
    plugins: [require.resolve("@babel/plugin-transform-modules-commonjs")],
  });
  module._compile(code, filename);
};
const JSZip = require("jszip");
const { readFirmwarePackage } = require("../src/components/FirmwareUpdate/utils/firmwarePackage");
const MCUManager = require("../src/components/FirmwareUpdate/utils/UpdateMCUManager").default;
const WatchToolsManager = require("../src/components/WatchTools/WatchToolsManager").default;
const CBOR = require("../src/mcumgr-web/js/cbor");
const { applyUploadedFirmware, runFirmwareUpdate, verifyInstalledImages } = require("../src/components/FirmwareUpdate/utils/updateWorkflow");
const { WatchMessageReader, boardFromHardwareVersion, NUS_SERVICE, NUS_WRITE, NUS_NOTIFY } = require("../src/components/FirmwareUpdate/utils/watchBluetooth");
const { fetchReleases, fetchArtifacts, compatibleAssets, boardFromAsset } = require("../src/components/FirmwareUpdate/utils/githubApi");

test("hardware identity and fragmented GadgetBridge notifications match the app", () => {
  assert.equal(boardFromHardwareVersion("watchdk@1/nrf5340/cpuapp"), "watchdk@1");
  assert.equal(boardFromHardwareVersion("zswatch_legacy@5"), "zswatch_legacy@5");
  assert.equal(boardFromHardwareVersion(undefined), "");
  const messages = [], reader = new WatchMessageReader((message) => messages.push(message));
  const bytes = new TextEncoder().encode('noise GB({"t":"ver","hw":"watchdk@1/nrf5340/cpuapp","fw":"v1","other":"å}\\\""})\n{"t":"status","bat":15}\n');
  for (const byte of bytes) reader.push(Uint8Array.of(byte));
  assert.equal(messages.length, 2);
  assert.equal(messages[0].hw, "watchdk@1/nrf5340/cpuapp");
  assert.equal(messages[1].bat, 15);
  reader.push(new TextEncoder().encode('{"unfinished":"' + 'x'.repeat(9000)));
  assert.equal(reader.buffer, "");
});

function bleWatch({ nus = true, smp = true, staleServices = false, resetDropsLink = false, shared = false } = {}) {
  const commands = [], delays = [], deviceListeners = new Set();
  let nusBuffer = "", enabled = !nus, connects = 0, permissionOptions;
  const characteristic = (extra = {}) => ({
    listeners: new Set(), properties: { writeWithoutResponse: true, notify: true },
    addEventListener(_name, fn) { this.listeners.add(fn); },
    removeEventListener(_name, fn) { this.listeners.delete(fn); },
    async startNotifications() { return this; },
    emit(bytes) { for (const fn of this.listeners) fn({ target: { value: new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength) } }); },
    ...extra,
  });
  const notify = characteristic();
  const writer = characteristic({ async writeValueWithoutResponse(bytes) {
    assert.ok(bytes.length <= 20);
    nusBuffer += new TextDecoder().decode(bytes);
    if (!nusBuffer.endsWith(")")) return;
    const message = JSON.parse(nusBuffer.slice(3, -1));
    nusBuffer = ""; commands.push(message);
    if (message.t === "smp") enabled = message.status;
    if (message.t === "ver") {
      const reply = new TextEncoder().encode('{"t":"ver","hw":"watchdk@1/nrf5340/cpuapp","fw":"v0.8.1"}\n');
      notify.emit(reply.slice(0, 13)); notify.emit(reply.slice(13));
    }
  } });
  const battery = characteristic({ async readValue() { return new DataView(Uint8Array.of(15).buffer); } });
  const instance = new (shared ? WatchToolsManager : MCUManager)({ keepSmpEnabled: shared, logger: { info() {}, error() {} } });
  instance.sleep = async (ms) => { delays.push(ms); };
  const smpChar = characteristic({ async writeValueWithoutResponse(packet) {
    if (packet[4] === 0 && packet[5] === 0 && packet[7] === 5 && resetDropsLink) { device.gatt.disconnect(); return; }
    const images = [{ image: 0, slot: 0, hash: Uint8Array.from({ length: 32 }, () => 0xab), active: true, confirmed: true, pending: false }];
    smpChar.emit(response(packet, packet[7] === 0 ? { images } : {}));
  } });
  const device = {
    name: "ZSWatch Test",
    addEventListener(_event, fn) { deviceListeners.add(fn); },
    removeEventListener(_event, fn) { deviceListeners.delete(fn); },
    gatt: {
      connected: false,
      async connect() { this.connected = true; connects++; return this; },
      disconnect() { this.connected = false; for (const fn of [...deviceListeners]) fn(); },
      async getPrimaryService(uuid) {
        if (uuid === NUS_SERVICE && nus) return { getCharacteristic: async (id) => id === NUS_WRITE ? writer : notify };
        if (uuid === 0x180f) return { getCharacteristic: async () => battery };
        if (uuid === instance.SERVICE_UUID && smp && enabled && (!staleServices || connects > 1)) return { getCharacteristic: async () => smpChar };
        throw new DOMException("Service not found", "NotFoundError");
      },
    },
  };
  return { instance, device, commands, delays, smpChar, notify, battery,
    get connects() { return connects; },
    forgetSmp() { enabled = false; },
    bluetooth: { requestDevice: async (options) => { permissionOptions = options; return device; } },
    get permissions() { return permissionOptions; },
  };
}

async function withBluetooth(watch, task) {
  const descriptor = Object.getOwnPropertyDescriptor(globalThis, "navigator");
  Object.defineProperty(globalThis, "navigator", { configurable: true, value: { bluetooth: watch.bluetooth } });
  try { await task(); }
  finally {
    await watch.instance.disconnect();
    if (descriptor) Object.defineProperty(globalThis, "navigator", descriptor);
    else delete globalThis.navigator;
  }
}

test("BLE requests NUS/DIS/BAS permissions, detects hardware and enables DFU before discovering SMP", async () => {
  const watch = bleWatch();
  await withBluetooth(watch, async () => {
    await watch.instance.connect({ transport: "ble", filters: [{ namePrefix: "ZSWatch" }] });
    assert.deepEqual(watch.permissions.optionalServices, [NUS_SERVICE, 0x180a, 0x180f, watch.instance.SERVICE_UUID]);
    assert.deepEqual(watch.commands, [{ t: "smp", status: true }, { t: "ver" }]);
    assert.equal(watch.instance.deviceInfo.hardwareVersion, "watchdk@1/nrf5340/cpuapp");
    assert.equal(watch.instance.deviceInfo.batteryLevel, 15);
    assert.equal(watch.delays.includes(2000), false, "No unconditional registration wait");
    assert.equal(watch.commands[0].t, "smp", "Request fast interval before metadata");
    assert.ok(watch.instance._characteristic);
    watch.forgetSmp();
    await watch.instance.ensureUpdateReady();
    assert.equal(watch.commands.filter((cmd) => cmd.t === "smp").length, 2);
    assert.equal(watch.smpChar.listeners.size, 1);
    watch.battery.emit(Uint8Array.of(50));
    assert.equal(watch.instance.deviceInfo.batteryLevel, 50);
  });
  assert.equal(watch.notify.listeners.size, 0);
  assert.equal(watch.battery.listeners.size, 0);
});

test("BLE explains protected NUS access failures without sending DFU commands", async () => {
  const watch = bleWatch();
  const failure = new DOMException("GATT connection lost during pairing", "NetworkError");
  watch.notify.startNotifications = async () => { throw failure; };
  await withBluetooth(watch, async () => {
    await assert.rejects(watch.instance.connect({ transport: "ble" }), (error) => {
      assert.match(error.message, /Settings → Bluetooth.*Pairable/);
      assert.equal(error.cause, failure);
      return true;
    });
    assert.equal(watch.commands.length, 0);
    assert.equal(watch.device.gatt.connected, false);
    assert.equal(watch.notify.listeners.size, 0);
  });
});

test("BLE refreshes cached services; SMP-only and older watches have a fallback", async () => {
  const stale = bleWatch({ staleServices: true });
  await withBluetooth(stale, async () => {
    await stale.instance.connect({ transport: "ble" });
    assert.equal(stale.connects, 2);
    assert.equal(stale.commands.filter((cmd) => cmd.t === "smp").length, 2);
  });
  const legacy = bleWatch({ nus: false });
  await withBluetooth(legacy, async () => {
    await legacy.instance.connect({ transport: "ble" });
    assert.ok(legacy.instance._characteristic);
    assert.deepEqual(legacy.commands, []);
  });
  const missing = bleWatch({ nus: false, smp: false });
  await withBluetooth(missing, async () => {
    await assert.rejects(missing.instance.connect({ transport: "ble" }), /Enable Bluetooth in System/);
    assert.equal(missing.device.gatt.connected, false);
  });
});

test("restart tolerates reset disconnect, reconnects, re-enables SMP, and reads installed images", async () => {
  const watch = bleWatch({ resetDropsLink: true });
  await withBluetooth(watch, async () => {
    await watch.instance.connect({ transport: "ble" });
    const stages = [];
    const result = await watch.instance.restartAndReconnect((stage) => stages.push(stage));
    assert.equal(result.verified, true);
    assert.equal(watch.connects, 2);
    assert.equal(watch.commands.filter((cmd) => cmd.t === "smp").length, 2);
    assert.deepEqual(stages, ["Restarting watch", "Reconnecting to watch", "Verifying installed images"]);
    verifyInstalledImages([{ name: "app.internal.bin", imageNumber: 0, hash: "ab".repeat(32) }], result.images);
    assert.throws(() => verifyInstalledImages([{ name: "app.internal.bin", imageNumber: 0, hash: "cd".repeat(32) }], result.images), /could not be verified/);
  });
});

test("USB restart reports that verification needs a reconnect instead of claiming success", async () => {
  const instance = manager();
  instance.setTransport("serial");
  instance.request = async () => ({});
  const result = await instance.restartAndReconnect();
  assert.equal(result.verified, false);
  assert.match(result.message, /reconnect over USB/);
});

test("USB disconnect releases writer/reader locks before closing the port", async () => {
  const instance = new MCUManager({ transport: "serial", logger: { info() {}, error() {} } });
  const actions = [];
  instance._serialReadLoopActive = true;
  instance._serialPort = { close: async () => actions.push("close port") };
  instance._serialReader = { cancel: async () => actions.push("cancel reader") };
  instance._serialReadLoopPromise = Promise.resolve().then(() => actions.push("reader released"));
  instance._serialWriter = { releaseLock: () => actions.push("release writer") };
  await instance.disconnect();
  assert.deepEqual(actions, ["cancel reader", "reader released", "release writer", "close port"]);
  assert.equal(instance.isConnected(), false);
});

test("USB hardware uses optional OS info and safely falls back on older firmware/recovery", async () => {
  const instance = new MCUManager({ transport: "serial", logger: { info() {}, error() {} } });
  const requests = [];
  instance.request = async (...args) => { requests.push(args); return { output: "zswatch_legacy@5" }; };
  await instance.readDeviceInfo(false);
  assert.equal(instance.deviceInfo.hardwareVersion, "zswatch_legacy@5");
  assert.deepEqual(requests[0], [0, 0, 7, { format: "i" }, 2500]);
  await instance.readDeviceInfo(true);
  assert.equal(requests.length, 1);
  instance.request = async () => { throw new Error("Unsupported command"); };
  await instance.readDeviceInfo(false);
});

test("firmware-only applies automatically; filesystem-only restarts after acknowledged upload", async () => {
  const actions = [];
  await runFirmwareUpdate(workflow({ mode: "firmware", transfer: async () => actions.push("firmware"), applyFirmware: async () => actions.push("apply") }));
  assert.deepEqual(actions, ["firmware", "firmware", "apply"]);
  actions.length = 0;
  let acknowledge;
  const task = runFirmwareUpdate(workflow({ mode: "filesystem", transfer: async () => { actions.push("filesystem"); await new Promise((resolve) => { acknowledge = resolve; }); }, restartFilesystem: async () => actions.push("restart") }));
  await new Promise(setImmediate);
  assert.deepEqual(actions, ["filesystem"]);
  acknowledge(); await task;
  assert.deepEqual(actions, ["filesystem", "restart"]);
});

const imageInfo = async (data) => ({ version: "1.2.3", hash: "ab".repeat(32), firstByte: new Uint8Array(data)[0] });
async function zipFile(entries, name = "firmware.zip") {
  const zip = new JSZip();
  for (const [key, value] of Object.entries(entries)) zip.file(key, value);
  return new File([await zip.generateAsync({ type: "nodebuffer" })], name);
}
async function dfu(marker, filename = "app.internal.bin", index = "0") {
  return zipFile({ [filename]: Uint8Array.of(marker), "manifest.json": JSON.stringify({ files: [{ file: filename, image_index: index, board: "watchdk@1" }] }) }, "dfu_application.zip");
}

test("complete packages unpack nested standard/rotated DFU and filesystem", async () => {
  const standard = await dfu(1), rotated = await dfu(2);
  const outer = await zipFile({ "build/dfu_application.zip": await standard.arrayBuffer(), "build/dfu_application_rotated.zip": await rotated.arrayBuffer(), "build/lvgl_resources_raw.bin": Uint8Array.of(3), "build/zephyr.bin": Uint8Array.of(4) });
  const normal = await readFirmwarePackage(outer, imageInfo);
  const rotation = await readFirmwarePackage(outer, imageInfo, { rotated: true });
  assert.equal(normal.images.length, 1);
  assert.equal(normal.images[0].firstByte, 1);
  assert.equal(rotation.images[0].firstByte, 2);
  assert.equal(rotation.variant, "Rotated");
  assert.equal(normal.images[0].board, "watchdk@1");
  assert.equal(normal.filesystem.size, 1);
});

test("manifest slots take precedence over filenames, string indices work", async () => {
  const file = await dfu(1, "nested/custom.signed.bin", "2");
  const loaded = await readFirmwarePackage(file, imageInfo);
  assert.equal(loaded.images[0].imageNumber, 2);
});

test("legacy DFU resolves paths and ignores unrelated binaries", async () => {
  const file = await zipFile({ "dir/app.internal.bin": Uint8Array.of(1), "dir/zephyr.bin": Uint8Array.of(2), "dir/ipc_radio.bin": Uint8Array.of(3) });
  const loaded = await readFirmwarePackage(file, imageInfo, { imageNumber: 0 });
  assert.deepEqual(loaded.images.map((image) => image.imageNumber), [0, 1]);
});

test("missing requested variant and invalid manifests are rejected", async () => {
  const standard = await dfu(1);
  await assert.rejects(readFirmwarePackage(await zipFile({ "dfu_application.zip": await standard.arrayBuffer() }), imageInfo, { rotated: true }), /Rotated firmware is missing/);
  await assert.rejects(readFirmwarePackage(await dfu(1, "app.internal.bin", "bad"), imageInfo), /Invalid image index/);
  await assert.rejects(readFirmwarePackage(await zipFile({ "manifest.json": '{"files":{}}' }), imageInfo), /must be an array/);
});

test("duplicate slots, malformed images, and empty packages fail atomically", async () => {
  const entries = { files: [{ file: "a.bin", image_index: 0 }, { file: "b.bin", image_index: 0 }] };
  await assert.rejects(readFirmwarePackage(await zipFile({ "a.bin": Uint8Array.of(1), "b.bin": Uint8Array.of(2), "manifest.json": JSON.stringify(entries) }), imageInfo), /Multiple binaries/);
  await assert.rejects(readFirmwarePackage(await dfu(1), async () => { throw new Error("Invalid image"); }), /Invalid image/);
  await assert.rejects(readFirmwarePackage(await zipFile({ "readme.txt": "nothing" }), imageInfo), /No supported firmware/);
  await assert.rejects(readFirmwarePackage(new File([], "lvgl_resources_raw.bin"), imageInfo), /empty/);
});

test("standalone BIN uses selected index; resources are not treated as firmware", async () => {
  const binary = await readFirmwarePackage(new File([Uint8Array.of(1)], "custom.bin"), imageInfo, { imageNumber: 2 });
  assert.equal(binary.images[0].imageNumber, 2);
  const resources = await readFirmwarePackage(new File([Uint8Array.of(3)], "lvgl_resources_raw.bin"), imageInfo);
  assert.equal(resources.images.length, 0);
  assert.equal(resources.filesystem.size, 1);
});

function response(packet, data) {
  const payload = new Uint8Array(CBOR.encode(data));
  return Uint8Array.from([packet[0] + 1, 0, payload.length >> 8, payload.length & 255, packet[4], packet[5], packet[6], packet[7], ...payload]);
}
function manager() {
  const instance = new MCUManager({ logger: { info() {}, error() {} } });
  instance.isConnected = () => true;
  return instance;
}

test("upload waits for all acknowledged chunks and delivers 100%", async (t) => {
  const instance = manager();
  t.after(() => instance.cancelUpload());
  const packets = [], percentages = [];
  instance._writeRaw = async (packet) => { packets.push(packet); };
  let finished = false;
  const upload = instance.upload(new ArrayBuffer(1000), 2, false, ({ percentage }) => percentages.push(percentage)).then(() => { finished = true; });
  await new Promise(setImmediate);
  assert.equal(finished, false);
  for (let index = 0; !finished; index++) {
    assert.ok(index < 20);
    for (let wait = 0; !packets[index] && wait < 100; wait++) await new Promise((resolve) => setTimeout(resolve, 1));
    const packet = packets[index];
    assert.ok(packet, "Expected another upload chunk");
    const data = CBOR.decode(packet.slice(8).buffer);
    assert.equal(data.image, 2);
    instance._processMessage(response(packet, { off: data.off + data.data.length }));
    await new Promise(setImmediate);
  }
  await upload;
  assert.equal(percentages.at(-1), 100);
  assert.ok(packets.length > 1);
});

test("watch errors and cancellation reject uploads without lingering state", async (t) => {
  const instance = manager();
  t.after(() => instance.cancelUpload());
  let packet;
  instance._writeRaw = async (value) => { packet = value; };
  const upload = instance.upload(new ArrayBuffer(100), 0, true);
  const rejected = assert.rejects(upload, /Upload rejected/);
  await new Promise(setImmediate);
  instance._processMessage(response(packet, { err: { group: 8, rc: 5 } }));
  await rejected;
  assert.equal(instance._uploadIsInProgress, false);
  const cancelled = instance.upload(new ArrayBuffer(100), 0, true);
  const cancellation = assert.rejects(cancelled, /cancelled/);
  instance.cancelUpload();
  await cancellation;
  assert.equal(instance._transfer, null);
  instance._processMessage(response(packet, { off: 100 }));
  assert.equal(instance._transfer, null);
});

test("invalid offsets fail, zero offsets retry, and unrelated responses are ignored", async (t) => {
  const instance = manager();
  t.after(() => instance.cancelUpload());
  const packets = [];
  instance._writeRaw = async (packet) => packets.push(packet);
  const upload = instance.upload(new ArrayBuffer(100), 0, true);
  const rejected = assert.rejects(upload, /Invalid upload offset/);
  await new Promise(setImmediate);
  const unrelated = response(packets[0], { off: 100 }); unrelated[6]++;
  instance._processMessage(unrelated);
  assert.ok(instance._transfer);
  instance._processMessage(response(packets[0], { off: 0 }));
  await new Promise(setImmediate);
  assert.equal(packets.length, 2);
  instance._processMessage(response(packets[1], { off: 101 }));
  await rejected;
});

test("commands wait for matching acknowledgement and reject SMP errors", async (t) => {
  const instance = manager();
  t.after(() => instance.cancelUpload());
  let packet;
  instance._writeRaw = async (value) => { packet = value; };
  const confirmed = instance.request(2, 1, 0, { confirm: true });
  const wrong = response(packet, {}); wrong[6]++;
  instance._processMessage(wrong);
  assert.ok(instance._request);
  instance._processMessage(response(packet, { images: [] }));
  assert.deepEqual(await confirmed, { images: [] });
  const reset = instance.request(2, 0, 5);
  const rejected = assert.rejects(reset, /Command rejected/);
  instance._processMessage(response(packet, { rc: 3 }));
  await rejected;
});

test("acknowledged image reads still update the firmware UI message listener", async () => {
  const instance = manager();
  let packet;
  const messages = [];
  instance.onMessage((message) => messages.push(message));
  instance._writeRaw = async (value) => { packet = value; };
  const reading = instance.request(0, 1, 0);
  const images = [{ image: 0, slot: 0, version: "0.8.1" }];
  instance._processMessage(response(packet, { images }));
  assert.deepEqual((await reading).images, images);
  assert.equal(messages.length, 1);
  assert.deepEqual(messages[0].data.images, images);
  assert.equal(messages[0].op, 1);
});

test("disconnect and write failures reject pending work; simultaneous uploads fail", async (t) => {
  const instance = manager();
  t.after(() => instance.cancelUpload());
  instance._writeRaw = async () => {};
  const upload = instance.upload(new ArrayBuffer(100), 0, true);
  const rejected = assert.rejects(upload, /disconnected/);
  await assert.rejects(instance.upload(new ArrayBuffer(100)), /in progress/);
  await instance._disconnected();
  await rejected;
  instance._writeRaw = async () => { throw new Error("Write failed"); };
  await assert.rejects(instance.upload(new ArrayBuffer(100), 0, true), /Write failed/);
});

function workflow(overrides = {}) {
  return { selection: { images: [{ name: "app.internal.bin", imageNumber: 0, fileData: new ArrayBuffer(10) }, { name: "ipc_radio.bin", imageNumber: 1, fileData: new ArrayBuffer(10) }], filesystem: new File([Uint8Array.of(3)], "lvgl_resources_raw.bin") }, mode: "both", recovery: false, signal: new AbortController().signal, transfer: async () => {}, applyFirmware: async () => {}, restartFilesystem: async () => {}, onImageUploaded() {}, log() {}, ...overrides };
}

test("full update waits for filesystem, uploads images in order, then applies", async () => {
  const actions = [];
  let releaseFS;
  const task = runFirmwareUpdate(workflow({ transfer: async (_data, slot, stage) => { actions.push(stage); if (stage === "Uploading filesystem") await new Promise((resolve) => { releaseFS = resolve; }); }, applyFirmware: async () => actions.push("apply") }));
  await new Promise(setImmediate);
  assert.deepEqual(actions, ["Uploading filesystem"]);
  releaseFS(); await task;
  assert.deepEqual(actions, ["Uploading filesystem", "Uploading app.internal.bin", "Uploading ipc_radio.bin", "apply"]);
});

test("filesystem failure or cancellation never starts firmware or confirmation", async () => {
  let applied = false;
  await assert.rejects(runFirmwareUpdate(workflow({ transfer: async () => { throw new Error("FS failed"); }, applyFirmware: async () => { applied = true; } })), /FS failed/);
  assert.equal(applied, false);
  const controller = new AbortController();
  const actions = [];
  await assert.rejects(runFirmwareUpdate(workflow({ signal: controller.signal, transfer: async (_data, _slot, stage) => { actions.push(stage); controller.abort(); } })), /cancelled/);
  assert.deepEqual(actions, ["Uploading filesystem"]);
});

test("recovery remaps slots and waits for network core; filesystem is unavailable", async () => {
  const slots = []; let waited = false;
  await runFirmwareUpdate(workflow({ mode: "firmware", recovery: true, transfer: async (_data, slot) => slots.push(slot), waitForNetCore: async () => { waited = true; } }));
  assert.deepEqual(slots, [1, 3]); assert.equal(waited, true);
  await assert.rejects(runFirmwareUpdate(workflow({ recovery: true })), /application mode/);
});

test("hardware filters distinguish boards, and release results exclude unrelated assets", async () => {
  const previousFetch = global.fetch;
  try {
    assert.equal(boardFromAsset("zswatch_legacy@5_nrf5340_release.zip"), "zswatch_legacy@5");
    assert.equal(compatibleAssets([{ name: "watchdk@1_debug" }, { name: "zswatch@1_release" }], "watchdk@1").length, 1);
    global.fetch = async () => ({ ok: true, json: async () => [{ id: 1, assets: [{ name: "watchdk@1_debug.zip" }, { name: "sources.zip" }] }, { id: 2, draft: true, assets: [{ name: "watchdk@1_debug.zip" }] }] });
    const releases = await fetchReleases({ owner: "ZSWatch", repo: "ZSWatch" });
    assert.equal(releases.length, 1); assert.equal(releases[0].assets.length, 1);
    global.fetch = async () => ({ ok: false, status: 403 });
    await assert.rejects(fetchReleases({ owner: "ZSWatch", repo: "ZSWatch" }), /rate limit/);
  } finally { global.fetch = previousFetch; }
});

test("CI discovery queries only the firmware workflow and deduplicates runs and branches", async () => {
  const previousFetch = global.fetch;
  try {
    const run = (id, branch) => ({ id, head_branch: branch, conclusion: "success", actor: { login: "dev" } });
    const requests = [];
    global.fetch = async (url) => { requests.push(url); return { ok: true, json: async () => {
      if (url.includes("/artifacts")) return { artifacts: url.includes("/10/") ? [] : [{ id: 1, name: "watchdk@1_debug", expired: false }, { id: 2, name: "zswatch@1_release", expired: true }] };
      if (url.includes("branch=main")) return { workflow_runs: [run(10, "main"), run(9, "main")] };
      return { workflow_runs: [run(10, "main"), run(8, "feature"), run(7, "feature"), run(6, "main"), { ...run(5, "failed"), conclusion: "failure" }, run(4, "gh-pages")] };
    } }; };
    const builds = await fetchArtifacts({ owner: "ZSWatch", repo: "ZSWatch" });
    assert.deepEqual(builds.map((run) => [run.branch, run.runId]), [["main", 9], ["feature", 8]]);
    assert.equal(builds[0].artifacts.length, 1);
    const discovery = requests.filter((url) => !url.includes("/artifacts"));
    assert.equal(discovery.length, 2);
    assert.ok(discovery.every((url) => url.includes("/actions/workflows/ci.yml/runs?")));
    assert.ok(discovery.every((url) => url.includes("status=success")));
    assert.ok(discovery.some((url) => url.includes("branch=main") && url.includes("per_page=10")));
    assert.ok(discovery.some((url) => url.includes("per_page=50")));
    assert.deepEqual(requests.filter((url) => url.includes("/artifacts")).map((url) => Number(url.match(/runs\/(\d+)/)[1])), [10, 9, 8]);
    requests.length = 0;
    assert.equal((await fetchArtifacts({ owner: "ZSWatch", repo: "ZSWatch" }, 1)).length, 1);
    assert.equal(requests.filter((url) => url.includes("/artifacts")).length, 2);
  } finally { global.fetch = previousFetch; }
});


test("applying images waits for each confirmation before restarting", async () => {
  const stages = [], requests = [];
  let acknowledge;
  const mcumgr = { request: async (_op, group, id, data) => {
    requests.push({ group, id, data });
    if (requests.length === 1) await new Promise((resolve) => { acknowledge = resolve; });
  } };
  const images = [{ name: "app.internal.bin", hash: "ab".repeat(32) }, { name: "app.external.bin", hash: "cd".repeat(32) }];
  const task = applyUploadedFirmware({ mcumgr, images, recovery: false, onStage: (stage) => stages.push(stage) });
  assert.equal(requests.length, 1);
  acknowledge(); await task;
  assert.deepEqual(requests.map(({ group, id }) => [group, id]), [[1, 0], [1, 0], [0, 5]]);
  assert.equal(requests[0].data.confirm, true);
  assert.equal(requests[0].data.hash[0], 0xab);
  assert.deepEqual(stages, ["Confirming firmware", "Restarting watch"]);
});

test("failed confirmation never restarts; recovery only requests a restart", async () => {
  const requests = [];
  const images = [{ name: "app.internal.bin", hash: "ab".repeat(32) }];
  await assert.rejects(applyUploadedFirmware({ mcumgr: { request: async (_op, group) => { requests.push(group); throw new Error("Confirm failed"); } }, images, recovery: false, onStage() {} }), /Confirm failed/);
  assert.deepEqual(requests, [1]);
  requests.length = 0;
  await applyUploadedFirmware({ mcumgr: { request: async (_op, group, id) => requests.push([group, id]) }, images, recovery: true, onStage() {} });
  assert.deepEqual(requests, [[0, 5]]);
});

test("upload resumes at the device's initial offset", async (t) => {
  const instance = manager(); t.after(() => instance.cancelUpload());
  const packets = [];
  instance._writeRaw = async (packet) => packets.push(packet);
  const task = instance.upload(new ArrayBuffer(4000), 0, true);
  instance._processMessage(response(packets[0], { off: 3000 }));
  const resumed = CBOR.decode(packets[1].slice(8).buffer);
  assert.equal(resumed.off, 3000);
  instance._processMessage(response(packets[1], { off: 4000 }));
  await task;
});

test("unresponsive uploads fail after bounded retries and commands time out", async (t) => {
  t.mock.timers.enable({ apis: ["setTimeout"] });
  const instance = manager();
  t.after(() => instance.cancelUpload());
  let packets = 0;
  instance._writeRaw = async () => { packets++; };
  const task = instance.upload(new ArrayBuffer(100), 0, true);
  const rejected = assert.rejects(task, /stopped responding/);
  for (let retry = 0; retry < 4; retry++) t.mock.timers.tick(10000);
  await rejected;
  assert.equal(packets, 4);
  const command = instance.request(2, 0, 5);
  const commandRejected = assert.rejects(command, /did not acknowledge/);
  t.mock.timers.tick(10000);
  await commandRejected;
});

test("invalid archives and JSON report useful package errors", async () => {
  await assert.rejects(readFirmwarePackage(new File(["not a zip"], "bad.zip"), imageInfo), /Invalid ZIP file/);
  await assert.rejects(readFirmwarePackage(await zipFile({ "manifest.json": "bad json" }), imageInfo), /manifest is invalid/);
  await assert.rejects(readFirmwarePackage(await zipFile({ "manifest.json": "null" }), imageInfo), /files must be an array/);
});


test("ZIP-only firmware selection rejects standalone files unless advanced mode is enabled", async () => {
  const file = new File([Uint8Array.of(1)], "zephyr.bin");
  await assert.rejects(readFirmwarePackage(file, () => ({}), { allowStandalone: false }), /Enable Advanced/);
  const result = await readFirmwarePackage(file, () => ({}), { allowStandalone: true, imageNumber: 2 });
  assert.equal(result.images[0].imageNumber, 2);
});

test("shared manager keeps BLE identity, stops capture for firmware and propagates reboot state", async () => {
  const watch = bleWatch({ shared: true, resetDropsLink: true });
  let callbacks = [];
  watch.instance.onConnect(async (event) => {
    callbacks.push(event.reconnecting);
    if (!event.reconnecting) await watch.instance.request(0, 1, 0);
  });
  await withBluetooth(watch, async () => {
    await watch.instance.connect({ transport: "ble" });
    assert.equal(watch.instance.isConnected(), true);
    assert.equal(watch.instance.name, "ZSWatch Test");
    assert.deepEqual(callbacks, [false]);
    assert.equal(watch.commands.find((command) => command.t === "smp").status, true);
    const device = watch.instance._device;
    const info = watch.instance.deviceInfo;
    watch.instance._bleReadyAt = 0;
    await watch.instance.setLogCapture(true);
    assert.deepEqual(watch.commands.slice(-2), [{ t: "smp", status: false }, { t: "log", status: true }]);
    await watch.instance.ensureUpdateReady();
    assert.equal(watch.instance._device, device);
    assert.equal(watch.instance.deviceInfo.hardwareVersion, info.hardwareVersion);
    assert.equal(watch.instance.logState, "off");
    assert.ok(watch.commands.some((command) => command.t === "log" && !command.status));
    const result = await watch.instance.restartAndReconnect();
    assert.equal(result.verified, true);
    assert.deepEqual(callbacks, [false, true]);
    assert.equal(watch.instance.isConnected(), true);
    assert.equal(result.images.length, 1);
  });
});

const WatchBluetooth = require("../src/components/FirmwareUpdate/utils/watchBluetooth").default;

test("Connect always opens the filtered chooser, including after a previous connection", async () => {
  const watch = bleWatch();
  watch.device.id = "previously-selected-watch";
  watch.bluetooth.getDevices = async () => assert.fail("Saved grants must not bypass the chooser");
  const requestDevice = watch.bluetooth.requestDevice;
  let choices = 0;
  watch.bluetooth.requestDevice = async (options) => { choices++; return requestDevice(options); };
  await withBluetooth(watch, async () => {
    for (const prefix of ["ZSWatch", "ZSWatch Test"]) {
      await watch.instance.connect({ transport: "ble", filters: [{ namePrefix: prefix }] });
      assert.deepEqual(watch.permissions.filters, [{ namePrefix: prefix }]);
      assert.equal(watch.instance.isConnected(), true);
      await watch.instance.disconnect();
    }
    assert.equal(choices, 2);
  });
});

test("idle SMP keepalive refreshes the lease without reconnecting or rereading metadata", async () => {
  const watch = bleWatch({ shared: true });
  await withBluetooth(watch, async () => {
    await watch.instance.connect({ transport: "ble" });
    const connects = watch.connects, versions = watch.commands.filter(c => c.t === "ver").length;
    watch.instance._watch.smpActivityAt = Date.now() - 60000;
    await watch.instance.keepSmpAlive();
    assert.equal(watch.connects, connects);
    assert.equal(watch.commands.at(-1).t, "smp"); assert.equal(watch.commands.at(-1).status, true);
    assert.ok(Date.now() - watch.instance._watch.smpActivityAt < 1000);
    assert.equal(watch.commands.filter(c => c.t === "ver").length, versions);
  });
});

test("keepalive recovers expired SMP with fresh subscriptions on the same watch", async () => {
  const watch = bleWatch({ shared: true });
  await withBluetooth(watch, async () => {
    await watch.instance.connect({ transport: "ble" });
    const connects = watch.connects;
    watch.forgetSmp(); watch.instance._watch.smpActivityAt = Date.now() - 180001;
    await watch.instance.keepSmpAlive();
    assert.equal(watch.connects, connects + 1);
    assert.equal(watch.instance._device, watch.device);
    assert.equal(watch.instance._watch.smpEnabled, true);
    assert.equal(watch.smpChar.listeners.size, 1);
    const result = await watch.instance.request(0, 1, 0);
    assert.equal(result.images.length, 1);
  });
});

test("keepalive never re-enables intentionally disabled SMP or interrupts log capture and transfers", async () => {
  const watch = bleWatch({ shared: true });
  await withBluetooth(watch, async () => {
    await watch.instance.connect({ transport: "ble" });
    const initial = watch.commands.length;
    for (const key of ["_logRequested", "_request", "_transfer", "_rediscovering", "_preparingSmp"]) {
      watch.instance[key] = true; await watch.instance.keepSmpAlive(); watch.instance[key] = null;
    }
    await watch.instance._watch.send({ t: "smp", status: false });
    await watch.instance.keepSmpAlive();
    assert.equal(watch.commands.length, initial + 1);
    assert.equal(watch.commands.at(-1).status, false);
    watch.instance._watch.smpEnabled = true;
    watch.instance.setTransport("serial"); await watch.instance.keepSmpAlive(); watch.instance.setTransport("ble");
    await watch.instance.disconnect(); await watch.instance.keepSmpAlive();
    assert.equal(watch.commands.length, initial + 1);
  });
});

test("real SMP packet writes refresh idle activity, while rejected commands do not fake a lease", async () => {
  const watch = bleWatch({ shared: true });
  await withBluetooth(watch, async () => {
    await watch.instance.connect({ transport: "ble" });
    watch.instance._watch.smpActivityAt = 1;
    const write = watch.instance._writeRaw.bind(watch.instance);
    let written;
    watch.instance._writeRaw = packet => (written = write(packet));
    await watch.instance.request(0, 1, 0);
    await written;
    assert.ok(watch.instance._watch.smpActivityAt > 1);
    watch.instance._watch.smpActivityAt = 1;
    watch.instance._watch.write = { properties: { write: true }, writeValueWithResponse: async () => { throw new Error("write failed"); } };
    await assert.rejects(watch.instance.keepSmpAlive(), /write failed/);
    assert.equal(watch.instance._watch.smpActivityAt, 1);
    assert.equal(watch.instance._preparingSmp, false);
  });
});

test("simultaneous keepalive and UI commands retain whole-command order on fragmented NUS", async () => {
  const watch = new WatchBluetooth(() => {}, async () => {}), chunks = [];
  watch.write = { properties: { write: true }, writeValueWithResponse: async bytes => { chunks.push(new TextDecoder().decode(bytes)); await Promise.resolve(); } };
  const a = { t: "smp", status: true }, b = { t: "watchface_bg", action: "reset" };
  await Promise.all([watch.send(a), watch.send(b)]);
  assert.equal(chunks.join(""), `GB(${JSON.stringify(a)})GB(${JSON.stringify(b)})`);
  watch.cleanup();
});

test("disconnect invalidates queued NUS commands before a new connection can receive them", async () => {
  const watch = new WatchBluetooth(() => {}, async () => {});
  let finish, started;
  const ready = new Promise(resolve => { started = resolve; });
  watch.write = { properties: { write: true }, writeValueWithResponse: () => new Promise(resolve => { finish = resolve; started(); }) };
  const first = assert.rejects(watch.send({ t: "smp", status: true }), /disconnected/);
  const pending = assert.rejects(watch.send({ t: "log", status: true }), /disconnected/);
  await ready; watch.cleanup();
  const received = [];
  watch.write = { properties: { write: true }, writeValueWithResponse: async bytes => received.push(new TextDecoder().decode(bytes)) };
  const fresh = watch.send({ t: "ver" });
  await fresh;
  assert.equal(received.join(""), 'GB({"t":"ver"})', "New connection does not wait for an old write to reject");
  finish(); await Promise.all([first, pending, fresh]);
  assert.equal(received.join(""), 'GB({"t":"ver"})');
  assert.equal(watch.smpEnabled, false);
  watch.cleanup();
});

// Exercise hook callbacks with persistent state/refs and explicit rerenders.
// Effects are omitted: these tests cover package loading, not browser lifecycle.
function hookHarness(relativePath, exportName, args, dependencies = {}) {
  const filename = path.resolve(__dirname, relativePath);
  const localRequire = require("node:module").createRequire(filename);
  const slots = [];
  let cursor = 0;
  const hooks = {
    useState(initial) {
      const index = cursor++;
      if (!(index in slots)) slots[index] = initial;
      return [slots[index], (next) => { slots[index] = typeof next === "function" ? next(slots[index]) : next; }];
    },
    useRef(initial) {
      const index = cursor++;
      if (!(index in slots)) slots[index] = { current: initial };
      return slots[index];
    },
    useEffect() {},
    useCallback(callback) { return callback; },
  };
  const { code } = babel.transformSync(fs.readFileSync(filename, "utf8"), {
    filename, configFile: false, babelrc: false,
    plugins: [require.resolve("@babel/plugin-transform-modules-commonjs")],
  });
  const exports = {};
  new Function("require", "exports", code)((name) => {
    if (name === "react") return hooks;
    if (name === "@docusaurus/router") return { useHistory: () => ({}) };
    return dependencies[name] || localRequire(name);
  }, exports);
  return () => { cursor = 0; return exports[exportName](...args); };
}

test("package loading distinguishes success, parse failure and busy without replacing the current package", async () => {
  let finishPreparing;
  const render = hookHarness("../src/components/FirmwareUpdate/hooks/useFirmwareUpdate.js", "useFirmwareUpdate", [{
    imageInfo,
    ensureUpdateReady: () => new Promise((resolve) => { finishPreparing = resolve; }),
    cancelUpload() {},
  }, false, true]);
  const valid = await dfu(1);
  const loaded = await render().loadFile(valid);
  assert.equal(loaded.name, valid.name);
  assert.equal(render().selection, loaded);
  assert.equal(await render().loadFile(new File(["not a zip"], "bad.zip")), false);
  assert.equal(render().selection, loaded);
  assert.match(render().error, /Invalid ZIP/);

  let finishReading;
  const reading = render().loadFile({ name: valid.name, arrayBuffer: () => new Promise((resolve) => { finishReading = resolve; }) });
  const readingStatus = render().status;
  assert.equal(await render().loadFile(valid), null);
  assert.equal(render().status, readingStatus);
  finishReading(await valid.arrayBuffer());
  await reading;

  const updating = render().handleFileUpload();
  const updatingStatus = render().status;
  assert.equal(render().activity, "updating");
  assert.equal(await render().loadFile(valid), null);
  assert.equal(render().status, updatingStatus);
  render().cancel();
  finishPreparing();
  await updating;
  assert.match(render().error, /cancelled/);
});

test("release catalog marks only loaded packages selected and preserves status when the loader is busy", async () => {
  let result = { name: "good.zip" };
  const file = new File(["package"], "release.zip");
  const render = hookHarness("../src/components/FirmwareUpdate/hooks/useFirmwareManagement.js", "useFirmwareManagement", [{}, async () => result], {
    "../utils/githubApi": { downloadRelease: async (_asset, _signal, progress) => { progress(100); return file; } },
  });
  await render().selectRelease({ id: 1, name: "good.zip" });
  assert.equal(render().selectedAssetId, 1);
  assert.equal(render().downloadStatus, "");
  result = false;
  await render().selectRelease({ id: 2, name: "bad.zip" });
  assert.equal(render().selectedAssetId, 1);
  assert.match(render().downloadStatus, /package could not be loaded/);
  assert.equal(render().fallbackAsset, null, "A parse failure does not suggest downloading the same file again");
  const previousStatus = render().downloadStatus;
  result = null;
  await render().selectRelease({ id: 3, name: "busy.zip" });
  assert.equal(render().selectedAssetId, 1);
  assert.equal(render().downloadStatus, previousStatus);
  assert.equal(render().isDownloading, false);
});
