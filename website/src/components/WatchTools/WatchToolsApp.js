import React, { useEffect, useRef, useState } from "react";
import Layout from "@theme/Layout";
import Link from "@docusaurus/Link";
import useBaseUrl from "@docusaurus/useBaseUrl";
import backgrounds from "./backgrounds.json";
import useWatchTools from "./useWatchTools";
import useDiagnostics from "./useDiagnostics";
import { LogsPanel, CrashesPanel } from "./DiagnosticsPanels";
import { cropRectangle, drawBackground, loadImage, validateBackground } from "./imageProcessing";
import styles from "./styles.module.css";

const centeredCrop = { zoom: 1, x: 0, y: 0 };
const fileTypes = ["image/png", "image/jpeg", "image/webp", "image/gif", "image/bmp"];

export default function WatchToolsApp() {
  const tools = useWatchTools();
  const diagnostics = useDiagnostics(tools);
  const [panel, setPanel] = useState("watchface");
  const assets = useBaseUrl("/watchface-backgrounds/");
  const [deviceName, setDeviceName] = useState("ZSWatch");
  const [group, setGroup] = useState("All");
  const [selection, setSelection] = useState(null);
  const [crop, setCrop] = useState(centeredCrop);
  const [loading, setLoading] = useState(false);
  const [imageError, setImageError] = useState("");
  const [showTime, setShowTime] = useState(true);
  const canvas = useRef(null);
  const fileInput = useRef(null);
  const imageVersion = useRef(0);
  const drag = useRef(null);
  const disabled = tools.busy || loading;

  useEffect(() => {
    if (selection?.image && canvas.current) {
      try { drawBackground(canvas.current, selection.image, selection.kind === "custom" ? crop : centeredCrop); }
      catch (failure) { setImageError(failure.message); }
    }
  }, [selection, crop, panel]);
  useEffect(() => () => { imageVersion.current++; }, []);

  async function chooseBuiltin(background) {
    const version = ++imageVersion.current;
    tools.clearMessages(); setImageError(""); setLoading(true);
    try {
      const image = await loadImage(assets + background.image);
      if (version === imageVersion.current) { setSelection({ kind: "builtin", name: background.name, background, image }); setCrop(centeredCrop); }
    } catch (failure) { if (version === imageVersion.current) setImageError(failure.message); }
    finally { if (version === imageVersion.current) setLoading(false); }
  }

  async function chooseFile(file) {
    if (!file || tools.busy) return;
    const version = ++imageVersion.current;
    tools.clearMessages(); setImageError(""); setLoading(true);
    let url;
    try {
      if (!fileTypes.includes(file.type)) throw new Error("Choose a PNG, JPEG, WebP, GIF or BMP image.");
      if (file.size > 20 * 1024 * 1024) throw new Error("Choose an image smaller than 20 MB.");
      url = URL.createObjectURL(file);
      const image = await loadImage(url);
      if (version === imageVersion.current) { setSelection({ kind: "custom", name: file.name, image }); setCrop(centeredCrop); }
    } catch (failure) { if (version === imageVersion.current) setImageError(failure.message); }
    finally { if (url) URL.revokeObjectURL(url); if (version === imageVersion.current) setLoading(false); }
  }

  async function getBackground(signal) {
    if (!selection) throw new Error("Choose a background first.");
    if (selection.kind === "custom") return drawBackground(canvas.current, selection.image, crop);
    const response = await fetch(assets + selection.background.binary, { signal });
    if (!response.ok) throw new Error("Could not load this background. Please try again.");
    return validateBackground(new Uint8Array(await response.arrayBuffer()));
  }

  async function download() {
    setImageError("");
    try {
      const bytes = await getBackground();
      const url = URL.createObjectURL(new Blob([bytes], { type: "application/octet-stream" }));
      const anchor = document.createElement("a");
      anchor.href = url; anchor.download = "watchface-background.bin"; anchor.click();
      setTimeout(() => URL.revokeObjectURL(url), 1000);
    } catch (failure) { setImageError(failure.message); }
  }

  function startDrag(event) {
    if (selection?.kind !== "custom" || disabled) return;
    event.currentTarget.setPointerCapture(event.pointerId);
    drag.current = { startX: event.clientX, startY: event.clientY, crop, size: event.currentTarget.getBoundingClientRect().width };
  }
  function moveDrag(event) {
    if (!drag.current || disabled) return;
    const start = drag.current;
    const source = cropRectangle(selection.image.naturalWidth, selection.image.naturalHeight, start.crop.zoom);
    const adjust = (position, delta, dimension) => dimension === source.size ? 0 : Math.max(-1, Math.min(1, position - delta / start.size * source.size * 2 / (dimension - source.size)));
    setCrop({ ...start.crop, x: adjust(start.crop.x, event.clientX - start.startX, selection.image.naturalWidth), y: adjust(start.crop.y, event.clientY - start.startY, selection.image.naturalHeight) });
  }

  return <Layout title="Watch Tools" description="Customize your ZSWatch and view logs and crash reports over Bluetooth.">
    <main className={styles.page}>
      <header className={styles.hero}>
        <p className={styles.eyebrow}>MAKE IT YOURS</p>
        <h1>Watch Tools</h1>
        <p>Customize your watch, capture live logs, and download crash reports.</p>
      </header>

      <div className={styles.filters} role="group" aria-label="Watch tools">
        {[["watchface", "Watchface"], ["logs", "Logs"], ["crashes", "Crash reports"]].map(([key, label]) => <button key={key} className={panel === key ? styles.activeFilter : styles.filter} aria-pressed={panel === key} onClick={() => setPanel(key)}>{label}</button>)}
      </div>

      <div className={styles.workspace}>
        <aside className={styles.sidebar}>
          <section className={styles.card} aria-labelledby="connection-title">
            <div className={styles.cardHeading}><h2 id="connection-title">Your watch</h2><span className={`${styles.badge} ${tools.connected ? styles.connected : ""}`}>{tools.connected ? "Connected" : "Disconnected"}</span></div>
            {tools.connected ? <>
              <dl className={styles.details}>
                <div><dt>Hardware</dt><dd>{tools.deviceInfo.hardwareVersion || "Not reported"}</dd></div>
                {tools.deviceInfo.firmwareVersion && <div><dt>Firmware</dt><dd>{tools.deviceInfo.firmwareVersion}</dd></div>}
                {Number.isFinite(tools.deviceInfo.batteryLevel) && <div><dt>Battery</dt><dd>{tools.deviceInfo.batteryLevel}%</dd></div>}
              </dl>
              <button className={styles.secondaryButton} disabled={tools.busy} onClick={tools.disconnect}>Disconnect</button>
            </> : <>
              <label className={styles.field} htmlFor="watch-name">Device name<input id="watch-name" value={deviceName} onChange={(event) => setDeviceName(event.target.value)} placeholder="ZSWatch" disabled={tools.busy} /></label>
              <button className={styles.primaryButton} disabled={!tools.available || tools.busy} onClick={() => tools.connect(deviceName)}>{tools.phase === "connecting" ? "Connecting…" : "Connect watch"}</button>
              <p className={styles.help}>For your first connection, enable <strong>Pairable</strong> in <strong>Settings → Bluetooth</strong> on your watch and accept pairing on your computer.</p>
              {!tools.available && <p className={styles.notice}>Bluetooth is unavailable in this browser. Background previews and crash file import still work. Use a browser with Web Bluetooth support to connect.</p>}
            </>}
          </section>

          {panel === "watchface" && <section className={`${styles.card} ${styles.previewCard}`} aria-labelledby="preview-title">
            <div className={styles.cardHeading}><h2 id="preview-title">Preview</h2><label className={styles.check}><input type="checkbox" checked={showTime} onChange={(event) => setShowTime(event.target.checked)} />Show time</label></div>
            <div className={`${styles.watch} ${selection?.kind === "custom" ? styles.draggable : ""}`} onPointerDown={startDrag} onPointerMove={moveDrag} onPointerUp={() => { drag.current = null; }} onPointerCancel={() => { drag.current = null; }}>
              <canvas ref={canvas} width="240" height="240" aria-label="Watchface background preview" className={selection ? styles.canvas : styles.emptyCanvas} />
              {showTime && <div className={styles.clock} aria-hidden="true"><span>THU 01</span><strong>14:28</strong><span>ZSWatch</span></div>}
            </div>
            <p className={styles.selectionName}>{loading ? "Opening image…" : selection?.name || "Choose a background"}</p>
            {selection?.kind === "custom" && <div className={styles.cropControls}>
              <p className={styles.help}>Drag the image to position it, or use the controls below.</p>
              <label className={styles.slider}>Zoom <span>{crop.zoom.toFixed(1)}×</span><input aria-label="Zoom" type="range" min="1" max="4" step="0.01" value={crop.zoom} disabled={disabled} onChange={(event) => setCrop({ ...crop, zoom: Number(event.target.value) })} /></label>
              <label className={styles.slider}>Horizontal position<input aria-label="Horizontal position" type="range" min="-1" max="1" step="0.01" value={crop.x} disabled={disabled} onChange={(event) => setCrop({ ...crop, x: Number(event.target.value) })} /></label>
              <label className={styles.slider}>Vertical position<input aria-label="Vertical position" type="range" min="-1" max="1" step="0.01" value={crop.y} disabled={disabled} onChange={(event) => setCrop({ ...crop, y: Number(event.target.value) })} /></label>
              <button className={styles.textButton} disabled={disabled} onClick={() => setCrop(centeredCrop)}>Reset crop</button>
            </div>}
            <div className={styles.applyArea}>
              <button className={styles.primaryButton} disabled={!tools.connected || !selection || disabled} onClick={() => tools.apply(getBackground)}>{tools.phase === "uploading" ? "Uploading…" : tools.phase === "applying" ? "Applying…" : "Apply to watch"}</button>
              <button className={styles.textButton} disabled={!selection || disabled} onClick={download}>Download background file</button>
              <p className={styles.help}>Backgrounds are fitted to the 240×240 round display. The watch updates without restarting.</p>
            </div>
          </section>}

          <section className={styles.card}>
            {tools.status && <p role="status" aria-live="polite" className={tools.phase === "success" ? styles.success : styles.help}>{tools.status}</p>}
            {tools.progress && <><progress className={styles.progress} aria-label="Transfer progress" max="100" value={tools.progress.percentage} /><p className={styles.help}>{tools.progress.percentage}% · {(tools.progress.bytes / 1024).toFixed(1)} / {(tools.progress.total / 1024).toFixed(1)} KB</p></>}
            {["uploading", "reading"].includes(tools.phase) && <button className={styles.secondaryButton} onClick={tools.cancel}>Cancel transfer</button>}
            {(tools.error || panel === "watchface" && imageError) && <p role="alert" className={styles.error}>{panel === "watchface" && imageError || tools.error}</p>}
            {panel === "watchface" ? <><button className={styles.secondaryButton} disabled={!tools.connected || tools.busy} onClick={tools.restore}>{tools.phase === "restoring" ? "Restoring…" : "Restore default background"}</button>
            <p className={styles.help}>Switch back to the background included with your firmware.</p></> : <p className={styles.help}>Logs and crash reports are processed locally in your browser.</p>}
          </section>
        </aside>

        {panel === "logs" ? <LogsPanel tools={tools} diagnostics={diagnostics} /> : panel === "crashes" ? <CrashesPanel tools={tools} diagnostics={diagnostics} /> : <section className={styles.library} aria-labelledby="background-title">
          <div className={styles.uploadCard} onDragOver={(event) => event.preventDefault()} onDrop={(event) => { event.preventDefault(); if (!disabled) chooseFile(event.dataTransfer.files[0]); }}>
            <div><span className={styles.uploadIcon} aria-hidden="true">＋</span><h2>Your own image</h2><p>Drop a photo here, then crop it to make it yours.</p></div>
            <button className={styles.secondaryButton} disabled={disabled} onClick={() => fileInput.current.click()}>Choose image</button>
            <input ref={fileInput} id="background-file" hidden type="file" accept="image/png,image/jpeg,image/webp,image/gif,image/bmp" disabled={disabled} onChange={(event) => { chooseFile(event.target.files[0]); event.target.value = ""; }} />
            <span className={styles.help}>PNG, JPEG, WebP, GIF or BMP · up to 20 MB · processed on your device</span>
          </div>
          <div className={styles.libraryHeading}><div><p className={styles.eyebrow}>THE COLLECTION</p><h2 id="background-title">Find your background</h2></div><span className={styles.help}>33 backgrounds from the companion app</span></div>
          <div className={styles.filters} role="group" aria-label="Background categories">
            {["All", ...backgrounds.map((item) => item.name)].map((name) => <button key={name} className={group === name ? styles.activeFilter : styles.filter} aria-pressed={group === name} onClick={() => setGroup(name)}>{name}</button>)}
          </div>
          {backgrounds.filter((item) => group === "All" || group === item.name).map((category) => <section key={category.name} className={styles.category} aria-label={category.name}>
            <h3>{category.name}</h3>
            <div className={styles.gallery}>
              {category.backgrounds.map((background) => <button key={background.binary} className={`${styles.tile} ${selection?.background === background ? styles.selectedTile : ""}`} aria-pressed={selection?.background === background} aria-label={background.name} disabled={disabled} onClick={() => chooseBuiltin(background)}>
                <img src={assets + background.image} alt="" loading="lazy" width="240" height="240" /><span>{background.name}</span>{selection?.background === background && <span className={styles.selectedMark} aria-hidden="true">✓</span>}
              </button>)}
            </div>
          </section>)}
          <p className={styles.footerNote}>Looking for firmware updates? <Link to="/update">Open the firmware updater →</Link></p>
        </section>}
      </div>
    </main>
  </Layout>;
}
