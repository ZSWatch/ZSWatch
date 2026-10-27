import React, { useEffect, useMemo, useRef, useState } from "react";
import { crashFrequencies, downloadFile, reportBytes, trafficText } from "./diagnostics";
import styles from "./styles.module.css";

const logLabels = { off: "Watch logging stopped", waiting: "Waiting for the watch’s first log…", streaming: "Receiving firmware logs", "no-data": "No framed logs received yet. The watch may be quiet or this firmware may not support live logging." };

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
    <div className={styles.panelHeading}>
      <div><h2 id="logs-title">Logs</h2><p>Live firmware output and Bluetooth traffic.</p></div>
      <button className={`${active ? styles.secondaryButton : styles.primaryButton} ${styles.captureButton}`} disabled={!tools.connected || tools.busy} onClick={diagnostics.toggleLogs}>{active ? "Stop logs" : "Start logs"}</button>
    </div>
    <div className={styles.logConsole}>
      <div className={styles.consoleTools}>
        <div className={styles.consoleTopbar}>
          <div className={styles.sourceSwitch} role="group" aria-label="Log source">
            {[["logs", "Firmware logs"], ["traffic", "Bluetooth traffic"]].map(([key, label]) => <button key={key} className={view === key ? styles.activeFilter : styles.filter} aria-pressed={view === key} onClick={() => setView(key)}>{label}</button>)}
          </div>
          <span role="status" title={tools.connected ? logLabels[diagnostics.logState] : "Connect your watch to capture live logs."} className={`${styles.captureBadge} ${tools.connected && diagnostics.logState === "streaming" ? styles.captureLive : ""}`}>{!tools.connected ? "Disconnected" : ({ off: "Stopped", waiting: "Starting", streaming: "Live", "no-data": "Waiting for logs" })[diagnostics.logState]}</span>
        </div>
        <div className={styles.consoleFilters}>
          <label className={styles.field}>Search<input value={query} onChange={(event) => setQuery(event.target.value)} placeholder="Filter messages…" /></label>
          {view === "logs" ? <label className={styles.field}>Level<select value={level} onChange={(event) => setLevel(event.target.value)}>{["all", "debug", "info", "warning", "error", "unknown"].map((value) => <option key={value} value={value}>{value === "all" ? "All levels" : value}</option>)}</select></label> : <>
            <label className={styles.field}>Direction<select value={direction} onChange={(event) => setDirection(event.target.value)}>{["all", "RX", "TX"].map((value) => <option key={value} value={value}>{value === "all" ? "RX + TX" : value}</option>)}</select></label>
            <label className={styles.field}>Channel<select value={channel} onChange={(event) => setChannel(event.target.value)}>{["all", "NUS", "SMP"].map((value) => <option key={value} value={value}>{value === "all" ? "All channels" : value}</option>)}</select></label>
            <label className={styles.check}><input type="checkbox" checked={hex} onChange={(event) => setHex(event.target.checked)} />Hex</label>
          </>}
        </div>
        <div className={styles.consoleActions}>
          <div><button className={styles.secondaryButton} onClick={togglePause}>{paused ? "Resume view" : "Pause view"}</button><label className={styles.check}><input type="checkbox" checked={autoScroll} onChange={(event) => setAutoScroll(event.target.checked)} />Auto-scroll</label></div>
          <div>
            <button className={styles.textButton} disabled={!entries.length} onClick={() => { diagnostics.clearBuffer(view); setSnapshot((old) => ({ ...old, [view]: [] })); }}>Clear</button>
            <button className={styles.textButton} disabled={!visible.length} onClick={copy}>Copy</button>
            <button className={styles.textButton} disabled={!visible.length} onClick={() => downloadFile(`zswatch-${view}.txt`, text)}>Download</button>
          </div>
        </div>
      </div>
      <div ref={viewport} className={styles.logViewport} tabIndex="0" aria-label={view === "logs" ? "Firmware log output" : "Bluetooth traffic output"}>
        {visible.length ? visible.slice(-500).map((entry, i) => <pre key={i} className={entry.level === "error" ? styles.errorLine : entry.level === "warning" ? styles.warningLine : ""}>{view === "logs" ? entry.text : trafficText(entry, hex)}{entry.partial && <span className={styles.partialLine}> [partial line]</span>}</pre>) : <div className={styles.consoleEmpty}><strong>{query || level !== "all" ? "No matching messages" : "Ready for logs"}</strong><span>{query || level !== "all" ? "Change your filters to see more messages." : !tools.connected ? "Connect your watch, then start logs." : active ? "Waiting for messages from your watch…" : "Start logs to view your watch’s logs."}</span></div>}
      </div>
      <div className={styles.consoleFooter}>{visible.length} matching · {entries.length} buffered{diagnostics.dropped[view] ? ` · ${diagnostics.dropped[view]} older entries discarded` : ""}{paused ? " · View paused; capture continues" : ""}<span>Latest 500 matches shown</span></div>
    </div>
    {diagnostics.logState === "no-data" && <p role="status" className={styles.notice}>No firmware logs received. Check Bluetooth traffic for incoming messages; this firmware may be quiet or lack BLE logging. Performance mode does not generate logs.</p>}
    <details className={styles.captureInfo}><summary>About live logs</summary><p>Start logs enables the watch’s BLE log output. Stop logs disables that output and keeps the watch connected. Logging continues while you navigate between Tools and Firmware Update. File transfers stop firmware log capture; start it again afterward. Filters apply to received messages. Performance mode does not enable additional log levels. Only messages compiled into the firmware can be received. Bluetooth traffic shows received notifications and successfully written packets. Copy and Download include every filtered message in the buffer.</p></details>
    {diagnostics.localError && <p className={styles.error} role="alert">{diagnostics.localError}</p>}
  </section>;
}

export function CrashesPanel({ tools, diagnostics }) {
  const report = diagnostics.selected;
  const frequencies = crashFrequencies(diagnostics.history);
  return <section className={styles.diagnosticPanel} aria-labelledby="crashes-title">
    <div className={styles.panelHeading}><div><h2 id="crashes-title">Crash reports</h2><p>Download a crash from your watch or view a report in browser history.</p></div><button className={`${styles.secondaryButton} ${styles.captureButton}`} disabled={!tools.connected || tools.busy} onClick={diagnostics.checkCrash}>{tools.phase === "reading" ? "Downloading…" : "Check watch"}</button></div>
    <p className={styles.help}>Current firmware keeps one crash dump on the watch.</p>
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
    </div> : <p className={styles.emptyState}>Check your watch or select a report from browser history.</p>}
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
