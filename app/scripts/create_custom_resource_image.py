# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

import os
import argparse
from struct import pack

MAX_FILE_NAME = 32
FILE_TABLE_MAX_LEN = 32000
TABLE_MAGIC = 0x0A0A0A0A
INDEX_MAGIC = 0x5844495A  # "ZIDX"
FILE_ALIGNMENT = 4
INDEX_SLOT_SIZE = 12
INDEX_FOOTER_SIZE = 16
TRAILER_SIZE = 4
"""
All values little endian.

magic_number:uint32
header_len:uint32               Length of everything before [file data]
total_length:uint32
num_files:uint32
filename[MAX_FILE_NAME]         Name table, file offsets are relative to header_len
offset:uint32
len:uint32
...
[file data]                     Each file starts on a FILE_ALIGNMENT boundary
[hash index]                    Open addressing hash table with linear probing
    hash:uint32                 FNV-1a of the filename, starting from seed
    offset:uint32               Counted from the start of the image, 0 marks a free slot
    len:uint32
    ...
index_magic:uint32              Index footer
index_offset:uint32             Counted from the start of the image
index_num_slots:uint32          Power of two
index_seed:uint32
table_magic:uint32

Firmware without hash index support only looks at the header, the name table and
the trailing table_magic, so it can still read the image. Firmware with support
finds the index through the footer and does not need the name table.
"""


def fnv1a(data, seed):
    h = seed
    for b in data:
        h = ((h ^ b) * 0x01000193) & 0xFFFFFFFF
    return h


def align_up(value, alignment):
    return (value + alignment - 1) // alignment * alignment


def build_hash_index(files, data_start):
    num_slots = 4
    while num_slots < 2 * len(files):
        num_slots *= 2

    # The firmware trusts a matching hash without comparing names, so every
    # file needs a unique hash. Pick another seed in the unlikely case of a clash.
    seed = 0x811C9DC5
    while True:
        hashes = [fnv1a(name.encode("utf-8"), seed) for name, _, _ in files]
        if len(set(hashes)) == len(hashes):
            break
        seed = (seed + 1) & 0xFFFFFFFF

    slots = [None] * num_slots
    for file_hash, (_, offset, length) in zip(hashes, files):
        i = file_hash & (num_slots - 1)
        while slots[i] is not None:
            i = (i + 1) & (num_slots - 1)
        slots[i] = (file_hash, data_start + offset, length)

    index = bytearray()
    for slot in slots:
        index += pack("<III", *(slot if slot else (0, 0, 0)))

    return index, num_slots, seed


def create_custom_raw_fs_image(img_filename, source_dir, block_size=4096, legacy=False, verbose=False):
    files = []
    files_image = bytearray()
    seen = set()
    for root, dirs, filenames in sorted(os.walk(source_dir)):
        for filename in sorted(filenames):
            path = os.path.join(root, filename)
            if filename in seen:
                print("Duplicate filename, skipping", path)
                continue
            seen.add(filename)
            if verbose:
                print(f"Adding {path}")
            with open(path, "rb") as infile:
                data = infile.read()
            if not legacy:
                files_image.extend(b"\x00" * (align_up(len(files_image), FILE_ALIGNMENT) - len(files_image)))
            files.append((filename, len(files_image), len(data)))
            files_image.extend(data)

    name_table = bytearray()
    num_named_files = 0
    for name, offset, length in files:
        encoded_name = name.encode("utf-8")
        if len(encoded_name) > MAX_FILE_NAME:
            # Such a file is still reachable through the hash index.
            print("Filename too long for the name table:", name, len(encoded_name))
            if legacy:
                continue
        elif len(name_table) + MAX_FILE_NAME + 8 > FILE_TABLE_MAX_LEN:
            if legacy:
                print("File table is too big, increase the size on target side", FILE_TABLE_MAX_LEN)
                exit(1)
            # Firmware without hash index support loads at most this much of the table.
            print("Name table full, only reachable with hash index:", name)
        else:
            name_table += pack(f"<{MAX_FILE_NAME}sII", encoded_name, offset, length)
            num_named_files += 1

    header_len = 16 + len(name_table)
    index = bytearray()
    footer = bytearray()
    if not legacy:
        files_image.extend(b"\x00" * (align_up(len(files_image), FILE_ALIGNMENT) - len(files_image)))
        index, num_slots, seed = build_hash_index(files, header_len)
        footer = pack("<IIII", INDEX_MAGIC, header_len + len(files_image), num_slots, seed)

    # Pad so that the total size is a multiple of 4
    current_size = header_len + len(files_image) + len(index) + len(footer) + TRAILER_SIZE
    padding_needed = (4 - (current_size % 4)) % 4
    total_length = current_size + padding_needed

    header = pack("<IIII", TABLE_MAGIC, header_len, total_length, num_named_files)

    print(
        f"Creating Raw FS image: {len(files)} files, {len(files_image)} bytes of data, "
        f"{total_length} bytes total{' (legacy format)' if legacy else ''}"
    )

    with open(img_filename, "wb") as f:
        f.write(header)
        f.write(name_table)
        f.write(files_image)
        f.write(b"\x00" * padding_needed)
        f.write(index)
        f.write(footer)
        f.write(pack("<I", TABLE_MAGIC))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--img-filename", default="littlefs.img")
    parser.add_argument("--block-size", type=int, default=4096)
    parser.add_argument(
        "--legacy",
        action="store_true",
        help="Create the old format without file alignment and hash index, for testing.",
    )
    parser.add_argument("--verbose", action="store_true")
    parser.add_argument("source")
    args = parser.parse_args()

    create_custom_raw_fs_image(args.img_filename, args.source, args.block_size, args.legacy, args.verbose)
