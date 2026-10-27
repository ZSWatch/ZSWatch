import { useEffect, useRef, useState } from "react";
import { fetchArtifacts, fetchReleases, downloadRelease } from "../utils/githubApi";
import { demoBuilds, demoReleases, githubDemoEnabled } from "../utils/githubDemo";

export const useFirmwareManagement = (githubConfig, loadFile) => {
  const [firmwares, setFirmwares] = useState([]);
  const [releases, setReleases] = useState([]);
  const [isFetchingFirmwares, setIsFetchingFirmwares] = useState(false);
  const [isFetchingReleases, setIsFetchingReleases] = useState(false);
  const [errors, setErrors] = useState({});
  const [downloadStatus, setDownloadStatus] = useState("");
  const [isDownloading, setIsDownloading] = useState(false);
  const [fallbackAsset, setFallbackAsset] = useState(null);
  const [demoMode, setDemoMode] = useState(false);
  const [selectedAssetId, setSelectedAssetId] = useState(null);
  const downloadRef = useRef(null);
  const fetchingRef = useRef(new Set());
  useEffect(() => () => downloadRef.current?.abort(), []);
  useEffect(() => { setDemoMode(githubDemoEnabled()); refresh("releases"); }, []);

  const refresh = async (source) => {
    if (fetchingRef.current.has(source)) return;
    fetchingRef.current.add(source);
    const setFetching = source === "releases" ? setIsFetchingReleases : setIsFetchingFirmwares;
    setFetching(true);
    setErrors((previous) => ({ ...previous, [source]: "" }));
    try {
      const items = githubDemoEnabled()
        ? (source === "releases" ? demoReleases : demoBuilds)
        : (source === "releases" ? await fetchReleases(githubConfig) : await fetchArtifacts(githubConfig));
      (source === "releases" ? setReleases : setFirmwares)(items);
    } catch (error) {
      setErrors((previous) => ({ ...previous, [source]: error.message }));
    } finally { setFetching(false); fetchingRef.current.delete(source); }
  };

  const selectRelease = async (asset) => {
    if (asset.demo && githubDemoEnabled()) {
      setSelectedAssetId(asset.id);
      setFallbackAsset(null);
      setDownloadStatus("");
      return;
    }
    if (downloadRef.current) return;
    const previousStatus = downloadStatus;
    const previousFallback = fallbackAsset;
    const controller = new AbortController();
    downloadRef.current = controller;
    setIsDownloading(true);
    setFallbackAsset(null);
    setDownloadStatus(`Downloading ${asset.name}...`);
    try {
      const file = await downloadRelease(asset, controller.signal, (percentage) => setDownloadStatus(`Downloading ${asset.name}${percentage === null ? "..." : `: ${percentage}%`}`));
      const loaded = await loadFile(file);
      if (loaded) {
        setSelectedAssetId(asset.id);
        setDownloadStatus("");
      } else if (loaded === false) {
        setDownloadStatus("The downloaded package could not be loaded. See Firmware package for details.");
      } else {
        setDownloadStatus(previousStatus);
        setFallbackAsset(previousFallback);
      }
    } catch (error) {
      if (controller.signal.aborted) setDownloadStatus("Download cancelled.");
      else {
        setDownloadStatus("The browser could not download this release directly. Download the ZIP below, then select it in Firmware Package. No extraction is needed.");
        setFallbackAsset(asset);
      }
    } finally { downloadRef.current = null; setIsDownloading(false); }
  };

  return { firmwares, releases, errors, isFetchingFirmwares, isFetchingReleases, refresh, selectRelease, isDownloading, downloadStatus, fallbackAsset, demoMode, selectedAssetId, cancelDownload: () => downloadRef.current?.abort() };
};
