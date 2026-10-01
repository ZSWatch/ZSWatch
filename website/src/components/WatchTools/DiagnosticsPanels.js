import React, { useEffect, useMemo, useRef, useState } from "react";
import { crashFrequencies, downloadFile, reportBytes, trafficText } from "./diagnostics";
import styles from "./styles.module.css";

const logLabels = { off: "Capture stopped", waiting: "Waiting for the watch’s first log…", streaming: "Receiving firmware logs", "no-data": "No framed logs received yet. The watch may be quiet or this firmware may not support live logging." };

export function LogsPanel({ tools, diagnostics }) {
  const [view, setView] = useState("logs");
  const [query, setQuery] = useState("");
  const [level, setLevel] = useState("all");
  const [direction, setDirection] = useState("all");
  const [channel, setChannel] = useState("all");
  const [hex, setHex] = useState(false);
  const [paused, setPaused] = useState(false);
  const [autoScroll, setAutoScroll] = useState(true);
  const [snapshot, setSnapshot] = useState({ logs: [], traffic: [] });
  const viewport = useRef(null);
  const active = diagnostics.logState !== "off";
  const entries = paused ? snapshot[view] : diagnostics[view];
  const visible = useMemo(() => entries.filter((entry) => {
    if (view === "logs" && level !== "all" && entry.level !== level) return false;
    if (view === "traffic" && (direction !== "all" && entry.direction !== direction || channel !== "all" && entry.channel !== channel)) return false;
    return (view === "logs" ? entry.text : trafficText(entry, hex)).toLowerCase().includes(query.toLowerCase());
  }), [entries, view, level, direction, channel, query, hex]);
  const text = visible.map((entry) => view === "logs" ? `${entry.receivedAt} ${entry.text}` : trafficText(entry, hex)).join("\n");
  useEffect(() => { if (autoScroll && !paused && viewport.current) viewport.current.scrollTop = viewport.current.scrollHeight; }, [visible, autoScroll, paused]);

  function togglePause() {
    if (!paused) setSnapshot({ logs: diagnostics.logs, traffic: diagnostics.traffic });
    setPaused(!paused);
  }
  async function copy() {
    try { await navigator.clipboard.writeText(text); }
    catch { diagnostics.setLocalError("Could not copy to the clipboard. Use Download text instead."); }
  }

  return <section className={styles.diagnosticPanel} aria-labelledby="logs-title">
    <div className={styles.libraryHeading}><div><p className={styles.eyebrow}>DIAGNOSTICS</p><h2 id="logs-title">Logs</h2></div><button className={styles.secondaryButton} disabled={!tools.connected || tools.busy} onClick={diagnostics.toggleLogs}>{active ? "Stop capture" : "Start capture"}</button></div>
    <p className={styles.help} role="status">{tools.connected ? logLabels[diagnostics.logState] : "Connect your watch to capture live logs."}</p>
    <p className={styles.help}>Capture continues while you view other tools. File transfers stop firmware log capture; start it again afterward.</p>
    <div className={styles.toolbar} role="group" aria-label="Log source">
      {[ ["logs", "Firmware logs"], ["traffic", "Bluetooth traffic"] ].map(([key, label]) => <button key={key} className={view === key ? styles.activeFilter : styles.filter} aria-pressed={view === key} onClick={() => setView(key)}>{label}</button>)}
    </div>
    <div className={styles.toolbar}>
      <label className={styles.field}>Search<input value={query} onChange={(event) => setQuery(event.target.value)} placeholder="Filter messages" /></label>
      {view === "logs" ? <label className={styles.field}>Level<select value={level} onChange={(event) => setLevel(event.target.value)}>{["all", "debug", "info", "warning", "error", "unknown"].map((value) => <option key={value} value={value}>{value === "all" ? "All levels" : value}</option>)}</select></label> : <>
        <label className={styles.field}>Direction<select value={direction} onChange={(event) => setDirection(event.target.value)}>{["all", "RX", "TX"].map((value) => <option key={value} value={value}>{value === "all" ? "RX + TX" : value}</option>)}</select></label>
        <label className={styles.field}>Channel<select value={channel} onChange={(event) => setChannel(event.target.value)}>{["all", "NUS", "SMP"].map((value) => <option key={value} value={value}>{value === "all" ? "All channels" : value}</option>)}</select></label>
        <label className={styles.check}><input type="checkbox" checked={hex} onChange={(event) => setHex(event.target.checked)} />Hex</label>
      </>}
    </div>
    <div className={styles.toolbar}>
      <button className={styles.secondaryButton} onClick={togglePause}>{paused ? "Resume view" : "Pause view"}</button>
      <label className={styles.check}><input type="checkbox" checked={autoScroll} onChange={(event) => setAutoScroll(event.target.checked)} />Auto-scroll</label>
      <button className={styles.textButton} disabled={!entries.length} onClick={() => { diagnostics.clearBuffer(view); setSnapshot((old) => ({ ...old, [view]: [] })); }}>Clear view</button>
      <button className={styles.textButton} disabled={!visible.length} onClick={copy}>Copy filtered</button>
      <button className={styles.textButton} disabled={!visible.length} onClick={() => downloadFile(`zswatch-${view}.txt`, text)}>Download text</button>
    </div>
    <p className={styles.help}>{visible.length} matching · {entries.length} buffered{diagnostics.dropped[view] ? ` · ${diagnostics.dropped[view]} older entries discarded` : ""}{paused ? " · View paused; capture continues" : ""}. Showing the latest 500 matches.</p>
    <div ref={viewport} className={styles.logViewport} tabIndex="0" aria-label={view === "logs" ? "Firmware log output" : "Bluetooth traffic output"}>
      {visible.length ? visible.slice(-500).map((entry, i) => <pre key={i} className={entry.level === "error" ? styles.errorLine : entry.level === "warning" ? styles.warningLine : ""}>{view === "logs" ? entry.text : trafficText(entry, hex)}</pre>) : <p className={styles.emptyState}>{query || level !== "all" ? "No messages match your filters." : "Captured messages will appear here."}</p>}
    </div>
    <p className={styles.help}>Filters apply to messages received by this page. Log levels compiled out of the firmware cannot be captured. Bluetooth traffic shows received notifications and successfully written packets.</p>
    {diagnostics.localError && <p className={styles.error} role="alert">{diagnostics.localError}</p>}
  </section>;
}

