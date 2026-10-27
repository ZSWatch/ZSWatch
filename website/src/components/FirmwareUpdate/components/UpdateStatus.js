import React, { useState } from "react";
import styles from "./UpdateStatus.module.css";

export default function UpdateStatus({ update, isConnected, isSerialRecoveryMode, disabled }) {
  const [dragging, setDragging] = useState(false);
  const { selection, rotated, advanced, imageNumber, isBusy, isCritical, activity, status, error, progress, logs } = update;
  const images = selection?.images || [];
  const full = images.length > 0 && Boolean(selection?.filesystem);
  const start = images.length ? update.handleFileUpload : update.startFileSystemUpload;
  const canUpdate = images.length > 0 || (selection?.filesystem && !isSerialRecoveryMode);
  return <div className={styles.package}>
    <input ref={update.fileInputRef} type="file" accept={advanced ? ".zip,.bin" : ".zip"} aria-label="Select firmware package" hidden onChange={(event) => { update.handleFileChange(event); event.target.value = ""; }} />
    {selection ? <>
      <div className={styles.summary}>
        <div className={styles.fileIcon} aria-hidden="true">{selection.name.toLowerCase().endsWith(".zip") ? "ZIP" : "BIN"}</div>
        <div className={styles.fileInfo}>
          <strong>{selection.name}</strong>
          <span>{images.length ? `${selection.variant} · ${images.length} firmware ${images.length === 1 ? "image" : "images"}` : "Resources only"}{selection.filesystem ? ` · ${(selection.filesystem.size / 1024 / 1024).toFixed(1)} MB resources` : ""}</span>
        </div>
        <button className={styles.textButton} disabled={disabled} onClick={update.clear} aria-label="Remove selected package">Remove</button>
      </div>
      <div className={styles.actions}>
        <button className={styles.updateButton} disabled={disabled || !isConnected || !canUpdate} onClick={start}>{activity === "loading" ? "Reading…" : isBusy ? "Updating…" : images.length ? "Update firmware" : "Update resources"}</button>
        {full && <button className={`${styles.updateButton} ${styles.resourceButton}`} disabled={disabled || !isConnected || isSerialRecoveryMode} onClick={update.startBoth}>Update firmware + resources</button>}
        <button className={styles.textButton} disabled={disabled} onClick={() => update.fileInputRef.current.click()}>Change ZIP</button>
      </div>
      <p className={styles.hint}>{!isConnected ? "Connect your watch to update." : isSerialRecoveryMode ? "Recovery mode: firmware only. Resources require application mode." : "Applies the update and restarts your watch automatically."}</p>
      {full && <p className={styles.hint}>Firmware only keeps your current resources and is faster. Include resources when the new version changes icons or graphics.</p>}
    </> : <button className={`${styles.choose} ${dragging ? styles.dragging : ""}`} disabled={disabled} onClick={() => update.fileInputRef.current.click()} onDragOver={(event) => { event.preventDefault(); setDragging(true); }} onDragLeave={() => setDragging(false)} onDrop={(event) => { event.preventDefault(); setDragging(false); if (!disabled) update.loadFile(event.dataTransfer.files[0]); }}>
      <strong>{activity === "loading" ? "Reading package…" : "Choose firmware ZIP"}</strong><span>or drop it here · no extraction needed</span>
    </button>}
    <div className={styles.options}>
      <label><input type="checkbox" checked={rotated} onChange={(event) => update.changeRotated(event.target.checked)} disabled={disabled} />Rotated display</label>
      <label className={styles.advanced}><input type="checkbox" checked={advanced} onChange={(event) => update.setAdvanced(event.target.checked)} disabled={disabled} aria-controls="firmware-advanced-options" />Advanced</label>
    </div>
    {advanced && <div id="firmware-advanced-options" className={styles.advancedOptions}>
      <label>Standalone BIN image index <input type="number" aria-label="Image index for standalone BIN" min="0" max="2" value={imageNumber} onChange={(event) => update.setImageNumber(Number(event.target.value))} disabled={disabled} /></label>
      {images.length > 0 && <details><summary>Package contents</summary><ul>{images.map((image) => <li key={image.imageNumber}>{image.name} · {image.version} · {(image.fileSize / 1024).toFixed(0)} KB</li>)}{selection.filesystem && <li>{selection.filesystem.name}</li>}</ul></details>}
    </div>}
    {(isBusy || activity === "complete") && <p role="status" aria-live="polite" className={styles.hint}>{status}</p>}
    {error && <p role="alert" className={styles.error}>{error}</p>}
    {progress && <div className={styles.progress}>
      <div><span>{progress.stage}</span><span>{progress.percentage}%</span></div>
      <progress aria-label="Update progress" max="100" value={progress.percentage} />
      <span>Step {progress.step}/{progress.steps}{progress.speed > 0 ? ` · ${(progress.speed / 1024).toFixed(1)} KB/s` : ""}{progress.remaining != null ? ` · ~${progress.remaining}s remaining` : ""}</span>
    </div>}
    {isBusy && activity === "updating" && <button className={styles.textButton} onClick={update.cancel} disabled={isCritical}>{isCritical ? "Applying update…" : "Cancel update"}</button>}
    {logs.length > 0 && <details className={styles.logs}><summary>Update log</summary><pre aria-label="Update logs">{logs.join("\n")}</pre></details>}
  </div>;
}
