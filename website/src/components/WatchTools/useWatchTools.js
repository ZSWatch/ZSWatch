import { useEffect, useRef, useState } from "react";
import { useHistory } from "@docusaurus/router";
import { useWatchSession } from "../WatchSession/context";
import { SCREENS } from "../FirmwareUpdate/constants";

export function useWatchToolsConnection(session) {
  const history = useHistory();
  const manager = useRef(session.mcumgr);
  const operation = useRef(null);
  const alive = useRef(false);
  const [available, setAvailable] = useState(false);
  const sessionConnected = session.screen === SCREENS.CONNECTED;
  const connected = sessionConnected && session.transport === "ble";
  const deviceInfo = session.deviceInfo;
  const [phase, setPhase] = useState("idle");
  const [status, setStatus] = useState("");
  const [error, setError] = useState("");
  const [progress, setProgress] = useState(null);
  const busy = ["connecting", "uploading", "applying", "restoring", "reading", "erasing", "log-control"].includes(phase);

  useEffect(() => {
    alive.current = true;
    setAvailable(Boolean(navigator.bluetooth));
    return () => {
      alive.current = false;
      operation.current?.abort();
    };
  }, []);

  useEffect(() => {
    if (!busy || phase === "connecting") return;
    const unblock = history.block(() => false);
    const warn = (event) => { event.preventDefault(); event.returnValue = ""; };
    window.addEventListener("beforeunload", warn);
    let lock, released = false;
    navigator.wakeLock?.request("screen").then((value) => { lock = value; if (released) value.release(); }).catch(() => {});
    return () => { unblock(); released = true; window.removeEventListener("beforeunload", warn); lock?.release(); };
  }, [busy, phase, history]);

  async function connect(deviceName) {
    if (operation.current || !manager.current) return;
    const connection = manager.current;
    const controller = new AbortController();
    operation.current = controller;
    setError(""); setStatus("Connecting to watch..."); setPhase("connecting");
    try {
      const ok = await session.handleConnect({ transport: "ble", deviceName: deviceName.trim() });
      if (!ok) { setStatus(""); setPhase("idle"); return; }
      if (alive.current && manager.current === connection) { setPhase("idle"); setStatus("Connected. Watch tools are ready."); }
    } catch (failure) {
      if (alive.current && manager.current === connection) { setError(failure.name === "NotFoundError" ? "No watch selected. Connect again when ready." : failure.message); setStatus(""); setPhase("idle"); }
    } finally { if (operation.current === controller) operation.current = null; }
  }

  async function disconnect() {
    if (operation.current) return;
    await session.handleDisconnect();
    if (alive.current) { setStatus("Disconnected."); }
  }

  async function run(action, getBytes) {
    const connection = manager.current;
    if (operation.current || session.preparingFirmware || !connected || !connection?.isConnected()) return false;
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
      if (alive.current) { setError(failure.message); setStatus(""); setPhase("idle"); }
      return false;
    } finally { if (operation.current === controller) operation.current = null; }
  }

  function clearMessages() { if (!operation.current) { setError(""); setStatus(""); setProgress(null); setPhase("idle"); } }

  async function perform(kind, message, action) {
    const connection = manager.current;
    if (operation.current || session.preparingFirmware || !connected || !connection?.isConnected()) return { ok: false };
    const controller = new AbortController();
    operation.current = controller;
    setError(""); setProgress(null); setPhase(kind); setStatus(message);
    try {
      const value = await action(connection, { signal: controller.signal, onProgress: (value) => { if (alive.current) setProgress(value); } });
      if (alive.current) { setPhase("success"); setStatus(""); }
      return { ok: true, value };
    } catch (failure) {
      if (alive.current) { setError(failure.message); setStatus(""); setPhase("idle"); }
      return { ok: false };
    } finally { if (operation.current === controller) operation.current = null; }
  }

  return { available, connected, sessionConnected, transport: session.transport, deviceName: session.deviceName, watchName: session.mcumgr.name || session.deviceName, handleDeviceNameChange: session.handleDeviceNameChange, deviceInfo, phase, busy: busy || session.preparingFirmware || session.screen === SCREENS.CONNECTING, status: phase === "connecting" ? session.connectionStatus || status : status, error: error || session.connectionError, progress, connect, disconnect, clearMessages,
    manager, perform,
    apply: (getBytes) => run("apply", getBytes), restore: () => run("reset"), cancel: () => operation.current?.abort() };
}

export default function useWatchTools() { return useWatchSession().tools; }
