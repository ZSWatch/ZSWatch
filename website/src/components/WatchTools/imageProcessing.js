export const WATCH_SIZE = 240;
export const BACKGROUND_SIZE = 12 + WATCH_SIZE * WATCH_SIZE * 2;

// The companion app uses LVGL v9 RGB565, a 12-byte little-endian header,
// and black pixels outside the circle. Keep this format identical to the app.
export function rgbaToLvgl(rgba, width = WATCH_SIZE, height = WATCH_SIZE) {
  if (rgba.length !== width * height * 4 || width !== WATCH_SIZE || height !== WATCH_SIZE) {
    throw new Error("A watch background must contain exactly 240×240 RGBA pixels.");
  }
  const output = new Uint8Array(BACKGROUND_SIZE);
  const header = new DataView(output.buffer);
  header.setUint8(0, 0x19);
  header.setUint8(1, 0x12);
  header.setUint16(4, width, true);
  header.setUint16(6, height, true);
  header.setUint16(8, width * 2, true);
  for (let y = 0; y < height; y++) {
    for (let x = 0; x < width; x++) {
      const i = (y * width + x) * 4;
      const outside = (x - 120) ** 2 + (y - 120) ** 2 > 120 ** 2;
      const alpha = rgba[i + 3] / 255;
      const r = outside ? 0 : Math.round(rgba[i] * alpha);
      const g = outside ? 0 : Math.round(rgba[i + 1] * alpha);
      const b = outside ? 0 : Math.round(rgba[i + 2] * alpha);
      header.setUint16(12 + (y * width + x) * 2, ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3), true);
    }
  }
  return output;
}

export function validateBackground(bytes) {
  if (bytes.byteLength !== BACKGROUND_SIZE) throw new Error("Invalid watch background size.");
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  if (view.getUint8(0) !== 0x19 || view.getUint8(1) !== 0x12 || view.getUint16(2, true) !== 0 ||
      view.getUint16(4, true) !== WATCH_SIZE || view.getUint16(6, true) !== WATCH_SIZE ||
      view.getUint16(8, true) !== WATCH_SIZE * 2 || view.getUint16(10, true) !== 0) {
    throw new Error("Invalid watch background format. Choose a 240×240 LVGL RGB565 image.");
  }
  return bytes;
}

export function cropRectangle(width, height, zoom = 1, x = 0, y = 0) {
  if (width <= 0 || height <= 0 || ![width, height, zoom, x, y].every(Number.isFinite)) throw new Error("Invalid image dimensions.");
  const clamp = (value, min, max) => Math.max(min, Math.min(max, value));
  const size = Math.min(width, height) / clamp(zoom, 1, 4);
  return { x: (width - size) * (clamp(x, -1, 1) + 1) / 2, y: (height - size) * (clamp(y, -1, 1) + 1) / 2, size };
}

export function drawBackground(canvas, image, crop = {}) {
  const ctx = canvas.getContext("2d", { willReadFrequently: true });
  const source = cropRectangle(image.naturalWidth, image.naturalHeight, crop.zoom, crop.x, crop.y);
  canvas.width = canvas.height = WATCH_SIZE;
  ctx.fillStyle = "#000";
  ctx.fillRect(0, 0, WATCH_SIZE, WATCH_SIZE);
  ctx.drawImage(image, source.x, source.y, source.size, source.size, 0, 0, WATCH_SIZE, WATCH_SIZE);
  const pixels = ctx.getImageData(0, 0, WATCH_SIZE, WATCH_SIZE);
  const binary = rgbaToLvgl(pixels.data);
  // Match the binary's circular boundary in the preview as well.
  for (let y = 0; y < WATCH_SIZE; y++) for (let x = 0; x < WATCH_SIZE; x++) {
    if ((x - 120) ** 2 + (y - 120) ** 2 > 120 ** 2) {
      const i = (y * WATCH_SIZE + x) * 4;
      pixels.data[i] = pixels.data[i + 1] = pixels.data[i + 2] = 0;
    }
  }
  ctx.putImageData(pixels, 0, 0);
  return binary;
}

export function loadImage(url) {
  return new Promise((resolve, reject) => {
    const image = new Image();
    image.onload = () => resolve(image);
    image.onerror = () => reject(new Error("Could not open this image. Choose a PNG, JPEG, WebP or GIF file."));
    image.src = url;
  });
}
