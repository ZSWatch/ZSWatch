import React, { useEffect, useRef, useState } from "react";
import Layout from "@theme/Layout";
import Link from "@docusaurus/Link";
import useBaseUrl from "@docusaurus/useBaseUrl";
import backgrounds from "./backgrounds.json";
import useWatchTools from "./useWatchTools";
import { useWatchSession } from "../WatchSession/context";
import { LogsPanel, CrashesPanel } from "./DiagnosticsPanels";
import { cropRectangle, drawBackground, loadImage, validateBackground } from "./imageProcessing";
import styles from "./styles.module.css";
import WatchConnection from "../WatchSession/WatchConnection";

const centeredCrop = { zoom: 1, x: 0, y: 0 };
const fileTypes = ["image/png", "image/jpeg", "image/webp", "image/gif", "image/bmp"];
const panels = [["watchface", "Backgrounds"], ["logs", "Logs"], ["crashes", "Crash reports"]];

function ToolIcon({ panel }) {
  return <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.8" strokeLinecap="round" strokeLinejoin="round" aria-hidden="true">
    {panel === "watchface" ? <><rect x="3" y="3" width="18" height="18" rx="3" /><circle cx="8" cy="8" r="1.5" /><path d="m3 17 6-6 5 5 3-3 4 4" /></> : panel === "logs" ? <><path d="M9 6h12M9 12h12M9 18h12" /><path d="m3 5 2 1-2 1m0 4 2 1-2 1m0 4 2 1-2 1" /></> : <><path d="m12 3 10 18H2L12 3Z" /><path d="M12 9v5m0 3h.01" /></>}
  </svg>;
}

export default function WatchToolsApp() {
  const tools = useWatchTools();
  const { diagnostics } = useWatchSession();
  const [panel, setPanel] = useState("watchface");
  const panelTabs = useRef([]);
  const assets = useBaseUrl("/watchface-backgrounds/");
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

  function navigateTabs(event, index) {
    let next;
    if (event.key === "ArrowRight") next = (index + 1) % panels.length;
    else if (event.key === "ArrowLeft") next = (index + panels.length - 1) % panels.length;
    else if (event.key === "Home") next = 0;
    else if (event.key === "End") next = panels.length - 1;
    else return;
    event.preventDefault();
    setPanel(panels[next][0]);
    panelTabs.current[next]?.focus();
  }

  useEffect(() => {
    if (selection?.image && canvas.current) {
      try { drawBackground(canvas.current, selection.image, selection.kind === "custom" ? crop : centeredCrop); }
      catch (failure) { setImageError(failure.message); }
    }
  }, [selection, crop, panel]);
  useEffect(() => {
    // Show a real library image immediately; this only selects a preview.
    chooseBuiltin(backgrounds[0].backgrounds[1]);
    return () => { imageVersion.current++; };
  }, [assets]);

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
        <h1>Watch Tools</h1>
        <p>Backgrounds, live logs and crash reports for your watch.</p>
      </header>

      <WatchConnection />
      <div className={styles.toolFrame}>
        <div className={styles.toolNavigation}>
          <div className={styles.toolTabs} role="tablist" aria-label="Watch tools">
            {panels.map(([key, label], index) => <button key={key} ref={(node) => { panelTabs.current[index] = node; }} id={`watch-tool-${key}-tab`} role="tab" aria-selected={panel === key} aria-controls="watch-tool-panel" tabIndex={panel === key ? 0 : -1} className={`${styles.toolTab} ${panel === key ? styles.selectedToolTab : ""}`} onClick={() => setPanel(key)} onKeyDown={(event) => navigateTabs(event, index)}>
              <ToolIcon panel={key} /><span>{label}</span>
            </button>)}
          </div>
        </div>

        {(tools.status || tools.progress || tools.error || panel === "watchface" && imageError) && <div className={styles.operationStatus}>
          {tools.status && <p role="status" aria-live="polite" className={tools.phase === "success" ? styles.success : styles.help}>{tools.status}</p>}
          {tools.progress && <><progress className={styles.progress} aria-label="Transfer progress" max="100" value={tools.progress.percentage} /><span>{tools.progress.percentage}% · {(tools.progress.bytes / 1024).toFixed(1)} / {(tools.progress.total / 1024).toFixed(1)} KB</span></>}
          {["uploading", "reading"].includes(tools.phase) && <button className={styles.secondaryButton} onClick={tools.cancel}>Cancel transfer</button>}
          {(tools.error || panel === "watchface" && imageError) && <p role="alert" className={styles.error}>{panel === "watchface" && imageError || tools.error}</p>}
        </div>}

        <div className={styles.workspace} id="watch-tool-panel" role="tabpanel" aria-labelledby={`watch-tool-${panel}-tab`}>
          {panel === "logs" ? <LogsPanel tools={tools} diagnostics={diagnostics} /> : panel === "crashes" ? <CrashesPanel tools={tools} diagnostics={diagnostics} /> : <div className={styles.backgroundWorkspace}>
            <aside className={styles.sidebar}>
<section className={`${styles.card} ${styles.previewCard}`} aria-labelledby="preview-title">
            <div className={styles.cardHeading}><h2 id="preview-title">Preview</h2><label className={styles.check}><input type="checkbox" checked={showTime} onChange={(event) => setShowTime(event.target.checked)} />Show time</label></div>
            <div className={`${styles.watch} ${selection?.kind === "custom" ? styles.draggable : ""}`} onPointerDown={startDrag} onPointerMove={moveDrag} onPointerUp={() => { drag.current = null; }} onPointerCancel={() => { drag.current = null; }}>
              <canvas ref={canvas} width="240" height="240" aria-label="Watchface background preview" className={selection ? styles.canvas : styles.emptyCanvas} />
              {showTime && <div className={styles.clock} aria-hidden="true"><span>THU 01</span><strong>14:28</strong><span>Preview</span></div>}
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
              <button className={styles.secondaryButton} disabled={!tools.connected || tools.busy} onClick={tools.restore}>{tools.phase === "restoring" ? "Restoring…" : "Restore default"}</button>
              <p className={styles.help}>Backgrounds are fitted to the 240×240 round display. The watch updates without restarting.</p>
            </div>
          </section>
            </aside>
<section className={styles.library} aria-labelledby="background-title">
          <div className={styles.uploadCard} onDragOver={(event) => event.preventDefault()} onDrop={(event) => { event.preventDefault(); if (!disabled) chooseFile(event.dataTransfer.files[0]); }}>
            <div><h2>Your own image</h2><p>Choose a photo or drop it here.</p></div>
            <button className={styles.secondaryButton} disabled={disabled} onClick={() => fileInput.current.click()}>Choose image</button>
            <input ref={fileInput} id="background-file" hidden type="file" accept="image/png,image/jpeg,image/webp,image/gif,image/bmp" disabled={disabled} onChange={(event) => { chooseFile(event.target.files[0]); event.target.value = ""; }} />
            <span className={styles.help}>PNG, JPEG, WebP, GIF or BMP · up to 20 MB · processed on your device</span>
          </div>
          <div className={styles.libraryHeading}><div><h2 id="background-title">Background library</h2></div><span className={styles.help}>33 backgrounds from the companion app</span></div>
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
        </section>
          </div>}
        </div>
        <footer className={styles.workspaceFooter}>Images and diagnostics are processed locally in your browser.</footer>
      </div>
    </main>
  </Layout>;
}
