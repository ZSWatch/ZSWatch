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
const { WatchStreamReader, default: WatchBluetooth } = require("../src/components/FirmwareUpdate/utils/watchBluetooth");
const Manager = require("../src/components/WatchTools/WatchToolsManager").default;
const { parseCrash, makeReport, addReport, reportBytes, loadHistory, saveHistory, crashFrequencies, logLines, LogLineReader, HISTORY_KEY } = require("../src/components/WatchTools/diagnostics");
const CBOR = require("../src/mcumgr-web/js/cbor");

function dump({ binary = true, file = "", line = 0, timestamp = "14:37 11/08", data = "5a45020003000500" } = {}) {
  const text = new TextEncoder().encode(`#CD:BEGIN#\r\nASSERT:${timestamp}\r\nFILE:${file}\r\nLINE:${line}\r\n#CD:${data}\r\n#CD:END#\r\n`);
  if (!binary) return text;
  const bytes = new Uint8Array(72 + text.length);
  bytes.set(new TextEncoder().encode(timestamp).slice(0, 32));
  bytes.set(new TextEncoder().encode(file).slice(0, 32), 33);
  new DataView(bytes.buffer).setInt32(68, line, true);
  bytes.set(text, 72); return bytes;
}

test("fragmented BLELOG tags and UTF-8 stay separate from protocol JSON", () => {
  const messages = [], logs = [];
  const reader = new WatchStreamReader((value) => messages.push(value), (value) => logs.push(value));
  const bytes = new TextEncoder().encode('noise {"t":"ver","hw":"watchdk@1"}<BLELOG>[123] <inf> ä🙂 {"t":"watchface_bg","ok":true}\r\n</BLELOG>{"t":"status","bat":88}<BLELOG>next</BLELOG>');
  for (const byte of bytes) reader.push(Uint8Array.of(byte));
  assert.deepEqual(messages, [{ t: "ver", hw: "watchdk@1" }, { t: "status", bat: 88 }]);
  assert.deepEqual(logs, ['[123] <inf> ä🙂 {"t":"watchface_bg","ok":true}\r\n', "next"]);
  assert.equal(reader.buffer, "");
});

test("unterminated streams are bounded and can recover for later notifications", () => {
  const messages = [], logs = [];
  const reader = new WatchStreamReader((value) => messages.push(value), (value) => logs.push(value));
  reader.push(new TextEncoder().encode("<BLELOG>" + "x".repeat(66000)));
  assert.equal(reader.buffer.length, 0);
  reader.push(new TextEncoder().encode("<BLELOG>" + "x".repeat(66000) + "</BLELOG>"));
  assert.equal(logs.length, 0);
  reader.push(new TextEncoder().encode('{"t":"ver"}<BLELOG>okay</BLELOG>'));
  assert.equal(messages[0].t, "ver"); assert.deepEqual(logs, ["okay"]);
  reader.push(new TextEncoder().encode("<BLE")); reader.reset();
  reader.push(new TextEncoder().encode('{"t":"status"}'));
  assert.equal(messages.at(-1).t, "status");
});

test("JSON inside a log cannot acknowledge a watchface command", async () => {
  const watch = new WatchBluetooth(() => {}, async () => {});
  watch.write = { properties: {}, writeValueWithResponse: async () => {} };
  const command = watch.request({ t: "watchface_bg", action: "apply" }, (message) => message.t === "watchface_bg", 15);
  watch.reader.push(new TextEncoder().encode('<BLELOG>{"t":"watchface_bg","ok":true}</BLELOG>'));
  await assert.rejects(command, /did not confirm/);
});

test("log severities and ANSI escapes preserve unknown messages", () => {
  assert.deepEqual(logLines("\x1b[31m[123] <err> boom\x1b[0m\r\n[124] <wrn> warning\nplain\n").map(({ text, level }) => ({ text, level })), [
    { text: "[123] <err> boom", level: "error" }, { text: "[124] <wrn> warning", level: "warning" }, { text: "plain", level: "unknown" },
  ]);
});

test("log lines span multiple BLE output frames without losing severity or ANSI parsing", () => {
  const reader = new LogLineReader();
  assert.deepEqual(reader.push("\x1b[3", "first"), []);
  assert.deepEqual(reader.push("1m[123] <er", "second"), []);
  const entries = reader.push("r> long message\x1b[0m\r\nnext", "third");
  assert.deepEqual(entries, [{ text: "[123] <err> long message", level: "error", receivedAt: "first" }]);
  assert.deepEqual(reader.flush(), [{ text: "next", level: "unknown", receivedAt: "third", partial: true }]);
  assert.deepEqual(reader.flush(), []);
  reader.push("old partial"); reader.reset();
  assert.equal(reader.push("new\n")[0].text, "new");
});

