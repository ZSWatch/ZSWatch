import { useEffect, useRef, useState } from "react";
import { addReport, loadHistory, saveHistory, LogLineReader, makeReport, LOG_LIMIT, TRAFFIC_LIMIT } from "./diagnostics";

export default function useDiagnostics(tools) {
  const [logs, setLogs] = useState([]);
  const [traffic, setTraffic] = useState([]);
  const [logState, setLogState] = useState("off");
  const [history, setHistory] = useState([]);
  const [selected, setSelected] = useState(null);
  const [crashStatus, setCrashStatus] = useState("");
  const [localError, setLocalError] = useState("");
  const currentCrash = useRef(null);
  const logReader = useRef(new LogLineReader());
  const historyRef = useRef([]);
  const buffers = useRef({ logs: [], traffic: [], logDrops: 0, trafficDrops: 0, dirty: false });
  const [dropped, setDropped] = useState({ logs: 0, traffic: 0 });

  useEffect(() => {
    const connection = tools.manager.current;
    if (!connection) return;
    const append = (key, entries, limit) => {
      const buffer = buffers.current;
      buffer[key].push(...entries);
      const excess = Math.max(0, buffer[key].length - limit);
      if (excess) { buffer[key].splice(0, excess); buffer[key === "logs" ? "logDrops" : "trafficDrops"] += excess; }
      buffer.dirty = true;
    };
    connection.onFirmwareLog((text) => append("logs", logReader.current.push(text), LOG_LIMIT));
    connection.onTraffic((entry) => append("traffic", [{ ...entry, receivedAt: new Date().toISOString() }], TRAFFIC_LIMIT));
    connection.onLogState((state) => {
      if (state === "off") append("logs", logReader.current.flush(), LOG_LIMIT);
      setLogState(state);
    });
    const timer = setInterval(() => {
      const buffer = buffers.current;
      if (!buffer.dirty) return;
      buffer.dirty = false;
      setLogs([...buffer.logs]); setTraffic([...buffer.traffic]);
      setDropped({ logs: buffer.logDrops, traffic: buffer.trafficDrops });
    }, 200);
    try { const saved = loadHistory(window.localStorage); historyRef.current = saved; setHistory(saved); }
    catch { setLocalError("Could not read browser history. Downloads and live capture are still available."); }
    return () => { clearInterval(timer); connection.onFirmwareLog(null).onTraffic(null).onLogState(null); };
  }, []);

  useEffect(() => { if (!tools.connected) currentCrash.current = null; }, [tools.connected]);

  function persist(next) {
    historyRef.current = next; setHistory(next);
    try { saveHistory(window.localStorage, next); }
    catch { setLocalError("Browser history could not be saved. Download the dump to keep a copy."); }
  }

  function remember(report) {
    const next = addReport(historyRef.current, report);
    persist(next); setSelected(report);
  }

  async function checkCrash() {
    setLocalError(""); setCrashStatus(""); currentCrash.current = null;
    const result = await tools.perform("reading", "Checking the watch for a crash dump…", (manager, options) => manager.downloadCrash(options));
    if (!result.ok) return;
    if (!result.value) { setCrashStatus("No crash dump is stored on this watch."); return; }
    try {
      const report = await makeReport(result.value.bytes, tools.manager.current.deviceInfo, result.value.path);
      currentCrash.current = { ...result.value, id: report.id, generation: tools.manager.current.connectionGeneration };
      remember(report); setCrashStatus("Crash dump downloaded. Your watch keeps its copy until you erase it.");
    } catch (error) { setLocalError(error.message); }
  }

  async function eraseCrash() {
    const expected = currentCrash.current;
    if (!expected || expected.id !== selected?.id || expected.generation !== tools.manager.current.connectionGeneration) return;
    if (!window.confirm("Erase the downloaded crash dump from your watch? The copy in this browser will be kept.")) return;
    const result = await tools.perform("erasing", "Erasing the downloaded crash dump…", (manager, options) => manager.eraseCrash(expected, options));
    if (result.ok) { currentCrash.current = null; setCrashStatus("Crash dump erased from the watch. Browser history is unchanged."); }
  }

  function clearBuffer(key) {
    const buffer = buffers.current;
    buffer[key] = []; buffer[key === "logs" ? "logDrops" : "trafficDrops"] = 0; buffer.dirty = true;
    if (key === "logs") { logReader.current.reset(); setLogs([]); } else setTraffic([]);
  }

  return { logs, traffic, dropped, logState, history, selected, setSelected, crashStatus, localError, setLocalError,
    checkCrash, eraseCrash, clearBuffer,
    canErase: tools.connected && selected?.id === currentCrash.current?.id && currentCrash.current?.generation === tools.manager.current?.connectionGeneration,
    removeReport: (id) => { persist(historyRef.current.filter((report) => report.id !== id)); if (selected?.id === id) setSelected(null); },
    toggleLogs: () => tools.perform("log-control", "Changing live log capture…", (manager) => manager.setLogCapture(!["waiting", "streaming", "no-data"].includes(logState))),
  };
}
