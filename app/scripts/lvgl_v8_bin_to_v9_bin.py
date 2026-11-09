# Copyright (c) 2026 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

"""
Convert LVGL 8 .bin images (16 bit color depth) to the LVGL 9 .bin format, in place.

The LVGL 8 images in ZSWatch were made with LV_COLOR_16_SWAP, which LVGL 9 images
do not use, so the two bytes of each color are swapped back unless --no-swap is given.
"""

import argparse
import struct

LV_IMAGE_HEADER_MAGIC = 0x19
V8_CF_TRUE_COLOR = 4
V8_CF_TRUE_COLOR_ALPHA = 5
V9_CF_RGB565 = 0x12
V9_CF_RGB565A8 = 0x14


def swap_color_bytes(color):
    swapped = bytearray(color)
    swapped[0::2] = color[1::2]
    swapped[1::2] = color[0::2]
    return bytes(swapped)


def convert(path, swap):
    with open(path, "rb") as f:
        data = f.read()

    if data[0] == LV_IMAGE_HEADER_MAGIC:
        print(f"{path}: already LVGL 9 format")
        return

    (header,) = struct.unpack_from("<I", data, 0)
    cf = header & 0x1F
    w = (header >> 10) & 0x7FF
    h = (header >> 21) & 0x7FF
    pixels = data[4:]

    if cf == V8_CF_TRUE_COLOR and len(pixels) == w * h * 2:
        new_cf = V9_CF_RGB565
        alpha = b""
    elif cf == V8_CF_TRUE_COLOR_ALPHA and len(pixels) == w * h * 3:
        # LVGL 8 stores color and alpha per pixel, LVGL 9 as a color array followed by an alpha array.
        new_cf = V9_CF_RGB565A8
        color = bytearray()
        for i in range(0, len(pixels), 3):
            color += pixels[i : i + 2]
        alpha = pixels[2::3]
        pixels = bytes(color)
    else:
        print(f"{path}: unsupported format {cf} ({w}x{h}, {len(pixels)} bytes), skipping")
        return

    if swap:
        pixels = swap_color_bytes(pixels)

    with open(path, "wb") as f:
        f.write(struct.pack("<BBHHHHH", LV_IMAGE_HEADER_MAGIC, new_cf, 0, w, h, w * 2, 0))
        f.write(pixels)
        f.write(alpha)
    print(f"{path}: converted {w}x{h}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("files", nargs="+", help=".bin files to convert")
    parser.add_argument("--no-swap", action="store_true", help="Keep the byte order of the colors")
    args = parser.parse_args()

    for path in args.files:
        convert(path, not args.no_swap)