test("log line assembly bounds malformed data without discarding it", () => {
  const reader = new LogLineReader();
  const entries = reader.push("x".repeat(40000));
  assert.equal(entries.length, 2);
  assert.ok(entries.every((entry) => entry.partial));
  assert.ok(reader.pending.length <= 16384);
  assert.equal(entries.map((entry) => entry.text).join("").length + reader.flush()[0].text.length, 40000);
});

test("legacy binary and newer text dumps parse without inventing missing metadata", () => {
  const old = parseCrash(dump());
  assert.equal(old.timestamp, "14:37 11/08"); assert.equal(old.file, ""); assert.equal(old.line, null); assert.equal(old.dataBytes, 8);
  const newer = parseCrash(dump({ binary: false, file: "test.c", line: 123 }));
  assert.equal(newer.file, "test.c"); assert.equal(newer.line, 123); assert.equal(newer.format, "text");
  assert.throws(() => parseCrash(dump().slice(0, -20)), /incomplete/);
  assert.throws(() => parseCrash(dump({ data: "123z" })), /invalid data/);
  assert.throws(() => parseCrash(new Uint8Array(10)), /BEGIN/);
  assert.throws(() => parseCrash(new Uint8Array(1024 * 1024 + 1)), /1 MB/);
});

test("browser history deduplicates text exports, preserves originals and counts unique locations", async () => {
  const original = dump({ file: "test.c", line: 20 });
  const report = await makeReport(original, { firmwareVersion: "0.8.1" }, "/lvgl_lfs/coredump.txt");
  const textReport = await makeReport(new TextEncoder().encode(report.text));
  assert.equal(report.id, textReport.id);
  assert.deepEqual(reportBytes(report), original);
  const history = addReport(addReport([], report), textReport);
  assert.equal(history.length, 1);
  assert.deepEqual(crashFrequencies(history), [["test.c:20", 1]]);
  const saved = new Map(), storage = { getItem: (key) => saved.get(key), setItem: (key, value) => saved.set(key, value) };
  saveHistory(storage, history); assert.deepEqual(loadHistory(storage), history);
  storage.setItem(HISTORY_KEY, JSON.stringify([{ id: "bad", raw: "aaaa" }, null, ...history]));
  assert.deepEqual(loadHistory(storage), history);
  storage.setItem(HISTORY_KEY, '{"id":"bad"}');
  assert.throws(() => loadHistory(storage), /invalid/);
  const many = Array.from({ length: 22 }, (_, i) => ({ ...report, id: String(i) }));
  assert.equal(addReport(many, { ...report, id: "new" }).length, 20);
});

function transport({ path = "/lvgl_lfs/coredump.txt", data = dump(), malformed = "", error = null, v2 = false, onRead, eraseRet = 0, keepDump = false } = {}) {
  const manager = new Manager({ logger: { info() {}, error() {} } });
  manager._transport = "ble"; manager._device = { gatt: { connected: true } }; manager._mtu = 244;
  manager._buffer = new Uint8Array(); manager.sleep = async () => {};
  const commands = [], requests = [], traffic = [];
  let erased = false, raw = [];
  manager._watch = { send: async (command) => commands.push(command), subscribeCommands: async () => {}, cleanup() {} };
  manager.ensureSmp = async () => { manager.connectionGeneration++; commands.push({ t: "smp", status: true }); };
  manager.onTraffic((entry) => traffic.push(entry));
  manager._characteristic = { removeEventListener() {}, async writeValueWithoutResponse(bytes) {
    raw.push(...bytes);
    if (raw.length < 8 || raw.length < 8 + raw[2] * 256 + raw[3]) return;
    const packet = Uint8Array.from(raw); raw = [];
    const body = CBOR.decode(packet.slice(8).buffer), group = packet[4] * 256 + packet[5], id = packet[7];
    requests.push({ op: packet[0], group, id, body });
    let response;
    if (error) response = error;
    else if (group === 9) { erased = eraseRet === 0 && !keepDump; response = { ret: eraseRet }; }
    else if (body.name !== path || erased) response = v2 ? { err: { group: 8, rc: 3 } } : { rc: 5 };
    else if (id === 1) response = { len: data.length };
    else {
      response = { off: malformed === "offset" ? body.off + 1 : body.off, len: malformed === "length" ? data.length + 1 : data.length,
        data: malformed === "empty" ? new Uint8Array() : data.slice(body.off, body.off + 31) };
      onRead?.(manager, body);
    }
    const payload = new Uint8Array(CBOR.encode(response));
    const reply = Uint8Array.from([packet[0] + 1, 0, payload.length >> 8, payload.length & 255, packet[4], packet[5], packet[6], id, ...payload]);
    // Exercise actual SMP notification reassembly and response dispatch.
    for (let offset = 0; offset < reply.length; offset += 17) manager._notification({ target: { value: new DataView(reply.slice(offset, offset + 17).buffer) } });
  } };
  return { manager, commands, requests, traffic, get erased() { return erased; } };
}

