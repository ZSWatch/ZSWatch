import { binaryFileNameToImageIdSerialRecovery } from "./imageSlots";

function checkCancelled(signal) {
  if (signal.aborted) throw new Error("Update cancelled. You can retry with the selected package.");
}

function waitForNetworkCore(signal) {
  return new Promise((resolve, reject) => {
    const cancel = () => { clearTimeout(timer); reject(new Error("Update cancelled.")); };
    const timer = setTimeout(() => { signal.removeEventListener("abort", cancel); resolve(); }, 30000);
    signal.addEventListener("abort", cancel, { once: true });
    if (signal.aborted) cancel();
  });
}

export async function applyUploadedFirmware({ mcumgr, images, recovery, onStage }) {
  if (!recovery) {
    onStage("Confirming firmware");
    for (const image of images) {
      if (!/^[\da-f]{64}$/i.test(image.hash)) throw new Error(`Invalid image hash for ${image.name}.`);
      const hash = Uint8Array.from(image.hash.match(/.{2}/g).map((byte) => parseInt(byte, 16)));
      await mcumgr.request(2, 1, 0, { hash, confirm: true });
    }
  }
  onStage("Restarting watch");
  if (mcumgr.restartAndReconnect) return mcumgr.restartAndReconnect(onStage);
  await mcumgr.request(2, 0, 5);
}

export function verifyInstalledImages(expected, installed) {
  const missing = expected.filter((image) => !installed.some((actual) => {
    const hash = typeof actual.hash === "string" ? actual.hash : Array.from(actual.hash || [], (byte) => byte.toString(16).padStart(2, "0")).join("");
    return (actual.image ?? 0) === image.imageNumber && hash.toLowerCase() === image.hash.toLowerCase() && actual.active === true;
  }));
  if (missing.length) throw new Error(`The watch reconnected, but the new firmware could not be verified: ${missing.map((image) => image.name).join(", ")}. Check the installed images before retrying.`);
}

export async function runFirmwareUpdate({ selection, mode, recovery, signal, transfer, applyFirmware, restartFilesystem, onImageUploaded, log, waitForNetCore = waitForNetworkCore }) {
  if (!["both", "firmware", "filesystem"].includes(mode)) throw new Error("Unknown update mode.");
  if (mode !== "firmware" && (!selection.filesystem || recovery)) throw new Error("Filesystem upload requires an image and application mode.");
  if (mode !== "filesystem" && !selection.images.length) throw new Error("Select firmware first.");
  checkCancelled(signal);
  const steps = (mode === "both" ? 1 : 0) + (mode === "filesystem" ? 1 : selection.images.length);
  let step = 0;
  if (mode !== "firmware") {
    const data = await selection.filesystem.arrayBuffer();
    checkCancelled(signal);
    await transfer(data, 0, "Uploading filesystem", signal, ++step, steps);
    checkCancelled(signal);
  }
  if (mode !== "filesystem") {
    for (const image of selection.images) {
      checkCancelled(signal);
      const slot = recovery ? (binaryFileNameToImageIdSerialRecovery[image.name] ?? (image.imageNumber * 2 + 1)) : image.imageNumber;
      await transfer(image.fileData, slot, `Uploading ${image.name}`, signal, ++step, steps);
      checkCancelled(signal);
      onImageUploaded(image);
      if (recovery && slot === 3) {
        log("Waiting 30 seconds for the network core to load...");
        await waitForNetCore(signal);
        checkCancelled(signal);
      }
    }
    await applyFirmware(selection.images);
  } else await restartFilesystem();
}
