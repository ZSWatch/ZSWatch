#!/usr/bin/env python3
"""Generates the app and folder icons used by the app picker.

Every icon is a 56x56 RGB565A8 image: a colored disc with a white Font Awesome
glyph for apps, a dark disc with a colored ring and glyph for folders. Add a row
to APPS or FOLDERS and run from the app directory:

    python scripts/generate_app_icons.py [--preview preview.png]

Needs Pillow. Output goes to src/images/binaries/S/.
"""

import argparse
import struct
from pathlib import Path

from PIL import Image, ImageChops, ImageDraw, ImageFilter, ImageFont

APP_DIR = Path(__file__).resolve().parent.parent
FONT = APP_DIR / "lvgl_editor/fonts/FontAwesome5-Solid+Brands+Regular.woff"
OUT_DIR = APP_DIR / "src/images/binaries/S"

SIZE = 56
SCALE = 4  # Supersampling factor

# name: (Font Awesome 5 codepoint, disc color, glyph size relative to the disc)
APPS = {
    "settings":     (0xF013, 0x6E7A8F, 0.56),
    "music":        (0xF001, 0xA855F7, 0.50),
    "compass":      (0xF14E, 0xE5484D, 0.58),
    "fitness":      (0xF70C, 0x22C55E, 0.56),
    "hr":           (0xF21E, 0xFF2D6B, 0.54),
    "stopwatch":    (0xF2F2, 0xFF9500, 0.56),
    "timer":        (0xF252, 0x5E5CE6, 0.52),
    "voice_memo":   (0xF130, 0xFF6247, 0.54),
    "weather":      (0xF6C4, 0x2AA7E8, 0.62),
    "update":       (0xF019, 0x14B8A6, 0.50),
    "battery":      (0xF241, 0x65B32E, 0.62),
    "calculator":   (0xF1EC, 0xF97316, 0.52),
    "ppt_remote":   (0xF108, 0xD2472B, 0.56),
    "audio_test":   (0xF028, 0x8B5CF6, 0.56),
    "about":        (0xF129, 0x3B82F6, 0.50),
    "debug":        (0xF188, 0x9B51C4, 0.52),
    "watchface":    (0xF017, 0x0EA5E9, 0.56),
    "2048":         (0xF00A, 0xE0A91B, 0.50),
    "trivia":       (0xF128, 0xC026D3, 0.52),
    "altimeter":    (0xF6FC, 0x2F9E6E, 0.62),
    "fusion":       (0xF1B2, 0x0891B2, 0.54),
    "mic":          (0xF080, 0x3F8CFF, 0.52),
    "qr":           (0xF029, 0x596275, 0.50),
    "x_ray":        (0xF2DB, 0x0F9D8A, 0.54),
}

FOLDERS = {
    "tools":   (0xF0AD, 0xFF9F0A, 0.48),
    "games":   (0xF11B, 0x4C9BFF, 0.58),
    "fitness": (0xF70C, 0x34C759, 0.50),
    "system":  (0xF1DE, 0xFF5A52, 0.48),
    "sensors": (0xF519, 0x2DD4BF, 0.58),
    "random":  (0xF522, 0xF472B6, 0.58),
    "root":    (0xF07B, 0x9AA3B2, 0.50),
}

FOLDER_DISC = 0x1A1D24


