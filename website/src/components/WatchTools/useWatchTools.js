import { useEffect, useRef, useState } from "react";
import WatchToolsManager from "./WatchToolsManager";

export default function useWatchTools() {
  const manager = useRef(null);
  const operation = useRef(null);
  const alive = useRef(false);
  const [available, setAvailable] = useState(false);
  const [connected, setConnected] = useState(false);
  const [deviceInfo, setDeviceInfo] = useState({});
  const [phase, setPhase] = useState("idle");
  const [status, setStatus] = useState("");
  const [error, setError] = useState("");
  const [progress, setProgress] = useState(null);
  const busy = ["connecting", "uploading", "applying", "restoring"].includes(phase);

  useEffect(() => {
    alive.current = true;
    setAvailable(Boolean(navigator.bluetooth));
    const connection = new WatchToolsManager({ logger: { info() {}, error: console.error } });
    manager.current = connection;
    connection.onDeviceInfo((info) => { if (alive.current) setDeviceInfo(info); });
    connection.onDisconnect(() => { if (alive.current) setConnected(false); });
    return () => {
      alive.current = false;
      operation.current?.abort();
      connection.disconnect().catch(() => {});
    };
  }, []);

  useEffect(() => {
    if (!busy || phase === "connecting") return;
    const warn = (event) => { event.preventDefault(); event.returnValue = ""; };
    window.addEventListener("beforeunload", warn);
    let lock, released = false;
    navigator.wakeLock?.request("screen").then((value) => { lock = value; if (released) value.release(); }).catch(() => {});
    return () => { released = true; window.removeEventListener("beforeunload", warn); lock?.release(); };
  }, [busy, phase]);

  async function connect(deviceName) {
    if (operation.current || !manager.current) return;
    const connection = manager.current;
    const controller = new AbortController();
    operation.current = controller;
    setError(""); setStatus("Choose your watch in the Bluetooth device list."); setPhase("connecting");
    try {
      await connection.connect({ transport: "ble", filters: deviceName.trim() ? [{ namePrefix: deviceName.trim() }] : null });
      if (alive.current && manager.current === connection) { setConnected(true); setPhase("idle"); setStatus("Connected. Choose a background to get started."); }
      else await connection.disconnect();
    } catch (failure) {
      if (alive.current && manager.current === connection) { setError(failure.name === "NotFoundError" ? "No watch selected. Connect again when ready." : failure.message); setStatus(""); setPhase("idle"); }
    } finally { if (operation.current === controller) operation.current = null; }
  }

  async function disconnect() {
    if (operation.current) return;
    await manager.current?.disconnect();
    if (alive.current) { setDeviceInfo({}); setStatus("Disconnected."); }
  }

  async function run(action, getBytes) {
    const connection = manager.current;
    if (operation.current || !connection?.isConnected()) return false;
    const controller = new AbortController();
    operation.current = controller;
    setError(""); setProgress(null);
    setPhase(action === "apply" ? "uploading" : "restoring");
    setStatus(action === "apply" ? "Preparing your background..." : "Restoring the default background...");
    try {
      if (action === "apply") {
        const bytes = await getBytes(controller.signal);
        if (controller.signal.aborted) throw new Error("Background upload cancelled.");
        await connection.applyBackground(bytes, {
          signal: controller.signal,
          onProgress: (value) => { if (alive.current) { setProgress(value); setStatus("Uploading your background..."); } },
          onApplying: () => { if (alive.current) { setPhase("applying"); setStatus("Applying the background on your watch..."); } },
        });
      } else await connection.restoreDefault();
      if (alive.current) { setPhase("success"); setStatus(action === "apply" ? "Background applied. Your watch is ready." : "Default background restored."); }
      return true;
    } catch (failure) {
      if (alive.current) { setError(failure.message); setStatus(""); setPhase("idle"); setConnected(connection.isConnected()); }
      return false;
    } finally { if (operation.current === controller) operation.current = null; }
  }

  function clearMessages() { if (!operation.current) { setError(""); setStatus(""); setProgress(null); setPhase("idle"); } }

  return { available, connected, deviceInfo, phase, busy, status, error, progress, connect, disconnect, clearMessages,
    apply: (getBytes) => run("apply", getBytes), restore: () => run("reset"), cancel: () => operation.current?.abort() };
}