test("SMP crash reads use both paths and acknowledge all bytes, without legacy upload handling", async () => {
  for (const path of ["/lvgl_lfs/coredump.txt", "/user/coredump.txt"]) {
    const mock = transport({ path }), progress = [];
    const result = await mock.manager.downloadCrash({ onProgress: (value) => progress.push(value.percentage) });
    assert.equal(result.path, path); assert.deepEqual(result.bytes, dump()); assert.equal(progress.at(-1), 100);
    assert.ok(mock.requests.some((request) => request.op === 0 && request.id === 0));
    assert.deepEqual(mock.commands.at(-1), { t: "smp", status: false });
    assert.equal(mock.erased, false);
    assert.ok(mock.traffic.some((entry) => entry.channel === "SMP" && entry.direction === "RX"));
    assert.ok(mock.traffic.some((entry) => entry.channel === "SMP" && entry.direction === "TX"));
  }
});

test("missing files differ from unsupported or failed commands in both SMP error formats", async () => {
  for (const v2 of [false, true]) assert.equal(await transport({ path: "/absent", v2 }).manager.downloadCrash(), null);
  for (const error of [{ rc: 8 }, { err: { group: 8, rc: 2 } }, { err: { group: 0, rc: 3 } }]) {
    const mock = transport({ error });
    await assert.rejects(mock.manager.downloadCrash(), /rejected/);
    assert.deepEqual(mock.commands.at(-1), { t: "smp", status: false });
  }
});

test("inconsistent or truncated crash data is rejected and SMP is released", async () => {
  for (const malformed of ["offset", "length", "empty"]) {
    const mock = transport({ malformed });
    await assert.rejects(mock.manager.downloadCrash(), /inconsistent/);
    assert.deepEqual(mock.commands.at(-1), { t: "smp", status: false });
  }
  const mock = transport({ data: dump().slice(0, -20) });
  await assert.rejects(mock.manager.downloadCrash(), /incomplete/);
});

test("cancellation rejects pending reads, ignores late replies and releases SMP", async () => {
  const controller = new AbortController();
  const mock = transport({ onRead: () => controller.abort() });
  await assert.rejects(mock.manager.downloadCrash({ signal: controller.signal }), /cancelled/);
  assert.equal(mock.manager._request, null);
  assert.deepEqual(mock.commands.at(-1), { t: "smp", status: false });
  assert.equal(mock.requests.filter((request) => request.id === 0).length, 1);
});

test("timeout and disconnect release requests rather than reporting no crashes", async () => {
  const mock = transport(); mock.manager._characteristic.writeValueWithoutResponse = async () => {};
  await assert.rejects(mock.manager.request(0, 8, 1, { name: "/user/coredump.txt" }, 10), /did not acknowledge/);
  const pending = mock.manager.request(0, 8, 0, { name: "/user/coredump.txt", off: 0 });
  await mock.manager._disconnected();
  await assert.rejects(pending, /disconnected/); assert.equal(mock.manager._request, null);
});

test("erase checks the exact downloaded dump and verifies absence", async () => {
  const mock = transport(); const downloaded = await mock.manager.downloadCrash();
  await mock.manager.eraseCrash(downloaded);
  assert.equal(mock.erased, true);
  assert.deepEqual(mock.requests.find((request) => request.group === 9).body.argv, ["coredump", "erase", "0"]);
  assert.equal(mock.requests.at(-1).id, 1);
  const changed = transport({ data: dump({ line: 20 }) });
  await assert.rejects(changed.manager.eraseCrash(downloaded), /changed/);
  assert.equal(changed.erased, false);
});

test("unsupported erase and a dump still present cannot be reported as erased", async () => {
  for (const [options, message] of [[{ eraseRet: -8 }, /could not erase/], [{ keepDump: true }, /could not be confirmed/]]) {
    const mock = transport(options);
    const downloaded = await mock.manager.downloadCrash();
    await assert.rejects(mock.manager.eraseCrash(downloaded), message);
    assert.equal(mock.erased, false);
    assert.deepEqual(mock.commands.at(-1), { t: "smp", status: false });
  }
});

