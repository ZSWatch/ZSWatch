#!/bin/bash
# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0
#
# Package a ZSWatch-specific EDK from the Zephyr EDK tarball.
#
# Usage:
#   ./package_edk.sh <zephyr-edk-tarball> <fw-version> [output-dir]
#
# Example:
#   west build -t llext-edk --build-dir app/build_dbg_dk
#   ./app/scripts/package_edk.sh app/build_dbg_dk/zephyr/llext-edk.tar.xz 0.29.0
#
# Produces: zswatch-edk-<fw-version>.tar.xz

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

if [ $# -lt 2 ]; then
    echo "Usage: $0 <zephyr-edk-tarball> <fw-version> [output-dir]"
    exit 1
fi

ZEPHYR_EDK_TARBALL="$1"
FW_VERSION="$2"
OUTPUT_DIR="${3:-$(pwd)}"

if [ ! -f "$ZEPHYR_EDK_TARBALL" ]; then
    echo "Error: Zephyr EDK tarball not found: $ZEPHYR_EDK_TARBALL"
    exit 1
fi

# Create temporary working directory
WORK_DIR=$(mktemp -d)
trap "rm -rf $WORK_DIR" EXIT

EDK_NAME="zswatch-edk-${FW_VERSION}"
EDK_DIR="$WORK_DIR/$EDK_NAME"

echo "==> Extracting Zephyr EDK..."
mkdir -p "$EDK_DIR"
tar -xf "$ZEPHYR_EDK_TARBALL" -C "$EDK_DIR" --strip-components=1

echo "==> Patching cmake.cflags..."
# Remove -DLV_CONF_PATH=... from cflags — its absolute path doesn't work
# outside the original build tree. LV_CONF_INCLUDE_SIMPLE=1 is already
# defined and lv_conf.h is in the EDK include path, so it's not needed.
sed -i 's/-DLV_CONF_PATH=[^;]*;//g' "$EDK_DIR/cmake.cflags"
# Also fix Makefile.cflags
sed -i 's/-DLV_CONF_PATH=[^ ]* //g' "$EDK_DIR/Makefile.cflags"

echo "==> Adding ZSWatch headers..."
# Copy ZSWatch-specific headers into the EDK include tree
ZSW_INC="$EDK_DIR/include/zswatch"
mkdir -p "$ZSW_INC/managers"
mkdir -p "$ZSW_INC/llext"
mkdir -p "$ZSW_INC/events"
mkdir -p "$ZSW_INC/ui/utils"
mkdir -p "$ZSW_INC/history"
mkdir -p "$ZSW_INC/filesystem"
mkdir -p "$ZSW_INC/sensor_fusion"
mkdir -p "$ZSW_INC/ble"

# App manager
cp "$APP_DIR/src/managers/zsw_app_manager.h" "$ZSW_INC/managers/"

# LLEXT support headers
cp "$APP_DIR/src/llext/zsw_llext_iflash.h" "$ZSW_INC/llext/"
cp "$APP_DIR/src/llext/zsw_llext_log.h" "$ZSW_INC/llext/"

# Event headers
for f in "$APP_DIR/src/events/"*.h; do
    [ -f "$f" ] && cp "$f" "$ZSW_INC/events/"
done

# UI utils — use a stripped-down version for EDK (no notification manager dependency)
cat > "$ZSW_INC/ui/utils/zsw_ui_utils.h" << 'HEADEREOF'
/*
 * ZSWatch EDK — minimal zsw_ui_utils.h for LLEXT apps.
 * Only the image macros needed by apps are included here.
 */
#pragma once

#include <lvgl.h>

/* LLEXT apps always run on hardware with external flash images */
#define ZSW_LV_IMG_DECLARE(var_name)
#define ZSW_LV_IMG_USE(var_name)                            "S:"#var_name".bin"
#define ZSW_LV_IMG_USE_WITH_MOUNT(var_name, mount_letter)   mount_letter "/" #var_name ".bin"
HEADEREOF

# History
if [ -f "$APP_DIR/src/history/zsw_history.h" ]; then
    cp "$APP_DIR/src/history/zsw_history.h" "$ZSW_INC/history/"
fi

# Filesystem
if [ -f "$APP_DIR/src/filesystem/zsw_filesystem.h" ]; then
    cp "$APP_DIR/src/filesystem/zsw_filesystem.h" "$ZSW_INC/filesystem/"
fi

# Sensor fusion
for f in "$APP_DIR/src/sensor_fusion/"*.h; do
    [ -f "$f" ] && cp "$f" "$ZSW_INC/sensor_fusion/"
done

# BLE HTTP
if [ -f "$APP_DIR/src/ble/ble_http.h" ]; then
    cp "$APP_DIR/src/ble/ble_http.h" "$ZSW_INC/ble/"
fi

echo "==> Fixing LVGL include tree..."
# The Zephyr EDK packages lvgl/src/ headers, but src/lvgl.h does
# #include "../lvgl.h" — so we need the parent-level lvgl headers too.
LVGL_SRC=$(find "$EDK_DIR/include" -type d -path "*/gui/lvgl/src" | head -1)
if [ -n "$LVGL_SRC" ]; then
    LVGL_ROOT=$(dirname "$LVGL_SRC")
    REAL_LVGL_ROOT="${APP_DIR}/../modules/lib/gui/lvgl"
    for f in "$REAL_LVGL_ROOT"/*.h; do
        [ -f "$f" ] && cp "$f" "$LVGL_ROOT/" && echo "    Copied $(basename $f) → lvgl/"
    done
fi

echo "==> Adding ZSWatch CMake wrapper..."
# Copy the standalone build files
cp "$SCRIPT_DIR/edk/CMakeLists.txt" "$EDK_DIR/CMakeLists.txt.template"
cp "$SCRIPT_DIR/edk/toolchain.cmake" "$EDK_DIR/"

echo "==> Adding app template..."
mkdir -p "$EDK_DIR/template"
cp "$SCRIPT_DIR/edk/template/my_app.c" "$EDK_DIR/template/"
cp "$SCRIPT_DIR/edk/template/CMakeLists.txt" "$EDK_DIR/template/"

echo "==> Writing VERSION file..."
echo "$FW_VERSION" > "$EDK_DIR/VERSION"

echo "==> Writing ZSWatch-specific cflags..."
# Generate a zswatch_extra.cmake that adds ZSWatch-specific flags
cat > "$EDK_DIR/zswatch_extra.cmake" << 'CMAKEOF'
# ZSWatch-specific compile/link flags for LLEXT apps
# Include this AFTER the Zephyr EDK cmake.cflags

# XIP flash is far from firmware .text — need indirect calls via GOT
list(APPEND LLEXT_CFLAGS "-mlong-calls")

# ZSWatch headers
list(APPEND LLEXT_CFLAGS "-I${CMAKE_CURRENT_LIST_DIR}/include/zswatch")
list(APPEND LLEXT_CFLAGS "-I${CMAKE_CURRENT_LIST_DIR}/include/zswatch/managers")
list(APPEND LLEXT_CFLAGS "-I${CMAKE_CURRENT_LIST_DIR}/include/zswatch/llext")

# Keep .text.iflash as a separate section for internal flash copy
set(LLEXT_EXTRA_LINK_FLAGS "-Wl,--unique=.text.iflash")
CMAKEOF

echo "==> Packaging ${EDK_NAME}.tar.xz..."
mkdir -p "$OUTPUT_DIR"
tar -cJf "$OUTPUT_DIR/${EDK_NAME}.tar.xz" -C "$WORK_DIR" "$EDK_NAME"

echo "==> Done: $OUTPUT_DIR/${EDK_NAME}.tar.xz"
echo "    Extract and set LLEXT_EDK_INSTALL_DIR to use."
