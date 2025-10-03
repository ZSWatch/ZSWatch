# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

import os
import argparse
from struct import *

MAX_FILE_NAME = 32
FILE_TABLE_MAX_LEN = 32000
TABLE_MAGIC = 0x0A0A0A0A
"""
magic_number:uint32
header_len:uint32
total_length:uint32
num_files:uint32
filename[MAX_FILE_NAME]
offset:uint32
len:uint32
filename[MAX_FILE_NAME]
offset:uint32
len:uint32
...
[file data]
table_magic:uint32
"""


def create_custom_raw_fs_image(img_filename, source_dir, block_size=4096):
    table = {}
    offset = 0
    files_image = bytearray()
    header_images = bytearray()
    for root, dirs, files in os.walk(source_dir):
        print(f"root {root} dirs {dirs} files {files}")
        for filename in files:
            path = os.path.join(root, filename)
            relpath = os.path.relpath(path, start=source_dir)
            print(f"Adding {path}")
            with open(path, "rb") as infile:
                files_image.extend(infile.read())
                table[filename] = {"offset": offset, "len": infile.tell()}
                offset = offset + infile.tell()
    print(table)
    for name, data in table.items():
        if len(name) <= MAX_FILE_NAME:
            header_images = header_images + pack(
                f"<{MAX_FILE_NAME}sII",
                bytes(name, "utf-8"),
                data["offset"],
                data["len"],
            )
        else:
            print("Filename to long, skipping", name, len(name))

    # Insert dummy values as header length and total length so we can get the size of the header
    fake_header = header_image = (
        pack("<IIII", TABLE_MAGIC, len(table), 0, 0) + header_images
    )
    print("header len", len(fake_header))

    trailer_size = 4  # Size of trailer magic number
    real_header = (
        pack(
            "<IIII",
            TABLE_MAGIC,
            len(fake_header),
            len(fake_header) + len(files_image) + trailer_size,
            len(table),
        )
        + header_images
    )

    if len(header_images) > FILE_TABLE_MAX_LEN:
        print(
            "File table is to big, increase the size on target size",
            FILE_TABLE_MAX_LEN,
            "<",
            len(header_images),
        )
        exit(1)

    print(f"Creating Raw FS image: {len(table)} files, {len(files_image)} bytes")

    with open(img_filename, "wb") as f:
        f.write(real_header)
        f.write(files_image)
        f.write(pack("<I", TABLE_MAGIC))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--img-filename", default="littlefs.img")
    parser.add_argument("--block-size", type=int, default=4096)
    parser.add_argument("source")
    args = parser.parse_args()

    img_filename = args.img_filename
    block_size = args.block_size
    source_dir = args.source

    create_custom_raw_fs_image(img_filename, source_dir, block_size)
