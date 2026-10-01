export const MAX_CRASH_BYTES = 1024 * 1024;
export const HISTORY_KEY = "zswatch.crashes.v1";
export const LOG_LIMIT = 2000;
export const TRAFFIC_LIMIT = 1000;

export function logLines(text, receivedAt = new Date().toISOString()) {
  return text.replace(/\x1b\[[0-9;]*m/g, "").split(/\r?\n/).filter((line) => line.trim()).map((text) => {
    const level = text.match(/<(dbg|inf|wrn|err)>/i)?.[1]?.toLowerCase();
    return { text, receivedAt, level: ({ dbg: "debug", inf: "info", wrn: "warning", err: "error" })[level] || "unknown" };
  });
}

export function parseCrash(input) {
  const bytes = new Uint8Array(input);
  if (!bytes.length || bytes.length > MAX_CRASH_BYTES) throw new Error("Choose a crash dump between 1 byte and 1 MB.");
  const decoder = new TextDecoder("utf-8", { fatal: true });
  const marker = new TextEncoder().encode("#CD:BEGIN#");
  const at = (offset) => marker.every((value, i) => bytes[offset + i] === value);
  const offset = at(0) ? 0 : at(72) ? 72 : -1;
  if (offset < 0) throw new Error("This file is not a ZSWatch crash dump (missing BEGIN marker).");
  let text;
  try { text = decoder.decode(bytes.slice(offset)).replace(/\r\n/g, "\n").trim(); }
  catch { throw new Error("The crash dump contains invalid text."); }
  const lines = text.split("\n");
  if (lines[0] !== "#CD:BEGIN#" || lines.at(-1) !== "#CD:END#") throw new Error("The crash dump is incomplete (missing END marker).");
  const records = lines.slice(1, -1).filter((line) => line.startsWith("#CD:"));
  if (!records.length || records.some((line) => !/^#CD:(?:[a-f0-9]{2})+$/i.test(line))) throw new Error("The crash dump contains invalid data records.");
  const field = (name) => lines.find((line) => line.startsWith(name + ":"))?.slice(name.length + 1).trim() || "";
  const headerString = (start) => new TextDecoder().decode(bytes.slice(start, start + 33)).split("\0")[0].trim();
  const storedLine = offset ? new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength).getInt32(68, true) : 0;
  const line = storedLine || Number(field("LINE"));
  return { text, size: bytes.length, timestamp: (offset ? headerString(0) : "") || field("ASSERT"),
    file: (offset ? headerString(33) : "") || field("FILE"), line: Number.isInteger(line) && line > 0 ? line : null,
    format: offset ? "binary summary + text" : "text", dataBytes: records.reduce((sum, line) => sum + (line.length - 4) / 2, 0) };
}

export function bytesToBase64(bytes) {
  let value = "";
  for (let offset = 0; offset < bytes.length; offset += 8192) value += String.fromCharCode(...bytes.slice(offset, offset + 8192));
  return btoa(value);
}

export function reportBytes(report) {
  return Uint8Array.from(atob(report.raw), (char) => char.charCodeAt(0));
}

export async function makeReport(bytes, info = {}, source = "Imported file") {
  const parsed = parseCrash(bytes);
  // Hash the normalized dump so importing its text export does not duplicate it.
  const digest = await crypto.subtle.digest("SHA-256", new TextEncoder().encode(parsed.text));
  const id = Array.from(new Uint8Array(digest), (byte) => byte.toString(16).padStart(2, "0")).join("");
  return { ...parsed, id, raw: bytesToBase64(bytes), receivedAt: new Date().toISOString(), source, info: { ...info } };
}

export function addReport(history, report) {
  if (history.some((item) => item.id === report.id)) return history;
  const next = [report, ...history].slice(0, 20);
  while (JSON.stringify(next).length > 3 * 1024 * 1024 && next.length > 1) next.pop();
  return next;
}

export function loadHistory(storage) {
  const data = JSON.parse(storage.getItem(HISTORY_KEY) || "[]");
  if (!Array.isArray(data) || data.length > 20) throw new Error("Saved crash history is invalid.");
  return data.map((item) => {
    if (typeof item.id !== "string" || typeof item.raw !== "string" || item.raw.length > MAX_CRASH_BYTES * 1.4) throw new Error("Saved crash history is invalid.");
    return { ...item, ...parseCrash(reportBytes(item)) };
  });
}

export function saveHistory(storage, history) { storage.setItem(HISTORY_KEY, JSON.stringify(history)); }

export function crashFrequencies(history) {
  const counts = new Map();
  for (const report of history) {
    const label = report.file ? `${report.file}${report.line ? ":" + report.line : ""}` : "Location not reported";
    counts.set(label, (counts.get(label) || 0) + 1);
  }
  return [...counts].sort((a, b) => b[1] - a[1]);
}

export function downloadFile(name, data, type = "text/plain") {
  const url = URL.createObjectURL(new Blob([data], { type }));
  const anchor = document.createElement("a");
  anchor.href = url; anchor.download = name; anchor.click();
  setTimeout(() => URL.revokeObjectURL(url), 1000);
}

export function trafficText(entry, hex = false) {
  const content = hex ? Array.from(entry.bytes, (byte) => byte.toString(16).padStart(2, "0")).join(" ") : new TextDecoder().decode(entry.bytes).replace(/\0/g, "\\0");
  return `${entry.receivedAt} ${entry.channel} ${entry.direction} ${content}`;
}
