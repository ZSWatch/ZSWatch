#!/usr/bin/env python3
# Copyright (c) 2026 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0
"""Generates the night sky behind the Sand watchface as an RGB565 image for the resource filesystem.

Run from app/: python scripts/generate_sand_sky.py [--preview sky.png]
"""

import argparse
import random
import struct

from PIL import Image

OUT = "src/images/binaries/S/face_sand_sky.bin"
SIZE = 240
# The sky from the top to the horizon at the bottom: (position 0..1, color)
GRADIENT = [(0.0, (4, 6, 22)), (0.45, (11, 17, 52)), (0.8, (30, 24, 78)), (1.0, (58, 34, 92))]
NUM_STARS = 110
# Rows where the time is: only faint stars there, they would make it harder to read
TIME_ROWS = range(50, 116)
SEED = 7


def sky_color(y):
    position = y / (SIZE - 1)
    for (p0, c0), (p1, c1) in zip(GRADIENT, GRADIENT[1:]):
        if position <= p1:
            t = (position - p0) / (p1 - p0)
            return [c0[i] + (c1[i] - c0[i]) * t for i in range(3)]
    return list(GRADIENT[-1][1])


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--preview", help="Also save the image as a PNG")
    args = parser.parse_args()

    rng = random.Random(SEED)
    pixels = [[sky_color(y) for _ in range(SIZE)] for y in range(SIZE)]

    for _ in range(NUM_STARS):
        x = rng.randrange(SIZE)
        # More stars high up, none in the haze right at the horizon
        y = int(rng.random() ** 1.5 * SIZE * 0.82)
        brightness = rng.choice([70, 90, 110, 150, 200, 255])
        if y in TIME_ROWS:
            brightness = min(brightness, 90)
        tint = rng.choice([(1.0, 1.0, 1.0), (0.8, 0.88, 1.0), (1.0, 0.93, 0.8)])
        star = [brightness * t for t in tint]
        pixels[y][x] = [max(a, b) for a, b in zip(pixels[y][x], star)]
        if brightness == 255:
            # The brightest ones get a faint cross
            for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                if 0 <= x + dx < SIZE and 0 <= y + dy < SIZE:
                    pixels[y + dy][x + dx] = [max(a, b * 0.35) for a, b in zip(pixels[y + dy][x + dx], star)]

    # RGB565 has few dark blues: without noise the gradient shows as bands
    data = bytearray(struct.pack("<BBHHHHH", 0x19, 0x12, 0, SIZE, SIZE, SIZE * 2, 0))
    image = Image.new("RGB", (SIZE, SIZE))
    for y in range(SIZE):
        for x in range(SIZE):
            r, g, b = (min(255.0, max(0.0, c + rng.uniform(-0.5, 0.5) * step))
                       for c, step in zip(pixels[y][x], (8, 4, 8)))
            r5, g6, b5 = round(r) >> 3, round(g) >> 2, round(b) >> 3
            data += struct.pack("<H", (r5 << 11) | (g6 << 5) | b5)
            image.putpixel((x, y), (r5 << 3, g6 << 2, b5 << 3))

    with open(OUT, "wb") as file:
        file.write(data)
    if args.preview:
        image.save(args.preview)


if __name__ == "__main__":
    main()
