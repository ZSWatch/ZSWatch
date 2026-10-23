import React, { useEffect, useRef, useState } from "react";
import { artifactUrl, boardFromAsset, compatibleAssets } from "../utils/githubApi";
import { boardFromHardwareVersion } from "../utils/watchBluetooth";
import styles from "./PrebuiltFirmwares.module.css";

const sources = [["releases", "Releases"], ["builds", "CI Builds"]];
const date = (value) => value ? new Date(value).toLocaleDateString() : "";
const buildTypeOf = (name) => /_debug(?:\.zip)?$/i.test(name) ? "Debug" : /_release(?:\.zip)?$/i.test(name) ? "Release" : "Firmware";

export default function PrebuiltFirmwares({ manager, githubConfig, disabled, hardwareVersion }) {
  const [source, setSource] = useState("releases");
  const [selectedVersion, setSelectedVersion] = useState("");
  const tabRefs = useRef([]);
  const detectedBoard = boardFromHardwareVersion(hardwareVersion);
  const [boardOverride, setBoardOverride] = useState(null);
  const board = boardOverride ?? detectedBoard;
  useEffect(() => { setBoardOverride(null); }, [detectedBoard]);
  const [buildType, setBuildType] = useState("");
  const { releases, firmwares, isFetchingReleases, isFetchingFirmwares, errors, refresh, selectRelease, downloadStatus, fallbackAsset, isDownloading, cancelDownload, demoMode, selectedAssetId } = manager;
  const fetching = source === "releases" ? isFetchingReleases : isFetchingFirmwares;
  const assets = [...releases.flatMap((item) => item.assets), ...firmwares.flatMap((item) => item.artifacts)];
  const boards = [...new Set([detectedBoard, ...assets.map((asset) => boardFromAsset(asset.name))].filter(Boolean))].sort();
  const filtered = (items) => compatibleAssets(items, board).filter((asset) => !buildType || buildTypeOf(asset.name).toLowerCase() === buildType);
  const versions = (source === "releases" ? releases : firmwares).filter((item) => filtered(item.assets || item.artifacts).length);
  const keyOf = (item) => String(item.id ?? item.runId);
  const current = versions.find((item) => keyOf(item) === selectedVersion) || versions[0];
  const selectSource = (key) => {
    setSource(key);
    setSelectedVersion("");
    if (!(key === "releases" ? releases : firmwares).length && !errors[key]) refresh(key);
  };
  const tabKeyDown = (event, index) => {
    const next = event.key === "Home" ? 0 : event.key === "End" ? 1 : ["ArrowLeft", "ArrowRight"].includes(event.key) ? 1 - index : null;
    if (next === null) return;
    event.preventDefault();
    selectSource(sources[next][0]);
    tabRefs.current[next]?.focus();
  };

  return <div className={styles.catalog}>
    <div role="tablist" aria-label="Firmware source" className={styles.tabs}>
      {sources.map(([key, label], index) => <button key={key} ref={(node) => { tabRefs.current[index] = node; }} id={`firmware-${key}-tab`} role="tab" aria-selected={source === key} aria-controls="firmware-source-panel" tabIndex={source === key ? 0 : -1} className={`${styles.tab} ${source === key ? styles.selectedTab : ""}`} onClick={() => selectSource(key)} onKeyDown={(event) => tabKeyDown(event, index)}>
        <span className={styles.tabTitle}>{label}</span>
      </button>)}
    </div>
    {demoMode && <p className={styles.hint} role="status">Demo data · no firmware downloads</p>}
    <div className={styles.filters}>
      <label className={styles.field}>Hardware<select aria-label="Hardware" value={board} onChange={(event) => setBoardOverride(event.target.value)}><option value="">All hardware</option>{boards.map((item) => <option key={item} value={item}>{item}</option>)}</select></label>
      <label className={styles.field}>Build type<select aria-label="Build type" value={buildType} onChange={(event) => setBuildType(event.target.value)}><option value="">All builds</option><option value="release">Release</option><option value="debug">Debug</option></select></label>
      <button disabled={fetching || disabled} className={styles.button} onClick={() => refresh(source)}>{fetching ? "Refreshing…" : "↻ Refresh"}</button>
    </div>
    <p className={styles.hint}>{detectedBoard ? `Detected: ${detectedBoard}` : "Hardware is detected on connection."}</p>
    {errors[source] && <div role="alert" className={styles.error}>{errors[source]}{process.env.NODE_ENV === "development" && <a href="?githubDemo=1">Use sample GitHub data for UI testing</a>}</div>}
    {downloadStatus && <div role="status" className={styles.notice}><p>{downloadStatus}</p>{isDownloading && <button className={styles.button} onClick={cancelDownload}>Cancel download</button>}{fallbackAsset && <a href={fallbackAsset.browser_download_url} target="_blank" rel="noopener noreferrer">Download {fallbackAsset.name} ↗</a>}</div>}
    <div id="firmware-source-panel" role="tabpanel" aria-labelledby={`firmware-${source}-tab`}>
      {fetching && <p role="status">Loading {source === "releases" ? "releases" : "CI builds"}…</p>}
      {!fetching && !current && <div className={styles.notice}>No matching {source === "releases" ? "releases" : "CI builds"}. Change the filters or refresh.</div>}
      {current && <>
        <label className={styles.field}>{source === "releases" ? "Firmware version" : "Branch / build"}<select aria-label={source === "releases" ? "Firmware version" : "Branch / build"} value={keyOf(current)} onChange={(event) => setSelectedVersion(event.target.value)}>{versions.map((item) => <option key={keyOf(item)} value={keyOf(item)}>{source === "releases" ? `${item.name || item.tag_name} · ${date(item.published_at)}` : `${item.branch} · ${item.sha.slice(0, 7)} · ${date(item.createdAt)}`}</option>)}</select></label>
        <div className={styles.version}>
          {current.html_url && <a className={styles.releaseNotes} href={current.html_url} target="_blank" rel="noopener noreferrer">Release notes ↗</a>}
          {current.commitMessage && <p className={styles.commit} title={current.commitMessage}>{current.sha.slice(0, 7)} · {current.commitMessage.split("\n")[0]}</p>}
          <div className={styles.imageList} aria-label="Firmware images">
          {filtered(current.assets || current.artifacts).map((asset) => <div key={asset.id} className={`${styles.image} ${selectedAssetId === asset.id ? styles.selectedImage : ""}`}>
            <div className={styles.imageInfo}><div className={styles.imageTitle}><strong>{boardFromAsset(asset.name) || "Firmware"}</strong><span className={styles.badge}>{buildTypeOf(asset.name)}</span>{selectedAssetId === asset.id && <span className={styles.imageSelected}>✓ Selected</span>}{!!(asset.size || asset.size_in_bytes) && <span className={styles.hint}>{((asset.size || asset.size_in_bytes) / 1048576).toFixed(1)} MB</span>}</div><span className={styles.filename} title={asset.name}>{asset.name}</span></div>
            {source === "builds" && !asset.demo ? <a className={styles.selectImage} href={artifactUrl(githubConfig, current.runId, asset.id)} target="_blank" rel="noopener noreferrer">Download ZIP ↗</a> : <button className={styles.selectImage} disabled={disabled || isDownloading} onClick={() => selectRelease(asset)}>{asset.demo ? "Select (demo)" : selectedAssetId === asset.id ? "Reselect" : "Select"}</button>}
          </div>)}
          </div>
        </div>
      </>}
    </div>
    {source === "builds" && !demoMode && <p className={styles.hint}>CI ZIPs require GitHub sign-in. Open the download in Firmware Package.</p>}
  </div>;
}
