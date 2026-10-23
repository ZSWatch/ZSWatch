import React from "react";
import { useWatchSession } from "./context";
import { SCREENS } from "../FirmwareUpdate/constants";
import styles from "./workspace.module.css";

export default function WatchConnection({ firmwareUpdate = false, disabled = false }) {
  const { firmware: session, tools } = useWatchSession();
  const connected = session.screen === SCREENS.CONNECTED;
  const connecting = session.screen === SCREENS.CONNECTING;
  const transport = !firmwareUpdate && !connected ? "ble" : session.transport;
  const busy = disabled || tools.busy || session.preparingFirmware || connecting;
  const available = transport === "ble" ? session.bluetoothAvailable : session.serialAvailable;
  const info = session.deviceInfo;

  return <section className={styles.connection} aria-label="Watch connection">
    <div className={styles.summary}>
      <strong>{connected ? session.mcumgr.name || "Your watch" : "Your watch"}</strong>
      <span className={`${styles.badge} ${connected ? styles.connected : ""}`} role="status">{connected ? `Connected (${transport === "ble" ? "Bluetooth" : "USB"})` : connecting ? "Connecting…" : "Disconnected"}</span>
    </div>
    <div className={styles.controls}>
      <div className={styles.transport} role="group" aria-label="Connection method">
        <button type="button" aria-pressed={transport === "ble"} disabled={connected || busy} onClick={() => session.handleTransportChange("ble")}>Bluetooth</button>
        <button type="button" aria-pressed={transport === "serial"} disabled={!firmwareUpdate || connected || busy} title={!firmwareUpdate ? "USB is available in Firmware Update. Watch Tools use Bluetooth." : undefined} onClick={() => session.handleTransportChange("serial")}>USB</button>
      </div>
      {!connected ? <>
        <label className={styles.name}>Name filter<input value={session.deviceName} onChange={(event) => session.handleDeviceNameChange(event.target.value)} placeholder="ZSWatch" disabled={busy || transport !== "ble"} /></label>
        <button className={styles.primaryButton} disabled={busy || !available} onClick={() => firmwareUpdate ? session.handleConnect({ transport }) : tools.connect(session.deviceName)}>{connecting ? "Connecting…" : "Connect watch"}</button>
      </> : <button className={styles.secondaryButton} disabled={busy} onClick={tools.disconnect}>Disconnect</button>}
    </div>
    {connecting && session.connectionStatus && <p className={styles.message} role="status">{session.connectionStatus}</p>}
    {session.connectionError && <p className={styles.error} role="alert">{session.connectionError}</p>}
    {connected && !firmwareUpdate && transport === "serial" && <p className={styles.message}>Watch Tools require Bluetooth. Disconnect USB, then connect over Bluetooth.</p>}
    {connected && firmwareUpdate && Number.isFinite(info.batteryLevel) && info.batteryLevel < 20 && <p className={styles.warning} role="alert">Battery low ({info.batteryLevel}%). Connect your watch to power before updating.</p>}
    <div className={styles.disclosures}>
      <details>
        <summary>{available || connected ? "Connection help" : `${transport === "ble" ? "Bluetooth" : "USB"} unavailable in this browser`}</summary>
        <p>Connect watch opens the device picker. The name filter shows names starting with this text, including watches with a unique suffix. Matching is case-sensitive; leave it empty to show all devices.</p>
        <p>For your first Bluetooth connection, enable <strong>Pairable</strong> in <strong>Settings → Bluetooth</strong> on the watch and accept pairing on your computer. Use a browser with Web Bluetooth support, such as Chrome or Edge.</p>
        {firmwareUpdate && <><p>For USB, open <strong>Apps → Update</strong> on the watch and enable USB, then connect using Chrome or Edge on desktop. USB updates are faster than Bluetooth.</p><p>If the watch is not responding, hold the two right buttons for 25 seconds to enter MCUBoot recovery. On older firmware, enable Bluetooth in the watch’s Update app if automatic setup fails.</p></>}
      </details>
      {connected && <details><summary>Device details</summary><dl className={styles.deviceDetails}><div><dt>Hardware</dt><dd>{info.hardwareVersion || "Not reported"}</dd></div><div><dt>Firmware</dt><dd>{info.firmwareVersion || "Not reported"}</dd></div><div><dt>Battery</dt><dd>{Number.isFinite(info.batteryLevel) ? `${info.batteryLevel}%` : "Not reported"}</dd></div></dl></details>}
    </div>
  </section>;
}
