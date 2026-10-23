import { useCallback, useEffect, useRef, useState } from "react";
import { useHistory } from "@docusaurus/router";
import { readFirmwarePackage } from "../utils/firmwarePackage";
import { applyUploadedFirmware, runFirmwareUpdate, verifyInstalledImages } from "../utils/updateWorkflow";

export function useFirmwareUpdate(mcumgr, isSerialRecoveryMode, isConnected) {
  const history = useHistory();
  const [selection, setSelection] = useState(null);
  const [rotated, setRotated] = useState(false);
  const [advanced, setAdvanced] = useState(false);
  const [imageNumber, setImageNumber] = useState(0);
  const [activity, setActivity] = useState("idle");
  const [status, setStatus] = useState("Select a release package or DFU ZIP.");
  const [error, setError] = useState("");
  const [progress, setProgress] = useState(null);
  const [logs, setLogs] = useState([]);
  const fileInputRef = useRef(null);
  const fileFsInputRef = useRef(null);
  const sourceRef = useRef(null);
  const operationRef = useRef(null);
  const loadingRef = useRef(false);
  const log = useCallback((message) => {
    setStatus(message);
    setLogs((previous) => [...previous.slice(-199), `${new Date().toLocaleTimeString()} ${message}`]);
  }, []);
  const isBusy = activity === "loading" || activity === "updating";
  const isCritical = ["Confirming firmware", "Restarting watch", "Reconnecting to watch", "Verifying installed images"].includes(progress?.stage);

  useEffect(() => {
    if (!isBusy) return;
    const unblock = history.block(() => {
      log("Finish or cancel the current operation before leaving the update page.");
      return false;
    });
    const beforeUnload = (event) => { event.preventDefault(); event.returnValue = ""; };
    window.addEventListener("beforeunload", beforeUnload);
    let wakeLock;
    let disposed = false;
    const acquire = async () => {
      if (document.visibilityState !== "visible" || disposed) return;
      try {
        const lock = await navigator.wakeLock?.request("screen");
        if (disposed) await lock?.release();
        else wakeLock = lock;
      } catch { /* Updating is still supported when wake lock is unavailable. */ }
    };
    acquire();
    document.addEventListener("visibilitychange", acquire);
    return () => {
      disposed = true;
      unblock();
      wakeLock?.release();
      window.removeEventListener("beforeunload", beforeUnload);
      document.removeEventListener("visibilitychange", acquire);
    };
  }, [isBusy, history, log]);

  useEffect(() => () => {
    operationRef.current?.abort();
    mcumgr.cancelUpload();
  }, [mcumgr]);

  const loadFile = async (file, rotate = rotated, slot = imageNumber) => {
    if (!file || operationRef.current || loadingRef.current) return null;
    loadingRef.current = true;
    setActivity("loading");
    setError("");
    log(`Reading ${file.name}...`);
    try {
      const loaded = await readFirmwarePackage(file, (data) => mcumgr.imageInfo(data), { rotated: rotate, imageNumber: slot, allowStandalone: advanced });
      setSelection(loaded);
      sourceRef.current = file;
      setRotated(rotate);
      setProgress(null);
      log(`${loaded.name}: ${loaded.images.length} firmware image(s)${loaded.filesystem ? " + filesystem" : ""} ready.`);
      return loaded;
    } catch (failure) {
      setError(failure.message);
      log(failure.message);
      return false;
    } finally {
      loadingRef.current = false;
      setActivity("idle");
    }
  };

  const selectFilesystem = (event) => {
    const file = event.target.files[0];
    if (!file || operationRef.current || loadingRef.current) return;
    if (!file.size) { setError("The filesystem image is empty."); return; }
    setSelection((previous) => ({ ...(previous || { name: file.name, images: [], variant: "Standard" }), filesystem: file }));
    setError("");
    log(`Filesystem selected: ${file.name}`);
  };

  const checkCancelled = (signal) => {
    if (signal.aborted) throw new Error("Update cancelled. You can retry with the selected package.");
  };
  const transfer = async (data, image, stage, signal, step, steps) => {
    checkCancelled(signal);
    log(stage);
    setSelection((previous) => ({ ...previous, images: previous.images.map((image) => ({ ...image, isUploading: stage === `Uploading ${image.name}` })) }));
    const started = performance.now();
    let lastTime = started;
    let lastBytes = 0;
    let speed = 0;
    let history = [];
    setProgress({ stage, percentage: 0, bytes: 0, total: data.byteLength, speed: 0, step, steps, history });
    await mcumgr.upload(data, image, stage === "Uploading filesystem", ({ bytes, total, percentage }) => {
      const now = performance.now();
      if (now - lastTime >= 500) {
        speed = (bytes - lastBytes) / ((now - lastTime) / 1000);
        lastTime = now;
        lastBytes = bytes;
        history = [...history.slice(-59), speed];
      }
      setProgress({ stage, percentage, bytes, total, speed, remaining: speed > 0 ? Math.ceil((total - bytes) / speed) : null, step, steps, history });
    });
    checkCancelled(signal);
  };

  const onStage = (stage) => {
    log(stage);
    setProgress((previous) => ({ ...previous, stage }));
  };
  const confirmAndReset = async (images) => {
    const result = await applyUploadedFirmware({ mcumgr, images, recovery: isSerialRecoveryMode, onStage });
    if (result?.verified) {
      verifyInstalledImages(images, result.images);
      log("Update complete. The watch reconnected and the installed firmware was verified.");
    } else log(result?.message || "Update sent; reconnect to verify the installed images.");
  };
  const restartFilesystem = async () => {
    onStage("Restarting watch");
    await mcumgr.sleep(500);
    const result = await mcumgr.restartAndReconnect(onStage);
    log(result.verified ? "Filesystem upload complete. The watch restarted and reconnected." : result.message);
  };

  const run = async (mode) => {
    if (operationRef.current || loadingRef.current || !isConnected || !selection) return;
    if ((mode === "both" || mode === "filesystem") && (!selection.filesystem || isSerialRecoveryMode)) return;
    if (mode !== "filesystem" && !selection.images.length) return;
    const operation = new AbortController();
    operationRef.current = operation;
    setActivity("updating");
    setError("");
    setSelection((previous) => ({ ...previous, images: previous.images.map((image) => ({ ...image, isUploaded: false, isUploading: false })) }));
    try {
      setProgress(null);
      log("Preparing watch for update...");
      await mcumgr.ensureUpdateReady();
      checkCancelled(operation.signal);
      if (mcumgr.deviceInfo.batteryLevel < 20) log(`Battery low (${mcumgr.deviceInfo.batteryLevel}%). Connect the watch to power during the update.`);
      await runFirmwareUpdate({
        selection, mode, recovery: isSerialRecoveryMode, signal: operation.signal,
        transfer, applyFirmware: confirmAndReset, restartFilesystem, log,
        onImageUploaded: (image) => setSelection((previous) => ({ ...previous, images: previous.images.map((item) => item.imageNumber === image.imageNumber ? { ...item, isUploaded: true, isUploading: false } : item) })),
      });
      setActivity("complete");
    } catch (failure) {
      setError(failure.message);
      log(failure.message);
      setActivity("failed");
    } finally {
      operationRef.current = null;
      setSelection((previous) => ({ ...previous, images: previous.images.map((image) => ({ ...image, isUploading: false })) }));
    }
  };

  return {
    selection, rotated, imageNumber, advanced, activity, status, error, progress, logs, isBusy, isCritical,
    setAdvanced: (value) => {
      if (isBusy) return;
      setAdvanced(value);
      if (!value && sourceRef.current?.name.toLowerCase().endsWith(".bin")) {
        setSelection(null); sourceRef.current = null; setProgress(null); setError(""); setActivity("idle");
        if (fileInputRef.current) fileInputRef.current.value = "";
        log("Select a release package or DFU ZIP.");
      }
    },
    setImageNumber: (value) => {
      if (isBusy || !Number.isInteger(value) || value < 0 || value > 2) return;
      setImageNumber(value);
      if (sourceRef.current?.name.toLowerCase().endsWith(".bin")) loadFile(sourceRef.current, rotated, value);
    },
    changeRotated: (value) => sourceRef.current ? loadFile(sourceRef.current, value) : setRotated(value),
    loadFile, fileInputRef, fileFsInputRef,
    handleFileChange: (event) => loadFile(event.target.files[0]),
    handleFileSystemSelection: selectFilesystem,
    handleFileUpload: () => run("firmware"),
    startFileSystemUpload: () => run("filesystem"),
    startBoth: () => run("both"),
    cancel: () => { if (!isCritical) { operationRef.current?.abort(); mcumgr.cancelUpload(); } },
    clear: () => { if (isBusy) return; setSelection(null); sourceRef.current = null; setProgress(null); setError(""); setActivity("idle"); log("Select a release package or DFU ZIP."); if (fileInputRef.current) fileInputRef.current.value = ""; if (fileFsInputRef.current) fileFsInputRef.current.value = ""; },
  };
}
