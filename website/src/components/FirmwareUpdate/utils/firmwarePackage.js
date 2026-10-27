import JSZip from "jszip";
import { binaryFileNameToImageId } from "./imageSlots";

const basename = (name) => name.split("/").pop().toLowerCase();
const find = (zip, name) => Object.values(zip.files).find((entry) => !entry.dir && basename(entry.name) === name);

async function loadZip(data) {
  try { return await JSZip.loadAsync(data); }
  catch { throw new Error("Invalid ZIP file. Select a complete firmware package or DFU ZIP downloaded from GitHub."); }
}

// Accept both a DFU archive and the complete release/Actions download.
export async function readFirmwarePackage(file, imageInfo, { rotated = false, imageNumber, allowStandalone = true } = {}) {
  if (!allowStandalone && !file.name.toLowerCase().endsWith(".zip")) throw new Error("Choose a firmware ZIP. Enable Advanced to select a standalone BIN.");
  let filesystem = null;
  let files;
  let manifest = null;
  let variant = "Standard";
  if (file.name.toLowerCase().endsWith(".zip")) {
    let zip = await loadZip(await file.arrayBuffer());
    const standard = find(zip, "dfu_application.zip");
    const rotation = find(zip, "dfu_application_rotated.zip");
    const resources = find(zip, "lvgl_resources_raw.bin");
    if (resources) {
      filesystem = new File([await resources.async("arraybuffer")], "lvgl_resources_raw.bin");
      if (!filesystem.size) throw new Error("The filesystem image is empty.");
    }
    if (standard || rotation) {
      const selected = rotated ? rotation : standard;
      if (!selected) throw new Error(`${rotated ? "Rotated" : "Standard"} firmware is missing from this package. Choose the available display variant.`);
      zip = await loadZip(await selected.async("arraybuffer"));
      variant = rotated ? "Rotated" : "Standard";
    } else if (basename(file.name) === "dfu_application_rotated.zip") {
      variant = "Rotated";
    }
    const manifestFile = find(zip, "manifest.json");
    if (manifestFile) {
      try { manifest = JSON.parse(await manifestFile.async("string")); }
      catch { throw new Error("The firmware manifest is invalid. Download the package again."); }
      if (!Array.isArray(manifest?.files)) throw new Error("Invalid firmware manifest: files must be an array.");
    }
    files = await Promise.all(Object.values(zip.files).filter((entry) => !entry.dir && entry.name.toLowerCase().endsWith(".bin")).map(async (entry) => ({ name: basename(entry.name), data: await entry.async("arraybuffer") })));
  } else {
    const name = basename(file.name);
    if (name === "lvgl_resources_raw.bin") {
      if (!file.size) throw new Error("The filesystem image is empty.");
      return { images: [], filesystem: file, name: file.name, variant };
    }
    files = [{ name, data: await file.arrayBuffer() }];
  }

  const images = [];
  if (imageNumber !== undefined && (!Number.isInteger(imageNumber) || imageNumber < 0 || imageNumber > 2)) throw new Error("Choose image index 0, 1 or 2 for a standalone BIN.");
  for (const { name, data } of files) {
    const entry = manifest?.files.find((item) => basename(item.file || "") === name);
    if (manifest && !entry) continue;
    const slot = entry ? (entry.image_index == null ? NaN : Number(entry.image_index)) : (binaryFileNameToImageId[name] ?? (files.length === 1 && !file.name.toLowerCase().endsWith(".zip") ? imageNumber : undefined));
    if (slot === undefined) continue;
    if (!Number.isInteger(slot) || slot < 0) throw new Error(`Invalid image index for ${name}.`);
    if (images.some((image) => image.imageNumber === slot)) throw new Error(`Multiple binaries target image ${slot}.`);
    const info = await imageInfo(data);
    images.push({ name, fileData: data, fileSize: data.byteLength, imageNumber: slot, board: entry?.board, ...info, isUploaded: false, isUploading: false });
  }
  if (!images.length && !filesystem) throw new Error("No supported firmware or filesystem found. Select a complete release ZIP, dfu_application.zip or a firmware BIN.");
  return { images: images.sort((a, b) => a.imageNumber - b.imageNumber), filesystem, name: file.name, variant };
}