export function CrashesPanel({ tools, diagnostics }) {
  const fileInput = useRef(null);
  const report = diagnostics.selected;
  const frequencies = crashFrequencies(diagnostics.history);
  return <section className={styles.diagnosticPanel} aria-labelledby="crashes-title">
    <div className={styles.libraryHeading}><div><p className={styles.eyebrow}>DIAGNOSTICS</p><h2 id="crashes-title">Crash reports</h2></div><button className={styles.secondaryButton} disabled={!tools.connected || tools.busy} onClick={diagnostics.checkCrash}>{tools.phase === "reading" ? "Downloading…" : "Check watch"}</button></div>
    <p className={styles.help}>Download the crash stored on your watch, or open a saved dump. Reports stay on this computer. Current firmware keeps one crash dump on the watch.</p>
    <div className={styles.toolbar}>
      <button className={styles.secondaryButton} disabled={tools.busy} onClick={() => fileInput.current.click()}>Import dump</button>
      <input ref={fileInput} hidden type="file" accept=".bin,.txt" onChange={(event) => { diagnostics.importCrash(event.target.files[0]); event.target.value = ""; }} />
      <span className={styles.help}>ZSWatch binary or text dump · up to 1 MB</span>
    </div>
    {diagnostics.crashStatus && <p className={styles.notice} role="status">{diagnostics.crashStatus}</p>}
    {diagnostics.localError && <p className={styles.error} role="alert">{diagnostics.localError}</p>}
    {report ? <div className={styles.reportCard}>
      <h3>Selected report</h3>
      <dl className={styles.details}>
        <div><dt>Watch timestamp</dt><dd>{report.timestamp || "Not reported"}</dd></div>
        <div><dt>Crash location</dt><dd>{report.file ? `${report.file}${report.line ? ":" + report.line : ""}` : "Not reported"}</dd></div>
        <div><dt>Received</dt><dd>{new Date(report.receivedAt).toLocaleString()}</dd></div>
        <div><dt>Source</dt><dd>{report.source}</dd></div>
        <div><dt>Size / format</dt><dd>{report.size.toLocaleString()} bytes · {report.format}</dd></div>
        {report.info?.hardwareVersion && <div><dt>Hardware</dt><dd>{report.info.hardwareVersion}</dd></div>}
        {report.info?.firmwareVersion && <div><dt>Firmware when downloaded</dt><dd>{report.info.firmwareVersion}</dd></div>}
        {report.info?.firmwareSha && <div><dt>Firmware commit when downloaded</dt><dd>{report.info.firmwareSha}</dd></div>}
      </dl>
      <div className={styles.toolbar}>
        <button className={styles.secondaryButton} onClick={() => downloadFile("zswatch-coredump.bin", reportBytes(report), "application/octet-stream")}>Download original</button>
        <button className={styles.secondaryButton} onClick={() => downloadFile("zswatch-coredump.txt", report.text + "\n")}>Download text</button>
        {diagnostics.canErase && <button className={styles.dangerButton} disabled={tools.busy} onClick={diagnostics.eraseCrash}>Erase from watch…</button>}
      </div>
      <details><summary>Dump contents</summary><pre className={styles.dumpText}>{report.text}</pre></details>
    </div> : <p className={styles.emptyState}>Check your watch or import a dump to view a report.</p>}
    <div className={styles.reportCard}>
      <h3>Browser history <span className={styles.badge}>{diagnostics.history.length} unique reports</span></h3>
      <p className={styles.help}>Up to 20 reports are kept in this browser, within a storage limit. Repeated downloads of the same dump count once. Download reports you want to keep permanently.</p>
      {diagnostics.history.length ? <ul className={styles.historyList}>{diagnostics.history.map((item) => <li key={item.id}>
        <button className={item.id === report?.id ? styles.selectedReport : styles.reportButton} aria-pressed={item.id === report?.id} onClick={() => diagnostics.setSelected(item)}><strong>{item.file || "Location not reported"}{item.line ? ":" + item.line : ""}</strong><span>{item.timestamp || "Timestamp not reported"} · {new Date(item.receivedAt).toLocaleString()}</span></button>
        <button className={styles.textButton} disabled={tools.busy} aria-label={`Remove report received ${item.receivedAt} from browser history`} onClick={() => diagnostics.removeReport(item.id)}>Remove local copy</button>
      </li>)}</ul> : <p className={styles.help}>No reports saved yet.</p>}
      {frequencies.length > 0 && <><h4 className={styles.frequencyTitle}>Locations in saved reports</h4><ul className={styles.frequencyList}>{frequencies.map(([location, count]) => <li key={location}><span>{location}</span><strong>{count}</strong></li>)}</ul></>}
    </div>
  </section>;
}