test("quiet firmware remains unconfirmed and a later valid log can start the stream", async (context) => {
  const originalTimer = global.setTimeout;
  const callbacks = [];
  context.mock.method(global, "setTimeout", (callback, delay) => {
    const timer = originalTimer(callback, delay);
    callbacks.push(() => { clearTimeout(timer); callback(); });
    return timer;
  });
  const mock = transport(); mock.manager._watch.write = {};
  await mock.manager.setLogCapture(true);
  callbacks.forEach((run) => run());
  assert.equal(mock.manager.logState, "no-data");
  mock.manager._watchOptions.onLog("valid log");
  assert.equal(mock.manager.logState, "streaming");
  await mock.manager.setLogCapture(false);
  callbacks.forEach((run) => run());
  assert.equal(mock.manager.logState, "off");
});

test("live capture is confirmed by a frame rather than the command write and stops on disconnect", async () => {
  const mock = transport(), logs = [];
  mock.manager._watch.write = {};
  mock.manager.onFirmwareLog((text) => logs.push(text));
  await mock.manager.setLogCapture(true);
  assert.equal(mock.manager.logState, "waiting");
  mock.manager._watchOptions.onLog("[123] <inf> log");
  assert.equal(mock.manager.logState, "streaming"); assert.equal(logs.length, 1);
  await mock.manager._disconnected(); assert.equal(mock.manager.logState, "off");
  mock.manager._watchOptions.onLog("late"); assert.equal(logs.length, 1);
});

test("capture waits past the firmware connection grace period and respects stop during startup", async () => {
  const mock = transport(); mock.manager._watch.write = {};
  mock.manager._bleReadyAt = Date.now();
  let resume, waited;
  mock.manager.sleep = (delay) => { waited = delay; return new Promise((resolve) => { resume = resolve; }); };
  const start = mock.manager.setLogCapture(true);
  assert.ok(waited > 2900 && waited <= 3000);
  assert.equal(mock.commands.filter((command) => command.t === "log").length, 0);
  await mock.manager.setLogCapture(false);
  resume(); await start;
  assert.deepEqual(mock.commands.filter((command) => command.t === "log"), [{ t: "log", status: false }]);
  assert.equal(mock.manager.logState, "off");
});

test("a fresh log frame recovers a truncated frame without treating its JSON as an acknowledgement", () => {
  const messages = [], logs = [];
  const reader = new WatchStreamReader((message) => messages.push(message), (text) => logs.push(text));
  reader.push(new TextEncoder().encode('<BLELOG>damaged {"t":"watchface_bg","ok":true}'));
  reader.push(new TextEncoder().encode('<BLELOG>[123] <inf> recovered\n</BLELOG>{"t":"watchface_bg","action":"apply_result","ok":true}'));
  assert.deepEqual(logs, ['[123] <inf> recovered\n']);
  assert.deepEqual(messages, [{ t: "watchface_bg", action: "apply_result", ok: true }]);
});

test("renewing cached NUS notifications clears a stale log frame and receives the real background acknowledgement", async () => {
  const watch = new WatchBluetooth(() => {}, async () => {});
  let cached = true, subscribed = false;
  const order = [];
  watch.notify = {
    async stopNotifications() { order.push("stop"); cached = false; subscribed = false; },
    async startNotifications() { order.push("start"); if (!cached) subscribed = true; cached = true; },
  };
  watch.reader.push(new TextEncoder().encode("<BLELOG>unfinished log"));
  await watch.subscribeCommands();
  watch.write = { properties: { write: true, writeWithoutResponse: true },
    async writeValueWithoutResponse() { assert.fail("Commands should use acknowledged writes when available"); },
    async writeValueWithResponse(chunk) {
      if (new TextDecoder().decode(chunk).endsWith(")") && subscribed) watch.onNotification({ target: { value: new DataView(new TextEncoder().encode('{"t":"watchface_bg","action":"apply_result","ok":true}').buffer) } });
    },
  };
  const result = await watch.request({ t: "watchface_bg", action: "apply" }, (message) => message.t === "watchface_bg" && message.action === "apply_result", 50);
  assert.equal(result.ok, true);
  assert.deepEqual(order, ["stop", "start"]);
});

test("a service-change ATT rejection retries that packet once, while other write failures remain failures", async () => {
  const watch = new WatchBluetooth(() => {}, async () => {});
  let attempts = 0;
  watch.write = { properties: { write: true }, async writeValueWithResponse() {
    if (++attempts === 1) throw new Error("ATT error: 0x12 (Database Out Of Sync)");
  } };
  await watch.send({ t: "ver" });
  assert.equal(attempts, 2);
  watch.write.writeValueWithResponse = async () => { attempts++; throw new Error("Connection lost"); };
  await assert.rejects(watch.send({ t: "ver" }), /Connection lost/);
  assert.equal(attempts, 3);
});