def rgb(color):
    return ((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF)


def mix(color, other, amount):
    return tuple(round(c + (o - c) * amount) for c, o in zip(color, other))


def glyph_mask(codepoint, box):
    """Returns an L image of the glyph, fitted in a box x box square."""
    font = ImageFont.truetype(str(FONT), box)
    left, top, right, bottom = font.getbbox(chr(codepoint))
    fit = box / max(right - left, bottom - top)
    font = ImageFont.truetype(str(FONT), max(1, int(box * fit)))
    left, top, right, bottom = font.getbbox(chr(codepoint))
    mask = Image.new("L", (right - left, bottom - top), 0)
    ImageDraw.Draw(mask).text((-left, -top), chr(codepoint), font=font, fill=255)
    return mask


def paste_centered(canvas, mask, color, offset=(0, 0)):
    x = (canvas.width - mask.width) // 2 + offset[0]
    y = (canvas.height - mask.height) // 2 + offset[1]
    canvas.paste(Image.new("RGBA", mask.size, color), (x, y), mask)


def render(codepoint, color, glyph_scale, folder):
    big = SIZE * SCALE
    disc = Image.new("L", (big, big), 0)
    ImageDraw.Draw(disc).ellipse((0, 0, big - 1, big - 1), fill=255)

    base = rgb(color)
    if folder:
        img = Image.new("RGBA", (big, big), rgb(FOLDER_DISC) + (255,))
        ring = 2 * SCALE
        ImageDraw.Draw(img).ellipse((ring // 2, ring // 2, big - 1 - ring // 2, big - 1 - ring // 2),
                                    outline=mix(base, (0, 0, 0), 0.25) + (255,), width=ring)
        glyph_color = base + (255,)
    else:
        # Vertical gradient, lighter at the top
        top, bottom = mix(base, (255, 255, 255), 0.22), mix(base, (0, 0, 0), 0.22)
        img = Image.new("RGBA", (big, big))
        draw = ImageDraw.Draw(img)
        for y in range(big):
            draw.line((0, y, big, y), fill=mix(top, bottom, y / (big - 1)) + (255,))
        glyph_color = (255, 255, 255, 255)

    mask = glyph_mask(codepoint, int(big * glyph_scale))
    if not folder:
        shadow = Image.new("RGBA", (big, big), (0, 0, 0, 0))
        paste_centered(shadow, mask, mix(base, (0, 0, 0), 0.6) + (110,), offset=(0, SCALE))
        img.alpha_composite(shadow.filter(ImageFilter.GaussianBlur(SCALE)))
    glyph = Image.new("RGBA", (big, big), (0, 0, 0, 0))
    paste_centered(glyph, mask, glyph_color)
    img.alpha_composite(glyph)

    img.putalpha(ImageChops.multiply(img.getchannel("A"), disc))
    return img.resize((SIZE, SIZE), Image.LANCZOS)


def write_rgb565a8(img, path):
    """LVGL v9 binary image, same as LVGLImage.py --ofmt BIN --cf RGB565A8."""
    pixels = list(img.getdata())
    data = bytearray(struct.pack("<BBHHHHH", 0x19, 0x14, 0, img.width, img.height, img.width * 2, 0))
    for r, g, b, _ in pixels:
        data += struct.pack("<H", ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3))
    data += bytes(a for _, _, _, a in pixels)
    path.write_bytes(data)


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--preview", help="Also write a PNG contact sheet of all icons")
    args = parser.parse_args()

    icons = [(f"app_icon_{name}", spec, False) for name, spec in APPS.items()]
    icons += [(f"folder_icon_{name}", spec, True) for name, spec in FOLDERS.items()]

    rendered = []
    for name, (codepoint, color, glyph_scale), folder in icons:
        img = render(codepoint, color, glyph_scale, folder)
        write_rgb565a8(img, OUT_DIR / f"{name}.bin")
        rendered.append(img)

    if args.preview:
        cols = 7
        rows = (len(rendered) + cols - 1) // cols
        sheet = Image.new("RGB", (cols * (SIZE + 12) + 12, rows * (SIZE + 12) + 12), (0, 0, 0))
        for i, img in enumerate(rendered):
            sheet.paste(img, (12 + (i % cols) * (SIZE + 12), 12 + (i // cols) * (SIZE + 12)), img)
        sheet.resize((sheet.width * 2, sheet.height * 2), Image.LANCZOS).save(args.preview)

    print(f"Wrote {len(rendered)} icons to {OUT_DIR}")


if __name__ == "__main__":
    main()
