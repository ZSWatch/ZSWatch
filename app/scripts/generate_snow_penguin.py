#!/usr/bin/env python3
# Copyright (c) 2026 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0
"""Generates the penguin of the Snow watchface as RGB565A8 images: each pose facing right and left.

Run from app/: python scripts/generate_snow_penguin.py [--preview penguin.png]
"""

import argparse
import struct

from PIL import Image

OUT = "src/images/binaries/S/face_snow_peng_{}_{}.bin"
# Display pixels per pixel of the drawing: the same as a grain of the watchface
SCALE = 2

COLORS = {
    "B": 0x6C86D6,  # Back and head: lighter than a real penguin, to show against the night sky
    "D": 0x4A62B0,  # Wing and shadow
    "W": 0xFFFFFF,  # Belly and face
    "k": 0x141A30,  # Eye
    "o": 0xFF9500,  # Beak and feet
    "r": 0xFF453A,  # Scarf
}

# All poses face right and are the same size. The bottom row is what touches the snow.
POSES = {
    # Waddling: two frames, the body rocks and a foot lifts
    "walk_a": [
        "................",
        ".....BBBBB......",
        "....BBBBBBB.....",
        "....BBWWkWBoo...",
        "....BWWWWWB.....",
        "...rrrrrrrr.....",
        "..rrBBWWWWBB....",
        "..r.BDWWWWWB....",
        "....BDWWWWWB....",
        "....BDWWWWWB....",
        "....BBWWWWWB....",
        ".....BBWWWB.....",
        ".....oo..oo.....",
        "....ooo.ooo.....",
    ],
    "walk_b": [
        "......BBBBB.....",
        ".....BBBBBBB....",
        ".....BBWWkWBoo..",
        ".....BWWWWWB....",
        "....rrrrrrrr....",
        "..rrrBBWWWWBB...",
        ".....BDWWWWWB...",
        ".....BDWWWWWB...",
        ".....BDWWWWWB...",
        ".....BBWWWWWB...",
        "......BBWWWB....",
        "......oo..oo....",
        ".....ooo........",
        "..........ooo...",
    ],
    # Downhill he slides on his belly
    "slide": [
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "r..........BBB..",
        "rr.......BBBBBB.",
        ".rrBBBBBBBBWkWBo",
        "oo.BDDDDBBWWWWBo",
        "ooBBWWWWWWWWWWB.",
        "..BWWWWWWWWWWB..",
        "...BBWWWWWWBB...",
        "................",
    ],
    # Head first in the snow, feet in the air. Drawn lower than the others so the head is buried.
    "fallen": [
        "................",
        "................",
        ".....oo..oo.....",
        ".....oo..oo.....",
        "......o..o......",
        ".....BBBBBB.....",
        "....BBWWWWBB....",
        "...DBWWWWWWBD...",
        "...DBWWWWWWBD...",
        "....BWWWWWWB....",
        "....BBWWWWBB....",
        ".....rrrrrr.....",
        ".....BBBBBB.....",
        "......BBBB......",
    ],
}


def build(rows):
    image = Image.new("RGBA", (len(rows[0]), len(rows)), (0, 0, 0, 0))
    for y, row in enumerate(rows):
        for x, key in enumerate(row):
            if key != ".":
                color = COLORS[key]
                image.putpixel((x, y), ((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255))
    return image.resize((image.width * SCALE, image.height * SCALE), Image.NEAREST)


def write_rgb565a8(image, path):
    data = bytearray(struct.pack("<BBHHHHH", 0x19, 0x14, 0, image.width, image.height, image.width * 2, 0))
    pixels = list(image.getdata())
    for r, g, b, _ in pixels:
        data += struct.pack("<H", ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3))
    data += bytes(a for _, _, _, a in pixels)
    with open(path, "wb") as file:
        file.write(data)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--preview", help="Also save all poses, enlarged, as a PNG")
    args = parser.parse_args()

    images = []
    for name, rows in POSES.items():
        assert len({len(row) for row in rows}) == 1, name
        right = build(rows)
        write_rgb565a8(right, OUT.format(name, "r"))
        write_rgb565a8(right.transpose(Image.FLIP_LEFT_RIGHT), OUT.format(name, "l"))
        images.append(right)

    if args.preview:
        sheet = Image.new("RGBA", (sum(i.width for i in images) + 8 * len(images), images[0].height + 8), (16, 22, 60, 255))
        x = 4
        for image in images:
            sheet.alpha_composite(image, (x, 4))
            x += image.width + 8
        sheet.resize((sheet.width * 4, sheet.height * 4), Image.NEAREST).save(args.preview)


if __name__ == "__main__":
    main()
