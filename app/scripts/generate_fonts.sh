#!/usr/bin/env bash
# Generates the UI fonts in src/images/fonts from the TTFs in lvgl_editor/fonts.
# Needs lv_font_conv (npm i -g lv_font_conv). Run from the app directory.
#
# zsw_font_14 is the one to use for text that comes from the phone (notifications,
# music info...): it also has Latin-1 and Hebrew. The other sizes are ASCII only.
set -euo pipefail

FONTS=lvgl_editor/fonts
OUT=src/images/fonts
REGULAR=$FONTS/Rubik-Regular.ttf
MEDIUM=$FONTS/Rubik-Medium.ttf
AWESOME=$FONTS/FontAwesome5-Solid+Brands+Regular.woff
HEEBO=$FONTS/Heebo-Medium.ttf

# The LV_SYMBOL_* glyphs, same set as the LVGL built-in fonts.
SYMBOLS=61441,61448,61451,61452,61453,61457,61459,61461,61465,61468,61473,61478,61479,61480,61502,61507,61512,61515,61516,61517,61521,61522,61523,61524,61543,61544,61550,61552,61553,61556,61559,61560,61561,61563,61587,61589,61636,61637,61639,61641,61664,61671,61674,61683,61724,61732,61787,61931,62016,62017,62018,62019,62020,62087,62099,62212,62189,62810,63426,63650

ASCII=0x20-0x7E,176,8226

gen() {
    local name=$1 size=$2 ttf=$3 range=$4
    shift 4
    lv_font_conv --no-compress --no-prefilter --bpp 4 --size "$size" \
        --font "$ttf" -r "$range" "$@" \
        --format lvgl --force-fast-kern-format --lv-font-name "$name" -o "$OUT/$name.c"
}

gen zsw_font_12 12 "$REGULAR" "$ASCII" --font "$AWESOME" -r "$SYMBOLS"
gen zsw_font_14 14 "$REGULAR" "$ASCII,0xA1-0xFF,1488-1514" \
    --font "$AWESOME" -r "$SYMBOLS" --font "$HEEBO" -r 1520-1524
gen zsw_font_16 16 "$REGULAR" "$ASCII" --font "$AWESOME" -r "$SYMBOLS"
gen zsw_font_20 20 "$MEDIUM" "$ASCII" --font "$AWESOME" -r "$SYMBOLS"
gen zsw_font_28 28 "$MEDIUM" "$ASCII"
# Large numbers only: time and measurement values.
gen zsw_font_48 48 "$MEDIUM" "0x20,0x25,0x2B-0x3A,176"

# Emoji in phone text come from the fallback font.
sed -i -e 's|^/\*Initialize a public general font descriptor\*/|extern const lv_font_t noto_emoji_14_light;\n\n&|' \
    -e 's|^    \.fallback = NULL,|    .fallback = \&noto_emoji_14_light,|' "$OUT/zsw_font_14.c"
