# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

import os
import argparse
from littlefs import LittleFS


def create_littlefs_fs_image(
    img_filename,
    img_size,
    block_size,
    read_size,
    prog_size,
    name_max,
    file_max,
    attr_max,
    source_dir,
    disk_version,
):
    block_count = img_size // block_size
    if block_count * block_size != img_size:
        print("image size should be a multiple of block size")
        exit(1)

    if disk_version is None:
        disk_version = 0  # 0 means the latest
    else:
        # "2.1" -> 0x00020001
        try:
            major, minor = disk_version.split(".")
            disk_version = int(major) * 0x10000 + int(minor)
        except:
            print(f"failed to parse disk version: {disk_version}")
            exit(1)

    fs = LittleFS(
        block_size=block_size,
        block_count=block_count,
        read_size=read_size,
        prog_size=prog_size,
        name_max=name_max,
        file_max=file_max,
        attr_max=attr_max,
        disk_version=disk_version,
    )

    # Note: path component separator etc are assumed to be compatible
    # between littlefs and host.
    for root, dirs, files in os.walk(source_dir):
        print(f"root {root} dirs {dirs} files {files}")
        for dir in dirs:
            path = os.path.join(root, dir)
            relpath = os.path.relpath(path, start=source_dir)
            print(f"Mkdir {relpath}")
            fs.mkdir(relpath)
        for f in files:
            path = os.path.join(root, f)
            relpath = os.path.relpath(path, start=source_dir)
            print(f"Copying {path} to {relpath}")
            with open(path, "rb") as infile:
                with fs.open(relpath, "wb") as outfile:
                    outfile.write(infile.read())

    with open(img_filename, "wb") as f:
        f.write(fs.context.buffer)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--img-filename", default="littlefs.img")
    parser.add_argument("--img-size", type=int, default=2 * 1024 * 1024)
    parser.add_argument("--block-size", type=int, default=4096)
    parser.add_argument("--read-size", type=int, default=256)
    parser.add_argument("--prog-size", type=int, default=256)
    # Note: 0 means to use the build-time default.
    parser.add_argument("--name-max", type=int, default=0)
    parser.add_argument("--file-max", type=int, default=0)
    parser.add_argument("--attr-max", type=int, default=0)
    parser.add_argument("--disk-version", default=None)
    parser.add_argument("source")
    args = parser.parse_args()

    img_filename = args.img_filename
    img_size = args.img_size
    block_size = args.block_size
    read_size = args.read_size
    prog_size = args.prog_size
    name_max = args.name_max
    file_max = args.file_max
    attr_max = args.attr_max
    source_dir = args.source

    create_littlefs_fs_image(
        img_filename,
        img_size,
        block_size,
        read_size,
        prog_size,
        name_max,
        file_max,
        attr_max,
        source_dir,
        args.disk_version,
    )
