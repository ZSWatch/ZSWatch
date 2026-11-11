#!/usr/bin/env bash
# Generates the fonts in src/images/fonts that LVGL does not have built in.
# Needs lv_font_conv (npm i -g lv_font_conv). Run from the app directory.
set -euo pipefail

FONTS=lvgl_editor/fonts
OUT=src/images/fonts

# Large numbers only: time and measurement values.
lv_font_conv --no-compress --no-prefilter --bpp 4 --size 48 \
    --font "$FONTS/Montserrat-Medium.ttf" -r "0x20,0x25,0x2B-0x3A,176" \
    --format lvgl --force-fast-kern-format --lv-font-name zsw_font_48 -o "$OUT/zsw_font_48.c"
