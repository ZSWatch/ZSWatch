#!/usr/bin/env python3
# Copyright (c) 2026 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0
"""Generates the skier of the Sand watchface, facing right and left, as RGB565A8 images.

Run from app/: python scripts/generate_sand_skier.py [--preview skier.png]
"""

import argparse
import struct

from PIL import Image

OUT = "src/images/binaries/S/face_sand_skier_{}.bin"
# Display pixels per pixel of the drawing: the same as a grain of the watchface
SCALE = 2

COLORS = {
    "b": 0x2DD4BF,  # Beanie
    "s": 0xF2C6A0,  # Face
    "g": 0x1B2238,  # Goggles
    "r": 0xFF453A,  # Jacket
    "c": 0xFFD60A,  # Scarf, flying behind
    "n": 0x27408B,  # Trousers
    "p": 0x9AA3B5,  # Pole
    "k": 0xFF9500,  # Skis
}

# Facing right, leaning into the slope. The bottom row is what touches the snow.
SKIER = [
    "......bbb.......",
    ".....bbbbb......",
    ".....ssssg......",
    "......ssss......",
    ".cc.rrrrr.......",
    "c.ccrrrrrr......",
    "....rrrrrrp.....",
    "....rrrrr.p.....",
    "....nnnnn..p....",
    "...nnn.nnn.p....",
    "...nn...nn..p...",
    "..nnn...nnn.p...",
    "..............k.",
    "kkkkkkkkkkkkkkk.",
]


def build(rows):
    image = Image.new("RGBA", (len(rows[0]), len(rows)), (0, 0, 0, 0))
    for y, row in enumerate(rows):
        for x, key in enumerate(row):
            if key != ".":
                color = COLORS[key]
                image.putpixel((x, y), ((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255))
    return image


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
    parser.add_argument("--preview", help="Also save the skier, enlarged, as a PNG")
    args = parser.parse_args()

    right = build(SKIER)
    right = right.resize((right.width * SCALE, right.height * SCALE), Image.NEAREST)
    left = right.transpose(Image.FLIP_LEFT_RIGHT)
    write_rgb565a8(right, OUT.format("right"))
    write_rgb565a8(left, OUT.format("left"))
    if args.preview:
        right.resize((right.width * 6, right.height * 6), Image.NEAREST).save(args.preview)


if __name__ == "__main__":
    main()
