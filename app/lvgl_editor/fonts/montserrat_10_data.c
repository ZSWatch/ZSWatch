/*******************************************************************************
 * Size: 10 px
 * Bpp: 2
 * Opts: --font /fonts/Montserrat-Medium.ttf -o /fonts/montserrat_10_data.c --no-compress --size 10 --bpp 2 --format lvgl --range 0x00-0xff
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef MONTSERRAT_10_DATA
#define MONTSERRAT_10_DATA 1
#endif

#if MONTSERRAT_10_DATA

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0000 "\u0000" */

    /* U+000D "\r" */

    /* U+0020 " " */

    /* U+0021 "!" */
    0x33, 0x33, 0x10, 0x30,

    /* U+0022 "\"" */
    0x59, 0x58, 0x4,

    /* U+0023 "#" */
    0x8, 0x20, 0x21, 0x46, 0xef, 0x82, 0x20, 0xbe,
    0xe4, 0x52, 0x2, 0x8, 0x0,

    /* U+0024 "$" */
    0x0, 0x2, 0xfc, 0x61, 0x7, 0x50, 0x1f, 0x80,
    0x2a, 0x1, 0x63, 0xf8, 0x1, 0x0,

    /* U+0025 "%" */
    0x28, 0x8, 0x20, 0x88, 0x8, 0x25, 0x0, 0xa6,
    0x64, 0x2, 0x22, 0x2, 0x8, 0x80, 0x82, 0x90,

    /* U+0026 "&" */
    0xe, 0x40, 0x93, 0x1, 0xa4, 0x7, 0xc0, 0x61,
    0xa1, 0x82, 0xc2, 0xf9, 0x40, 0x0,

    /* U+0027 "'" */
    0x55, 0x0,

    /* U+0028 "(" */
    0xc, 0x24, 0x30, 0x30, 0x30, 0x30, 0x30, 0x24,
    0xc,

    /* U+0029 ")" */
    0x60, 0xc2, 0x9, 0x24, 0x92, 0xc, 0x60,

    /* U+002A "*" */
    0x14, 0x7d, 0x69, 0x0,

    /* U+002B "+" */
    0x0, 0x0, 0x90, 0x2f, 0x80, 0x90, 0x9, 0x0,

    /* U+002C "," */
    0x16, 0x50,

    /* U+002D "-" */
    0x7c,

    /* U+002E "." */
    0x6,

    /* U+002F "/" */
    0x0, 0x80, 0x90, 0x30, 0x18, 0x8, 0x3, 0x1,
    0x80, 0x80, 0x30, 0x0,

    /* U+0030 "0" */
    0x1f, 0x80, 0xc1, 0x86, 0x3, 0x18, 0xc, 0x60,
    0x30, 0xc1, 0x81, 0xf8, 0x0,

    /* U+0031 "1" */
    0xb8, 0x61, 0x86, 0x18, 0x61, 0x80,

    /* U+0032 "2" */
    0x7f, 0x44, 0xc, 0x0, 0xc0, 0x28, 0xa, 0x2,
    0x80, 0xbf, 0xd0,

    /* U+0033 "3" */
    0xbf, 0xc0, 0x34, 0xa, 0x0, 0xb8, 0x0, 0xc4,
    0xc, 0x7f, 0x40,

    /* U+0034 "4" */
    0x1, 0x80, 0x1c, 0x0, 0xc0, 0xc, 0x20, 0xbf,
    0xf4, 0x2, 0x0, 0x8, 0x0,

    /* U+0035 "5" */
    0x3f, 0xc3, 0x0, 0x30, 0x3, 0xf4, 0x0, 0xd0,
    0xd, 0x7f, 0x80,

    /* U+0036 "6" */
    0xb, 0xd3, 0x40, 0x60, 0x6, 0xa8, 0x70, 0x73,
    0x3, 0x1f, 0x80,

    /* U+0037 "7" */
    0xbf, 0xe9, 0xc, 0x1, 0xc0, 0x34, 0x3, 0x0,
    0x90, 0xc, 0x0,

    /* U+0038 "8" */
    0x1f, 0x83, 0x6, 0x30, 0x62, 0xfc, 0x60, 0x76,
    0x3, 0x2e, 0xd0,

    /* U+0039 "9" */
    0x2f, 0x49, 0x8, 0x90, 0xe2, 0xaa, 0x0, 0xa0,
    0xc, 0x3f, 0x40,

    /* U+003A ":" */
    0x60, 0x0, 0x60,

    /* U+003B ";" */
    0x60, 0x0, 0x62, 0x0,

    /* U+003C "<" */
    0x0, 0x0, 0x68, 0x34, 0x1, 0xa0, 0x0, 0x80,

    /* U+003D "=" */
    0x2a, 0x80, 0x0, 0x2a, 0x80,

    /* U+003E ">" */
    0x0, 0x2, 0x90, 0x2, 0xc1, 0xa4, 0x10, 0x0,

    /* U+003F "?" */
    0x3f, 0x44, 0xc, 0x0, 0xc0, 0x30, 0x9, 0x0,
    0x0, 0x9, 0x0,

    /* U+0040 "@" */
    0x6, 0xa9, 0x1, 0x80, 0x8, 0x21, 0xea, 0x55,
    0x60, 0xa2, 0x45, 0x2, 0x25, 0x20, 0xa2, 0x21,
    0xe7, 0x81, 0x80, 0x0, 0x6, 0xa8, 0x0,

    /* U+0041 "A" */
    0x0, 0xd0, 0x0, 0xac, 0x0, 0x32, 0x40, 0x14,
    0x20, 0xf, 0xfc, 0x6, 0x1, 0x83, 0x0, 0x30,

    /* U+0042 "B" */
    0xff, 0x83, 0x1, 0x8c, 0xa, 0x3f, 0xf0, 0xc0,
    0x33, 0x0, 0xcf, 0xfd, 0x0,

    /* U+0043 "C" */
    0xb, 0xf0, 0xd0, 0x16, 0x0, 0x18, 0x0, 0x60,
    0x0, 0xd0, 0x10, 0xbf, 0x0,

    /* U+0044 "D" */
    0xff, 0x83, 0x0, 0xcc, 0x1, 0xb0, 0x6, 0xc0,
    0x1b, 0x0, 0xcf, 0xf8, 0x0,

    /* U+0045 "E" */
    0xff, 0xcc, 0x0, 0xc0, 0xf, 0xf4, 0xc0, 0xc,
    0x0, 0xff, 0xc0,

    /* U+0046 "F" */
    0xff, 0xf0, 0xc, 0x3, 0xfd, 0xc0, 0x30, 0xc,
    0x0,

    /* U+0047 "G" */
    0xb, 0xf4, 0xd0, 0x16, 0x0, 0x18, 0x1, 0x60,
    0xc, 0xd0, 0x30, 0xbf, 0x40,

    /* U+0048 "H" */
    0xc0, 0x33, 0x0, 0xcc, 0x3, 0x3f, 0xfc, 0xc0,
    0x33, 0x0, 0xcc, 0x3, 0x0,

    /* U+0049 "I" */
    0xcc, 0xcc, 0xcc, 0xc0,

    /* U+004A "J" */
    0x1f, 0xc0, 0xc, 0x0, 0xc0, 0xc, 0x0, 0xc1,
    0xc, 0x2f, 0x40,

    /* U+004B "K" */
    0xc0, 0xa3, 0xa, 0xc, 0xa0, 0x3b, 0x40, 0xe7,
    0x3, 0x7, 0xc, 0x6, 0x0,

    /* U+004C "L" */
    0xc0, 0x30, 0xc, 0x3, 0x0, 0xc0, 0x30, 0xf,
    0xf8,

    /* U+004D "M" */
    0xc0, 0x9, 0xf0, 0xd, 0xe4, 0x39, 0xcc, 0x65,
    0xc6, 0xc5, 0xc3, 0x45, 0xc0, 0x5,

    /* U+004E "N" */
    0xd0, 0x33, 0xc0, 0xcd, 0xc3, 0x32, 0x8c, 0xc2,
    0x73, 0x3, 0xcc, 0x3, 0x0,

    /* U+004F "O" */
    0xb, 0xf4, 0x34, 0xd, 0x60, 0x6, 0x60, 0x3,
    0x60, 0x6, 0x34, 0xd, 0xb, 0xf4,

    /* U+0050 "P" */
    0xff, 0x8c, 0x9, 0xc0, 0x6c, 0x9, 0xff, 0x4c,
    0x0, 0xc0, 0x0,

    /* U+0051 "Q" */
    0xb, 0xf4, 0xd, 0x3, 0x46, 0x0, 0x61, 0x80,
    0xc, 0x60, 0x7, 0xd, 0x3, 0x40, 0xbf, 0x40,
    0x1, 0xa8,

    /* U+0052 "R" */
    0xff, 0x8c, 0x9, 0xc0, 0x6c, 0x9, 0xff, 0x8c,
    0x18, 0xc0, 0x90,

    /* U+0053 "S" */
    0x2f, 0xc6, 0x0, 0x70, 0x1, 0xf8, 0x0, 0xa0,
    0x6, 0x2f, 0x80,

    /* U+0054 "T" */
    0xff, 0xe0, 0x90, 0x9, 0x0, 0x90, 0x9, 0x0,
    0x90, 0x9, 0x0,

    /* U+0055 "U" */
    0x30, 0xc, 0xc0, 0x33, 0x0, 0xcc, 0x3, 0x30,
    0xc, 0x90, 0xa0, 0xbe, 0x0,

    /* U+0056 "V" */
    0x34, 0x3, 0x6, 0x2, 0x40, 0xc0, 0xc0, 0x18,
    0x60, 0x3, 0x30, 0x0, 0xb8, 0x0, 0xd, 0x0,

    /* U+0057 "W" */
    0xa0, 0x34, 0xc, 0xc1, 0xe0, 0x93, 0x9, 0xc3,
    0xa, 0x32, 0x4c, 0xd, 0x86, 0x90, 0x3c, 0xf,
    0x0, 0xb0, 0x2c, 0x0,

    /* U+0058 "X" */
    0x70, 0x30, 0xa2, 0x40, 0xe8, 0x1, 0xc0, 0xd,
    0xc0, 0xd2, 0x8a, 0x3, 0x0,

    /* U+0059 "Y" */
    0x24, 0xc, 0xc, 0x18, 0xa, 0x30, 0x3, 0x90,
    0x1, 0xc0, 0x1, 0x80, 0x1, 0x80,

    /* U+005A "Z" */
    0x7f, 0xf0, 0x2, 0x40, 0x28, 0x1, 0xc0, 0xc,
    0x0, 0xd0, 0xb, 0xff, 0x0,

    /* U+005B "[" */
    0xe3, 0xc, 0x30, 0xc3, 0xc, 0x30, 0xe0,

    /* U+005C "\\" */
    0x20, 0xc, 0x1, 0x40, 0x30, 0x8, 0x1, 0x80,
    0x30, 0x8, 0x1, 0x80,

    /* U+005D "]" */
    0xb4, 0x92, 0x49, 0x24, 0x92, 0x49, 0xb4,

    /* U+005E "^" */
    0xa, 0x0, 0xa0, 0x21, 0x42, 0x8,

    /* U+005F "_" */
    0xaa, 0x80,

    /* U+0060 "`" */
    0x20,

    /* U+0061 "a" */
    0x2f, 0x40, 0xc, 0x2a, 0xc6, 0xc, 0x3a, 0xc0,

    /* U+0062 "b" */
    0x30, 0x0, 0xc0, 0x3, 0xbd, 0xc, 0xc, 0x30,
    0x24, 0xd0, 0xc3, 0xbd, 0x0,

    /* U+0063 "c" */
    0x1f, 0x87, 0x4, 0x90, 0x7, 0x4, 0x1f, 0x80,

    /* U+0064 "d" */
    0x0, 0x30, 0x3, 0x2f, 0xb7, 0x7, 0x90, 0x36,
    0x7, 0x2e, 0xb0,

    /* U+0065 "e" */
    0x2f, 0x86, 0x9, 0xba, 0xa6, 0x0, 0x1f, 0x80,

    /* U+0066 "f" */
    0x1e, 0x30, 0xb9, 0x30, 0x30, 0x30, 0x30,

    /* U+0067 "g" */
    0x2f, 0xb6, 0x7, 0x90, 0x37, 0x7, 0x1f, 0xb0,
    0x7, 0x2f, 0x80,

    /* U+0068 "h" */
    0x30, 0x3, 0x0, 0x3b, 0xd3, 0x3, 0x30, 0x33,
    0x3, 0x30, 0x30,

    /* U+0069 "i" */
    0x30, 0x3, 0xc, 0x30, 0xc3, 0x0,

    /* U+006A "j" */
    0xc, 0x0, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc,
    0xb4,

    /* U+006B "k" */
    0x30, 0x0, 0xc0, 0x3, 0x9, 0xc, 0xd0, 0x3f,
    0x0, 0xc7, 0x3, 0x6, 0x0,

    /* U+006C "l" */
    0x33, 0x33, 0x33, 0x30,

    /* U+006D "m" */
    0x3a, 0xda, 0xc3, 0x7, 0x6, 0x30, 0x30, 0x63,
    0x3, 0x6, 0x30, 0x30, 0x60,

    /* U+006E "n" */
    0x3a, 0xd3, 0x3, 0x30, 0x33, 0x3, 0x30, 0x30,

    /* U+006F "o" */
    0x1f, 0x87, 0x6, 0x90, 0x37, 0x6, 0x1f, 0x80,

    /* U+0070 "p" */
    0x3a, 0xd0, 0xc0, 0xc3, 0x2, 0x4d, 0xc, 0x3b,
    0xd0, 0xc0, 0x3, 0x0, 0x0,

    /* U+0071 "q" */
    0x2f, 0xb7, 0x7, 0x90, 0x37, 0x7, 0x2f, 0xb0,
    0x3, 0x0, 0x30,

    /* U+0072 "r" */
    0x3a, 0x30, 0x30, 0x30, 0x30,

    /* U+0073 "s" */
    0x3f, 0x68, 0x2, 0xe0, 0x6, 0x7f, 0x0,

    /* U+0074 "t" */
    0x30, 0xb9, 0x30, 0x30, 0x30, 0x1e,

    /* U+0075 "u" */
    0x30, 0x33, 0x3, 0x30, 0x33, 0x7, 0x1e, 0xb0,

    /* U+0076 "v" */
    0x30, 0x24, 0x60, 0xc0, 0xc9, 0x1, 0xb0, 0x3,
    0x80,

    /* U+0077 "w" */
    0x80, 0xc0, 0x98, 0x68, 0x93, 0x33, 0x30, 0xa8,
    0xa8, 0xc, 0xc, 0x0,

    /* U+0078 "x" */
    0x60, 0xc2, 0xb0, 0xd, 0x2, 0x70, 0x60, 0xc0,

    /* U+0079 "y" */
    0x30, 0x24, 0x60, 0xc0, 0xc5, 0x1, 0xb0, 0x3,
    0x80, 0xc, 0x3, 0xd0, 0x0,

    /* U+007A "z" */
    0x6b, 0x80, 0xc0, 0xc0, 0xd0, 0xba, 0x80,

    /* U+007B "{" */
    0x1c, 0x24, 0x24, 0x24, 0x70, 0x24, 0x24, 0x24,
    0x1c,

    /* U+007C "|" */
    0xff, 0xff, 0xc0,

    /* U+007D "}" */
    0xa0, 0xc3, 0xc, 0x2c, 0xc3, 0xc, 0xa0,

    /* U+007E "~" */
    0x28, 0x45, 0x28,

    /* U+00A0 " " */

    /* U+00A1 "¡" */
    0x30, 0x23, 0x33, 0x10,

    /* U+00A2 "¢" */
    0x0, 0x0, 0x10, 0x2f, 0x86, 0x10, 0x91, 0x7,
    0x14, 0x1f, 0x80, 0x10,

    /* U+00A3 "£" */
    0xb, 0xf0, 0x60, 0x2, 0x40, 0x1e, 0xa0, 0x24,
    0x0, 0x90, 0xb, 0xff, 0x0,

    /* U+00A4 "¤" */
    0x0, 0x1, 0xae, 0x93, 0x3, 0x8, 0x8, 0x30,
    0x31, 0xae, 0x90, 0x0, 0x0,

    /* U+00A5 "¥" */
    0x24, 0x3, 0x3, 0x3, 0x0, 0x62, 0x40, 0xa,
    0xc0, 0xa, 0xe8, 0x2, 0xba, 0x0, 0xc, 0x0,

    /* U+00A6 "¦" */
    0xfc, 0xf, 0xc0,

    /* U+00A7 "§" */
    0x2a, 0x58, 0x3, 0x92, 0x4a, 0x65, 0x42, 0xc0,
    0x25, 0xa8,

    /* U+00A8 "¨" */
    0x55,

    /* U+00A9 "©" */
    0x5, 0x50, 0x22, 0x98, 0x48, 0x1, 0x4c, 0x1,
    0x48, 0x1, 0x22, 0x98, 0x5, 0x50,

    /* U+00AA "ª" */
    0x28, 0x59, 0x55,

    /* U+00AB "«" */
    0x5, 0x48, 0x86, 0x60, 0x59,

    /* U+00AC "¬" */
    0x3f, 0xc0, 0x8, 0x0, 0x40,

    /* U+00AD "­" */
    0x7c,

    /* U+00AE "®" */
    0x5, 0x50, 0x26, 0x98, 0x45, 0x21, 0x46, 0xd1,
    0x45, 0x11, 0x20, 0x8, 0x5, 0x50,

    /* U+00AF "¯" */
    0x69,

    /* U+00B0 "°" */
    0x18, 0x42, 0x42, 0x18,

    /* U+00B1 "±" */
    0x9, 0x0, 0x90, 0x2f, 0x80, 0x90, 0x0, 0x2,
    0xa8,

    /* U+00B2 "²" */
    0x69, 0x5, 0x18, 0x79,

    /* U+00B3 "³" */
    0x6d, 0xc, 0x2, 0x69,

    /* U+00B4 "´" */
    0x20,

    /* U+00B5 "µ" */
    0x30, 0x33, 0x3, 0x30, 0x33, 0x7, 0x3a, 0xb3,
    0x0, 0x30, 0x0,

    /* U+00B6 "¶" */
    0x7e, 0xdf, 0xc5, 0x7c, 0x50, 0xc5, 0x8, 0x50,
    0x85, 0x8, 0x50, 0x85,

    /* U+00B7 "·" */
    0x3, 0x0,

    /* U+00B8 "¸" */
    0x14, 0x60,

    /* U+00B9 "¹" */
    0x28, 0x8, 0x8, 0x2d,

    /* U+00BA "º" */
    0x69, 0x82, 0x28,

    /* U+00BB "»" */
    0x54, 0x8, 0x82, 0x65, 0x98,

    /* U+00BC "¼" */
    0x28, 0x1, 0x0, 0x20, 0x20, 0x0, 0x81, 0x44,
    0xa, 0x48, 0x50, 0x0, 0x82, 0x40, 0x8, 0x2a,
    0x80, 0x10, 0x4, 0x0,

    /* U+00BD "½" */
    0x28, 0x1, 0x0, 0x80, 0x80, 0x8, 0x14, 0x2,
    0x92, 0x18, 0x0, 0x80, 0x20, 0x20, 0x14, 0x1,
    0x6, 0x90,

    /* U+00BE "¾" */
    0x69, 0x1, 0x0, 0x20, 0x20, 0x0, 0x21, 0x44,
    0x1a, 0x48, 0x50, 0x0, 0x82, 0x40, 0x8, 0x2a,
    0x80, 0x10, 0x4, 0x0,

    /* U+00BF "¿" */
    0x9, 0x0, 0x0, 0x9, 0x1, 0xc0, 0x30, 0x3,
    0x4, 0x1f, 0x80,

    /* U+00C0 "À" */
    0x1, 0x0, 0x0, 0x20, 0x0, 0xd, 0x0, 0xa,
    0xc0, 0x3, 0x24, 0x1, 0x42, 0x0, 0xff, 0xc0,
    0x60, 0x18, 0x30, 0x3, 0x0,

    /* U+00C1 "Á" */
    0x0, 0x10, 0x0, 0x24, 0x0, 0xd, 0x0, 0xa,
    0xc0, 0x3, 0x24, 0x1, 0x42, 0x0, 0xff, 0xc0,
    0x60, 0x18, 0x30, 0x3, 0x0,

    /* U+00C2 "Â" */
    0x0, 0x40, 0x0, 0x98, 0x0, 0xd, 0x0, 0xa,
    0xc0, 0x3, 0x24, 0x1, 0x42, 0x0, 0xff, 0xc0,
    0x60, 0x18, 0x30, 0x3, 0x0,

    /* U+00C3 "Ã" */
    0x1, 0x40, 0x0, 0x48, 0x0, 0xd, 0x0, 0xa,
    0xc0, 0x3, 0x24, 0x1, 0x42, 0x0, 0xff, 0xc0,
    0x60, 0x18, 0x30, 0x3, 0x0,

    /* U+00C4 "Ä" */
    0x2, 0x20, 0x0, 0x34, 0x0, 0x2b, 0x0, 0xc,
    0x90, 0x5, 0x8, 0x3, 0xff, 0x1, 0x80, 0x60,
    0xc0, 0xc,

    /* U+00C5 "Å" */
    0x0, 0x50, 0x0, 0x14, 0x0, 0xd, 0x0, 0xa,
    0xc0, 0x3, 0x24, 0x1, 0x42, 0x0, 0xff, 0xc0,
    0x60, 0x18, 0x30, 0x3, 0x0,

    /* U+00C6 "Æ" */
    0x0, 0x3f, 0xf8, 0x2, 0x70, 0x0, 0x18, 0xc0,
    0x0, 0xc3, 0xfd, 0xb, 0xfc, 0x0, 0x20, 0x30,
    0x2, 0x40, 0xff, 0x80,

    /* U+00C7 "Ç" */
    0xb, 0xf0, 0xd0, 0x16, 0x0, 0x18, 0x0, 0x60,
    0x0, 0xd0, 0x10, 0xbf, 0x0, 0x50, 0x1, 0x80,

    /* U+00C8 "È" */
    0x10, 0x0, 0x80, 0xff, 0xcc, 0x0, 0xc0, 0xf,
    0xf4, 0xc0, 0xc, 0x0, 0xff, 0xc0,

    /* U+00C9 "É" */
    0x1, 0x0, 0x90, 0xff, 0xcc, 0x0, 0xc0, 0xf,
    0xf4, 0xc0, 0xc, 0x0, 0xff, 0xc0,

    /* U+00CA "Ê" */
    0x4, 0x2, 0x60, 0xff, 0xcc, 0x0, 0xc0, 0xf,
    0xf4, 0xc0, 0xc, 0x0, 0xff, 0xc0,

    /* U+00CB "Ë" */
    0x22, 0xf, 0xfc, 0xc0, 0xc, 0x0, 0xff, 0x4c,
    0x0, 0xc0, 0xf, 0xfc,

    /* U+00CC "Ì" */
    0x10, 0x8, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc,
    0xc,

    /* U+00CD "Í" */
    0x4, 0x24, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30,

    /* U+00CE "Î" */
    0x4, 0x9, 0x80, 0xc0, 0x30, 0xc, 0x3, 0x0,
    0xc0, 0x30, 0xc, 0x0,

    /* U+00CF "Ï" */
    0x88, 0xc3, 0xc, 0x30, 0xc3, 0xc,

    /* U+00D0 "Ð" */
    0x3f, 0xe0, 0x30, 0xc, 0x30, 0x6, 0xbe, 0x3,
    0x30, 0x6, 0x30, 0xc, 0x3f, 0xf4,

    /* U+00D1 "Ñ" */
    0x8, 0x40, 0x49, 0xd, 0x3, 0x3c, 0xc, 0xdc,
    0x33, 0x28, 0xcc, 0x27, 0x30, 0x3c, 0xc0, 0x30,

    /* U+00D2 "Ò" */
    0x1, 0x0, 0x2, 0x40, 0xb, 0xf4, 0x34, 0xd,
    0x60, 0x6, 0x60, 0x3, 0x60, 0x6, 0x34, 0xd,
    0xb, 0xf4,

    /* U+00D3 "Ó" */
    0x0, 0x10, 0x0, 0x80, 0xb, 0xf4, 0x34, 0xd,
    0x60, 0x6, 0x60, 0x3, 0x60, 0x6, 0x34, 0xd,
    0xb, 0xf4,

    /* U+00D4 "Ô" */
    0x0, 0x40, 0x2, 0x60, 0xb, 0xf4, 0x34, 0xd,
    0x60, 0x6, 0x60, 0x3, 0x60, 0x6, 0x34, 0xd,
    0xb, 0xf4,

    /* U+00D5 "Õ" */
    0x2, 0x10, 0x4, 0x50, 0xb, 0xf4, 0x34, 0xd,
    0x60, 0x6, 0x60, 0x3, 0x60, 0x6, 0x34, 0xd,
    0xb, 0xf4,

    /* U+00D6 "Ö" */
    0x2, 0x10, 0xb, 0xf4, 0x34, 0xd, 0x60, 0x6,
    0x60, 0x3, 0x60, 0x6, 0x34, 0xd, 0xb, 0xf4,

    /* U+00D7 "×" */
    0x0, 0x85, 0x3c, 0x69, 0x0,

    /* U+00D8 "Ø" */
    0x0, 0x0, 0xb, 0xfc, 0x34, 0x3d, 0x60, 0x56,
    0x60, 0x83, 0x62, 0x6, 0x3c, 0xd, 0x2f, 0xf4,
    0x10, 0x0,

    /* U+00D9 "Ù" */
    0x5, 0x0, 0x9, 0x3, 0x0, 0xcc, 0x3, 0x30,
    0xc, 0xc0, 0x33, 0x0, 0xc9, 0xa, 0xb, 0xe0,

    /* U+00DA "Ú" */
    0x0, 0x50, 0x6, 0x3, 0x0, 0xcc, 0x3, 0x30,
    0xc, 0xc0, 0x33, 0x0, 0xc9, 0xa, 0xb, 0xe0,

    /* U+00DB "Û" */
    0x1, 0x40, 0x1a, 0x43, 0x0, 0xcc, 0x3, 0x30,
    0xc, 0xc0, 0x33, 0x0, 0xc9, 0xa, 0xb, 0xe0,

    /* U+00DC "Ü" */
    0x5, 0x50, 0xc0, 0x33, 0x0, 0xcc, 0x3, 0x30,
    0xc, 0xc0, 0x32, 0x42, 0x82, 0xf8,

    /* U+00DD "Ý" */
    0x0, 0x10, 0x0, 0xc0, 0x24, 0xc, 0xc, 0x18,
    0xa, 0x30, 0x3, 0x90, 0x1, 0xc0, 0x1, 0x80,
    0x1, 0x80,

    /* U+00DE "Þ" */
    0xc0, 0xf, 0xf4, 0xc0, 0x9c, 0x6, 0xc0, 0x9f,
    0xf4, 0xc0, 0x0,

    /* U+00DF "ß" */
    0x1f, 0x80, 0xc1, 0x83, 0x6, 0xc, 0xb4, 0x30,
    0x30, 0xc0, 0xc3, 0x3d, 0x0,

    /* U+00E0 "à" */
    0x18, 0x0, 0x0, 0x2f, 0x40, 0xc, 0x2a, 0xc6,
    0xc, 0x3a, 0xc0,

    /* U+00E1 "á" */
    0x3, 0x0, 0x0, 0x2f, 0x40, 0xc, 0x2a, 0xc6,
    0xc, 0x3a, 0xc0,

    /* U+00E2 "â" */
    0xa, 0x0, 0x0, 0x2f, 0x40, 0xc, 0x2a, 0xc6,
    0xc, 0x3a, 0xc0,

    /* U+00E3 "ã" */
    0x1a, 0x40, 0x0, 0x2f, 0x40, 0xc, 0x2a, 0xc6,
    0xc, 0x3a, 0xc0,

    /* U+00E4 "ä" */
    0x15, 0x0, 0x0, 0x2f, 0x40, 0xc, 0x2a, 0xc6,
    0xc, 0x3a, 0xc0,

    /* U+00E5 "å" */
    0x5, 0x0, 0x50, 0x5, 0x2, 0xf4, 0x0, 0xc2,
    0xac, 0x60, 0xc3, 0xac,

    /* U+00E6 "æ" */
    0x2f, 0x6f, 0x40, 0xd, 0xc, 0x2a, 0xea, 0x96,
    0xd, 0x0, 0x3f, 0x6f, 0x80,

    /* U+00E7 "ç" */
    0x1f, 0x87, 0x4, 0x90, 0x7, 0x4, 0x1f, 0x80,
    0x50, 0x6, 0x0,

    /* U+00E8 "è" */
    0x8, 0x0, 0x0, 0x2f, 0x86, 0x9, 0xba, 0xa6,
    0x0, 0x1f, 0x80,

    /* U+00E9 "é" */
    0x2, 0x40, 0x0, 0x2f, 0x86, 0x9, 0xba, 0xa6,
    0x0, 0x1f, 0x80,

    /* U+00EA "ê" */
    0xa, 0x0, 0x0, 0x2f, 0x86, 0x9, 0xba, 0xa6,
    0x0, 0x1f, 0x80,

    /* U+00EB "ë" */
    0x5, 0x40, 0x0, 0x2f, 0x86, 0x9, 0xba, 0xa6,
    0x0, 0x1f, 0x80,

    /* U+00EC "ì" */
    0x24, 0x0, 0xc, 0xc, 0xc, 0xc, 0xc,

    /* U+00ED "í" */
    0x28, 0x0, 0x30, 0x30, 0x30, 0x30, 0x30,

    /* U+00EE "î" */
    0x1d, 0x0, 0xc, 0xc, 0xc, 0xc, 0xc,

    /* U+00EF "ï" */
    0x94, 0x3, 0xc, 0x30, 0xc3, 0x0,

    /* U+00F0 "ð" */
    0x3f, 0x90, 0xad, 0x10, 0x31, 0xa7, 0x60, 0xb6,
    0x6, 0x2f, 0x80,

    /* U+00F1 "ñ" */
    0xa, 0x80, 0x0, 0x3a, 0xd3, 0x3, 0x30, 0x33,
    0x3, 0x30, 0x30,

    /* U+00F2 "ò" */
    0xc, 0x0, 0x0, 0x1f, 0x87, 0x6, 0x90, 0x37,
    0x6, 0x1f, 0x80,

    /* U+00F3 "ó" */
    0x2, 0x40, 0x0, 0x1f, 0x87, 0x6, 0x90, 0x37,
    0x6, 0x1f, 0x80,

    /* U+00F4 "ô" */
    0xa, 0x0, 0x0, 0x1f, 0x87, 0x6, 0x90, 0x37,
    0x6, 0x1f, 0x80,

    /* U+00F5 "õ" */
    0xa, 0x40, 0x0, 0x1f, 0x87, 0x6, 0x90, 0x37,
    0x6, 0x1f, 0x80,

    /* U+00F6 "ö" */
    0x9, 0x40, 0x0, 0x1f, 0x87, 0x6, 0x90, 0x37,
    0x6, 0x1f, 0x80,

    /* U+00F7 "÷" */
    0x9, 0x0, 0x0, 0x2a, 0x80, 0x0, 0x9, 0x0,
    0x0,

    /* U+00F8 "ø" */
    0x0, 0x1, 0xfc, 0x71, 0xa9, 0x13, 0x78, 0x63,
    0xf8, 0x0, 0x0,

    /* U+00F9 "ù" */
    0x9, 0x0, 0x0, 0x30, 0x33, 0x3, 0x30, 0x33,
    0x7, 0x1e, 0xb0,

    /* U+00FA "ú" */
    0x2, 0x80, 0x0, 0x30, 0x33, 0x3, 0x30, 0x33,
    0x7, 0x1e, 0xb0,

    /* U+00FB "û" */
    0xa, 0x40, 0x0, 0x30, 0x33, 0x3, 0x30, 0x33,
    0x7, 0x1e, 0xb0,

    /* U+00FC "ü" */
    0x8, 0x80, 0x0, 0x30, 0x33, 0x3, 0x30, 0x33,
    0x7, 0x1e, 0xb0,

    /* U+00FD "ý" */
    0x1, 0x80, 0x0, 0x3, 0x2, 0x46, 0xc, 0xc,
    0x50, 0x1b, 0x0, 0x38, 0x0, 0xc0, 0x3d, 0x0,

    /* U+00FE "þ" */
    0x30, 0x0, 0xc0, 0x3, 0xbd, 0xd, 0xc, 0x30,
    0x24, 0xd0, 0xc3, 0xbd, 0xc, 0x0, 0x30, 0x0,

    /* U+00FF "ÿ" */
    0x8, 0x80, 0x0, 0x3, 0x2, 0x46, 0xc, 0xc,
    0x50, 0x1b, 0x0, 0x38, 0x0, 0xc0, 0x3d, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 43, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 43, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 43, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 63, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 7, .adv_w = 112, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 20, .adv_w = 99, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 34, .adv_w = 135, .box_w = 9, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 50, .adv_w = 110, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 64, .adv_w = 34, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 66, .adv_w = 54, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 75, .adv_w = 54, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 82, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 86, .adv_w = 93, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 94, .adv_w = 36, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 96, .adv_w = 61, .box_w = 4, .box_h = 1, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 97, .adv_w = 36, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 98, .adv_w = 56, .box_w = 5, .box_h = 9, .ofs_x = -1, .ofs_y = -1},
    {.bitmap_index = 110, .adv_w = 107, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 123, .adv_w = 59, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 129, .adv_w = 92, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 140, .adv_w = 92, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 151, .adv_w = 107, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 164, .adv_w = 92, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 175, .adv_w = 99, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 186, .adv_w = 96, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 197, .adv_w = 103, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 208, .adv_w = 99, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 219, .adv_w = 36, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 222, .adv_w = 36, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 226, .adv_w = 93, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 234, .adv_w = 93, .box_w = 6, .box_h = 3, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 239, .adv_w = 93, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 247, .adv_w = 92, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 258, .adv_w = 165, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 281, .adv_w = 117, .box_w = 9, .box_h = 7, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 297, .adv_w = 121, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 310, .adv_w = 116, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 323, .adv_w = 132, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 336, .adv_w = 107, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 347, .adv_w = 102, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 356, .adv_w = 124, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 369, .adv_w = 130, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 382, .adv_w = 50, .box_w = 2, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 386, .adv_w = 82, .box_w = 6, .box_h = 7, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 397, .adv_w = 115, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 410, .adv_w = 95, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 419, .adv_w = 153, .box_w = 8, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 433, .adv_w = 130, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 446, .adv_w = 134, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 460, .adv_w = 116, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 471, .adv_w = 134, .box_w = 9, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 489, .adv_w = 116, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 500, .adv_w = 99, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 511, .adv_w = 94, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 522, .adv_w = 127, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 535, .adv_w = 114, .box_w = 9, .box_h = 7, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 551, .adv_w = 180, .box_w = 11, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 571, .adv_w = 108, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 584, .adv_w = 104, .box_w = 8, .box_h = 7, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 598, .adv_w = 105, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 611, .adv_w = 53, .box_w = 3, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 618, .adv_w = 56, .box_w = 5, .box_h = 9, .ofs_x = -1, .ofs_y = -1},
    {.bitmap_index = 630, .adv_w = 53, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 637, .adv_w = 93, .box_w = 6, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 643, .adv_w = 80, .box_w = 5, .box_h = 1, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 645, .adv_w = 96, .box_w = 3, .box_h = 1, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 646, .adv_w = 96, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 654, .adv_w = 109, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 667, .adv_w = 91, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 675, .adv_w = 109, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 686, .adv_w = 98, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 694, .adv_w = 56, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 701, .adv_w = 110, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 712, .adv_w = 109, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 723, .adv_w = 45, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 729, .adv_w = 45, .box_w = 4, .box_h = 9, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 738, .adv_w = 99, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 751, .adv_w = 45, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 755, .adv_w = 169, .box_w = 10, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 768, .adv_w = 109, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 776, .adv_w = 102, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 784, .adv_w = 109, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 797, .adv_w = 109, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 808, .adv_w = 66, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 813, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 820, .adv_w = 66, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 826, .adv_w = 108, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 834, .adv_w = 89, .box_w = 7, .box_h = 5, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 843, .adv_w = 144, .box_w = 9, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 855, .adv_w = 88, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 863, .adv_w = 89, .box_w = 7, .box_h = 7, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 876, .adv_w = 83, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 883, .adv_w = 56, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 892, .adv_w = 48, .box_w = 1, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 895, .adv_w = 56, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 902, .adv_w = 93, .box_w = 6, .box_h = 2, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 905, .adv_w = 43, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 905, .adv_w = 43, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 909, .adv_w = 91, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 921, .adv_w = 103, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 934, .adv_w = 112, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 947, .adv_w = 113, .box_w = 9, .box_h = 7, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 963, .adv_w = 48, .box_w = 1, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 966, .adv_w = 80, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 976, .adv_w = 96, .box_w = 4, .box_h = 1, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 977, .adv_w = 128, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 991, .adv_w = 65, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 994, .adv_w = 81, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 999, .adv_w = 93, .box_w = 6, .box_h = 3, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 1004, .adv_w = 61, .box_w = 4, .box_h = 1, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 1005, .adv_w = 128, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1019, .adv_w = 96, .box_w = 4, .box_h = 1, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 1020, .adv_w = 67, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 1024, .adv_w = 93, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1033, .adv_w = 69, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 1037, .adv_w = 69, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 1041, .adv_w = 96, .box_w = 3, .box_h = 1, .ofs_x = 2, .ofs_y = 6},
    {.bitmap_index = 1042, .adv_w = 109, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1053, .adv_w = 104, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1065, .adv_w = 43, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 1067, .adv_w = 96, .box_w = 3, .box_h = 2, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1069, .adv_w = 69, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 1073, .adv_w = 67, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1076, .adv_w = 81, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 1081, .adv_w = 165, .box_w = 11, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1101, .adv_w = 165, .box_w = 10, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1119, .adv_w = 165, .box_w = 11, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1139, .adv_w = 92, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1150, .adv_w = 117, .box_w = 9, .box_h = 9, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1171, .adv_w = 117, .box_w = 9, .box_h = 9, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1192, .adv_w = 117, .box_w = 9, .box_h = 9, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1213, .adv_w = 117, .box_w = 9, .box_h = 9, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1234, .adv_w = 117, .box_w = 9, .box_h = 8, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1252, .adv_w = 117, .box_w = 9, .box_h = 9, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1273, .adv_w = 168, .box_w = 11, .box_h = 7, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1293, .adv_w = 116, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1309, .adv_w = 107, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1323, .adv_w = 107, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1337, .adv_w = 107, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1351, .adv_w = 107, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1363, .adv_w = 50, .box_w = 4, .box_h = 9, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1372, .adv_w = 50, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1381, .adv_w = 50, .box_w = 5, .box_h = 9, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1393, .adv_w = 50, .box_w = 3, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1399, .adv_w = 133, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1413, .adv_w = 130, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1429, .adv_w = 134, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1447, .adv_w = 134, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1465, .adv_w = 134, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1483, .adv_w = 134, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1501, .adv_w = 134, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1517, .adv_w = 93, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 1522, .adv_w = 134, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1540, .adv_w = 127, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1556, .adv_w = 127, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1572, .adv_w = 127, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1588, .adv_w = 127, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1602, .adv_w = 104, .box_w = 8, .box_h = 9, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1620, .adv_w = 116, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1631, .adv_w = 108, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1644, .adv_w = 96, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1655, .adv_w = 96, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1666, .adv_w = 96, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1677, .adv_w = 96, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1688, .adv_w = 96, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1699, .adv_w = 96, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1711, .adv_w = 158, .box_w = 10, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1724, .adv_w = 91, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1735, .adv_w = 98, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1746, .adv_w = 98, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1757, .adv_w = 98, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1768, .adv_w = 98, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1779, .adv_w = 45, .box_w = 4, .box_h = 7, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1786, .adv_w = 45, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1793, .adv_w = 45, .box_w = 4, .box_h = 7, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1800, .adv_w = 45, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1806, .adv_w = 102, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1817, .adv_w = 109, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1828, .adv_w = 102, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1839, .adv_w = 102, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1850, .adv_w = 102, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1861, .adv_w = 102, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1872, .adv_w = 102, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1883, .adv_w = 93, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1892, .adv_w = 102, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1903, .adv_w = 108, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1914, .adv_w = 108, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1925, .adv_w = 108, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1936, .adv_w = 108, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1947, .adv_w = 89, .box_w = 7, .box_h = 9, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 1963, .adv_w = 109, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1979, .adv_w = 89, .box_w = 7, .box_h = 9, .ofs_x = -1, .ofs_y = -2}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0xd
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 0, .range_length = 14, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 2, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 3,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 160, .range_length = 96, .glyph_id_start = 98,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 0, 0, 0, 1, 2, 0, 3,
    4, 5, 2, 6, 7, 8, 9, 10,
    9, 10, 11, 12, 0, 13, 14, 15,
    16, 17, 18, 19, 12, 20, 20, 0,
    0, 0, 21, 22, 23, 24, 25, 22,
    26, 27, 28, 29, 29, 30, 31, 32,
    29, 29, 22, 33, 34, 35, 3, 36,
    30, 37, 37, 38, 39, 40, 41, 42,
    43, 0, 44, 0, 45, 46, 47, 48,
    49, 50, 51, 45, 52, 52, 53, 48,
    45, 45, 46, 46, 54, 55, 56, 57,
    51, 58, 58, 59, 58, 60, 41, 0,
    0, 9, 0, 61, 47, 62, 63, 64,
    0, 65, 0, 22, 8, 66, 0, 9,
    22, 0, 67, 0, 0, 0, 0, 51,
    29, 9, 0, 0, 8, 9, 0, 0,
    0, 68, 23, 23, 23, 23, 23, 23,
    26, 25, 26, 26, 26, 26, 29, 29,
    29, 29, 22, 29, 22, 22, 22, 22,
    22, 0, 22, 30, 30, 30, 30, 39,
    69, 46, 45, 45, 45, 45, 45, 45,
    49, 47, 49, 49, 49, 49, 51, 51,
    70, 51, 71, 45, 46, 46, 46, 46,
    46, 9, 46, 51, 51, 51, 51, 58,
    46, 58
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 0, 0, 0, 1, 2, 0, 3,
    4, 5, 2, 6, 7, 8, 9, 10,
    9, 10, 11, 12, 13, 14, 15, 16,
    17, 12, 18, 19, 20, 21, 21, 0,
    0, 0, 22, 23, 24, 25, 23, 25,
    25, 25, 23, 25, 25, 26, 25, 25,
    25, 25, 23, 25, 23, 25, 3, 27,
    28, 29, 29, 30, 31, 32, 33, 34,
    35, 0, 36, 0, 37, 38, 39, 39,
    39, 0, 39, 38, 40, 41, 38, 38,
    42, 42, 39, 42, 39, 42, 43, 44,
    45, 46, 46, 47, 46, 48, 0, 0,
    35, 9, 0, 49, 39, 50, 51, 52,
    0, 53, 0, 23, 8, 9, 9, 9,
    23, 0, 54, 0, 0, 0, 0, 42,
    55, 9, 0, 0, 8, 56, 0, 0,
    0, 57, 24, 24, 24, 24, 24, 24,
    24, 23, 25, 25, 25, 25, 25, 25,
    25, 25, 25, 25, 23, 23, 23, 23,
    23, 0, 23, 28, 28, 28, 28, 31,
    25, 38, 37, 37, 37, 37, 37, 37,
    37, 39, 39, 39, 39, 39, 58, 42,
    59, 60, 39, 42, 39, 39, 39, 39,
    39, 9, 39, 45, 45, 45, 45, 46,
    38, 46
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 0, 0, 0,
    0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 7, 0, 4,
    -4, 0, 0, 0, 0, -9, -10, 1,
    8, 4, 3, -6, 1, 8, 0, 7,
    2, 5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 10, 1, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, 3,
    0, 3, 7, 0, -7, 0, 0, 0,
    0, -5, 0, 0, 0, 0, 0, -3,
    3, 3, 0, 0, -2, 0, -1, 2,
    0, -2, 0, -2, -1, -3, 0, 0,
    0, 0, -2, 0, 0, -2, -2, 0,
    0, -2, 0, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, -2, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    -2, 0, 0, 0, 0, -4, 0, -19,
    0, 0, -3, 0, 3, 5, 0, 0,
    -3, 2, 2, 5, 3, -3, 3, 0,
    0, -9, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, 0, 0, 0, 0, 0,
    -2, -8, 0, -6, -1, 0, 0, 0,
    0, 0, 6, 0, -5, -1, 0, 0,
    0, -3, 0, 0, -1, -12, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -13, -1, 6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, 0, 0, -7, -7, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 5, 0,
    2, 0, 0, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 6, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 3, 2, 5,
    -2, 0, 0, 3, -2, -5, -22, 1,
    4, 3, 0, -2, 0, 6, 0, 5,
    0, 5, 0, -15, 0, -2, 5, 0,
    5, -2, 3, 2, 0, 0, 0, -2,
    0, 0, -3, 13, 0, 13, 0, 5,
    0, 7, 2, 3, 0, 0, 1, 0,
    0, 5, 6, 2, -13, 13, 13, 13,
    0, 0, 0, -6, 0, 0, 0, 0,
    0, -1, 0, 1, -3, -2, -3, 1,
    0, -2, 0, 0, 0, -6, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -10, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, -4, 2, 0, -14, -2,
    0, 0, 0, 0, 0, -9, 0, -10,
    0, 0, 0, 0, -1, 0, 16, -2,
    -2, 2, 2, -1, 0, -2, 2, 0,
    0, -8, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -16, 0, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -2, -5,
    0, -10, -21, 0, 2, 0, 0, 0,
    0, 10, 0, 0, -6, 0, 5, 0,
    -11, -16, -11, -3, 5, 0, 0, -11,
    0, 2, -4, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 4, 5, -20, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -10, -2, 0, 6, -3, 8, 0, -8,
    -12, 0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 1, 1, -2, -3, 0,
    0, 0, -2, 0, 0, -1, 0, 0,
    0, -3, 0, -1, 0, -4, -3, 0,
    -4, -5, -5, -3, 0, -3, 0, -3,
    0, 0, 0, 0, -1, 0, 0, 2,
    0, 1, -2, 0, 0, 0, 0, -2,
    0, 0, 0, 0, -5, 0, 0, 0,
    0, 0, 0, 2, -1, 0, 0, 0,
    -1, 2, 2, 0, 0, 0, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, -1, 0, -2, 0, -3, 0,
    0, -1, 0, 5, 0, 0, -2, 0,
    0, 0, 0, 0, 0, 0, -1, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -1, -1, 0, -2, -2, 0, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    -2, -2, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -1, 0, 0, 0,
    0, -1, -2, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, -5, -1, -5, 3, 0, 0, -3,
    2, 3, 4, 0, -4, 0, -2, 0,
    0, -8, 2, -1, 1, -8, 2, 0,
    0, 0, -8, 0, -8, -1, -14, -1,
    0, -8, 0, 3, 4, 0, 2, 0,
    0, 0, 0, 0, 0, -3, -2, 0,
    0, 2, 2, -2, 3, -5, -3, 0,
    2, 0, 0, 0, 0, 0, 0, -2,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, -1, -1, 0, -1, -2, 0, 0,
    0, 0, 0, 0, 0, -2, -2, 0,
    -1, -2, -1, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -1, -1, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, -1, 0, -3, 2, 0, 0, -2,
    1, 2, 2, 0, 0, 0, 0, 0,
    0, -1, 0, 0, 0, 0, 0, 1,
    0, 0, -2, 0, -2, -1, -2, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    -1, 0, 0, 0, 0, -2, -2, 0,
    0, 0, 2, 0, 3, -3, 0, 0,
    0, 0, 0, 0, 0, 5, -1, 0,
    -5, 0, 0, 4, -8, -8, -7, -3,
    2, 0, -1, -10, -3, 0, -3, 0,
    -3, 2, -3, -10, 0, -4, 0, 0,
    1, 0, 1, -1, 0, 2, 0, -5,
    -6, 0, -8, -4, -3, -4, -5, -2,
    -4, 0, -3, -4, -4, -2, -3, 2,
    -2, 1, 4, -3, -10, -4, -4, -4,
    0, 0, 0, -2, 0, 0, 0, 1,
    0, 2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, -1,
    0, 0, -2, 0, -3, -4, -4, 0,
    0, -5, 0, 0, 0, 0, 0, 0,
    -1, 0, 0, 0, 0, 1, -1, 0,
    0, 0, 0, -2, 2, 0, 0, 0,
    -3, 0, 0, 0, 0, 2, 0, 0,
    0, 0, 0, 0, 0, 0, 8, 0,
    0, 0, 0, 0, 0, 1, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, 0, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -6,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, 0, 0, 0, -3, 0, 0, 0,
    0, -8, -5, 0, 0, 0, -2, -8,
    0, 0, -2, 2, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, 0, 0, -2, 0, 2, 0, 2,
    -17, 0, 0, 0, 0, -3, 0, 0,
    0, 0, 2, 0, 1, -3, -3, 0,
    -2, -2, -2, 0, 0, 0, 0, 0,
    0, -5, 0, -2, 0, -2, -2, 0,
    -4, -4, -5, -1, 0, -3, 0, -5,
    0, 0, 0, 0, 13, 0, 0, 1,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 2, 0, 0, -3, 0, 0, 0,
    0, -7, 0, 0, 0, 0, 0, -15,
    -3, 5, 5, -1, -7, 0, 2, -2,
    0, -8, -1, -2, 2, -11, -2, 2,
    0, 2, -6, -2, -6, -5, -7, 0,
    0, -10, 0, 9, 0, 0, -1, 0,
    0, 0, -1, -1, -2, -4, -5, 0,
    2, 0, 0, 0, 0, -15, -13, 2,
    3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -2, 0,
    -1, -2, -2, 0, 0, -3, 0, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, 0, 0, 3,
    0, 2, 0, -4, 2, -1, 0, -4,
    -2, 0, -2, -2, -1, 0, -2, -3,
    0, 0, -1, 0, -1, -3, -2, 0,
    0, -2, 0, 2, -1, 0, -4, 0,
    0, 0, -3, 0, -3, 0, -3, -3,
    -2, 0, 0, 0, -1, 2, 1, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, 2, 0, -2,
    0, -1, -2, -5, -1, -1, -1, 0,
    -1, -2, 0, 0, 0, 0, 0, 0,
    -2, -1, -1, 0, 0, 0, 0, 2,
    -1, 0, -1, 0, 0, 0, -1, -2,
    -1, -1, -2, -1, -1, 0, 0, 0,
    -1, 0, 0, 0, 0, 0, 0, 0,
    1, 6, 0, 0, -4, 0, -1, 3,
    0, -2, -7, -2, 2, 0, 0, -8,
    -3, 2, -3, 1, 0, -1, -1, -5,
    0, -2, 1, 0, 0, -3, 0, 0,
    0, 2, 2, -3, -3, 0, -3, -2,
    -2, -2, -2, 0, -3, 1, -3, -3,
    -2, 0, 0, 0, -1, 5, 2, -2,
    -8, -2, 2, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -1, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, 0, -2, 0,
    0, -2, -2, 0, 0, 0, 0, -2,
    0, 0, 0, 0, -1, 0, 0, 0,
    0, 0, -1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -1, 0, 0, 0,
    0, 0, -2, 0, -3, 0, 0, 0,
    -5, 0, 1, -4, 3, 0, -1, -8,
    0, 0, -4, -2, 0, -6, -4, -4,
    0, 0, -7, -2, -6, -6, -8, 0,
    -4, 0, 1, 11, -2, 0, -4, -2,
    0, -2, -3, -4, -3, -6, -7, -4,
    0, 0, 0, 0, 0, -2, -1, 0,
    0, -2, -2, -2, 0, 0, -1, 0,
    0, 0, 0, -11, -1, 5, 4, -4,
    -6, 0, 0, -5, 0, -8, -1, -2,
    3, -15, -2, 0, 0, 0, -10, -2,
    -8, -2, -12, 0, 0, -11, 0, 9,
    0, 0, -1, 0, 0, 0, 0, -1,
    -1, -6, -1, 0, 0, 0, 0, 0,
    0, -10, -12, 1, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 0, -1, 0,
    0, -4, -8, 0, 0, -1, -2, -5,
    -2, 0, -1, 0, 0, 0, 0, -7,
    -2, -5, -5, -1, -3, -4, -2, -3,
    0, -3, -1, -5, -2, 0, -2, -3,
    -2, -3, 0, 1, 0, -1, -5, 0,
    0, 0, 0, 0, 0, 3, 0, 0,
    -8, -3, -3, -3, 0, -3, 0, 0,
    0, 0, 2, 0, 1, -3, 7, 0,
    -2, -2, -2, 0, 0, 0, 0, 0,
    0, -5, 0, -2, 0, -2, -2, 0,
    -4, -4, -5, -1, 0, -3, 1, 6,
    0, 0, 0, 0, 13, 0, 0, 1,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 2, 0, 0, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, -3,
    0, 0, 0, 0, 0, -1, 0, 0,
    0, -2, -2, 0, 0, -3, -2, 0,
    0, -3, 0, 3, -1, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 2, 0, 0,
    -2, 0, 0, 0, 3, 1, -1, 0,
    -5, -3, 0, 5, -5, -5, -3, -3,
    6, 3, 2, -14, -1, 3, -2, 0,
    -2, 2, -2, -6, 0, -2, 2, -2,
    -1, -5, -1, 0, 0, 5, 3, 0,
    -4, 0, -9, -2, 5, -2, -6, 0,
    -2, -5, -5, -2, -3, 0, 0, 0,
    0, 6, 5, -2, -9, 2, 5, 3,
    2, 0, -2, 0, -4, 0, 1, 5,
    -4, -6, -6, -4, 5, 0, 0, -12,
    -1, 2, -3, -1, -4, 0, -4, -6,
    -2, -2, -1, 0, 0, -4, -3, -2,
    0, 5, 4, -2, -9, 0, -9, -2,
    0, -6, -9, 0, -5, -3, -5, -4,
    -5, 0, 0, 0, 0, 4, 4, -2,
    -12, 3, 6, 3, 0, 0, -2, 0,
    -3, -1, 0, -2, -3, 0, 3, -5,
    2, 0, 0, -8, 0, -2, -4, -3,
    -1, -5, -4, -5, -4, 0, -5, -2,
    -4, -3, -5, -2, 0, 0, 0, 8,
    -3, 0, -5, -2, 0, -2, -3, -4,
    -4, -4, -6, -2, -2, 0, 0, 0,
    0, -3, 0, -1, 0, -2, -2, -2,
    3, 0, -2, 0, -8, -2, 1, 3,
    -5, -6, -3, -5, 5, -2, 1, -15,
    -3, 3, -4, -3, -6, 0, -5, -7,
    -2, -2, -1, -2, -3, -5, 0, 0,
    0, 5, 4, -1, -10, 0, -10, -4,
    4, -6, -11, -3, -6, -7, -8, -5,
    -6, 0, 0, 0, -3, 3, 4, -3,
    -13, 5, 5, 4, 0, 0, 0, 0,
    -2, 0, 0, 2, -2, 3, 1, -3,
    3, 0, 0, -5, 0, 0, 0, 0,
    0, 0, -1, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 1, 5,
    0, 0, -2, 0, 0, 0, 0, -1,
    -1, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 0, 1, 0,
    -1, 0, 6, 0, 3, 0, 0, -2,
    0, 3, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 5, 0, 4, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -10, 0, -2,
    3, 0, 5, 0, 0, 16, 2, -3,
    -3, 2, 2, -1, 0, -8, 0, 0,
    8, -10, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -11, 6, 22,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 6, 0, 0, 0,
    5, -10, -5, 1, 5, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 0, -3, -1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -1, 0, -4,
    0, 0, 0, 0, 0, 2, 21, -3,
    -1, 5, 4, -4, 2, 0, 0, 2,
    2, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -21, 4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 3, -1, -2,
    4, 0, -11, 2, 0, 0, 0, 0,
    0, 0, 0, -4, 0, 0, 0, -4,
    0, 0, 0, 0, -4, -1, 0, 0,
    0, -4, 0, -2, 0, -8, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -11, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, -3, -4, 0,
    0, 0, 0, 0, 0, -2, 0, -4,
    0, 0, 0, -3, 2, -2, 0, 0,
    -4, -2, -4, 0, 0, -4, 0, -2,
    0, -8, 0, -2, 0, 0, -13, -3,
    -6, -2, -6, 0, 0, -11, 0, -4,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, -2, -3, -1, 0, 0, 0, 0,
    0, -3, -4, 0, -2, 0, 0, 0,
    0, 0, 0, 0, -4, 0, -4, 2,
    -2, 3, 0, -1, -4, -1, -3, -3,
    0, -2, -1, -1, 1, -4, 0, 0,
    0, 0, -14, -1, -2, 0, -4, 0,
    -1, -8, -1, 0, 0, -1, -1, 0,
    0, 0, 0, 1, 0, -1, -3, -1,
    0, 0, 0, 0, -1, 3, 0, 0,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, -1, 0, 0, 0, -3,
    2, 0, 0, 0, -4, -2, -3, 0,
    0, -4, 0, -2, 0, -8, 0, 0,
    0, 0, -16, 0, -3, -6, -8, 0,
    0, -11, 0, -1, -2, 0, 0, 0,
    0, 0, 0, 0, 0, -2, -2, -1,
    0, 0, 0, 0, 0, -2, -3, 0,
    -2, 0, 0, 0, 0, 0, 0, 3,
    -2, 0, 5, 8, -2, -2, -5, 2,
    8, 3, 4, -4, 2, 7, 2, 5,
    4, 4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 10, 8, -3,
    -2, 0, -1, 13, 7, 13, 0, 0,
    0, 2, 0, 0, 0, 0, 0, 0,
    0, 6, 7, 0, -2, 11, 8, 10,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -1, 0, 0, 0,
    0, 0, 0, 0, 0, 2, 0, 0,
    0, 0, -13, -2, -1, -7, -8, 0,
    0, -11, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 0, -13, -2,
    -1, -7, -8, 0, 0, -6, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -1, 0, 0, 0,
    -4, 2, 0, -2, 1, 3, 2, -5,
    0, 0, -1, 2, 0, 1, 0, 0,
    0, 0, -4, 0, -1, -1, -3, 0,
    -1, -6, 0, 10, -2, 0, -4, -1,
    0, -1, -3, 0, -2, -4, -3, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -1, -1, -1, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 0, -13, -2,
    -1, -7, -8, 0, 0, -11, 0, 0,
    0, 0, 0, 0, 8, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, -5, -2, -1, 5,
    -1, -2, -6, 0, -1, 0, -1, -4,
    0, 4, 0, 1, 0, 1, -4, -6,
    -2, 0, -6, -3, -4, -7, -6, 0,
    -3, -3, -2, -2, -1, -1, -2, -1,
    0, -1, 0, 2, 0, 2, -1, 0,
    0, 0, 0, 0, 2, 5, 5, 0,
    -6, -1, -1, -1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, -2, -2, 0, 0, -4, 0, -1,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -10, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, -2, 0, 0, 0, 0, 0,
    0, -2, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -1, 0, 0, -3,
    -2, 2, 0, -3, -3, -1, 0, -5,
    -1, -4, -1, -2, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -11, 0, 5, 0, 0, -3, 0,
    0, 0, 0, -2, 0, -2, 0, 0,
    0, 0, 0, 0, -1, -1, -3, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    -4, 0, 0, 7, -2, -5, -5, 1,
    2, 2, 0, -4, 1, 2, 1, 5,
    1, 5, -1, -4, 0, 0, -6, 0,
    0, -5, -4, 0, 0, -3, 0, -2,
    -3, 0, -2, 0, -2, 0, -1, 2,
    0, -1, -5, -2, 0, 0, 0, 0,
    0, 6, 5, 0, -5, 0, 0, 0,
    0, 0, -1, 0, -3, 0, 0, 2,
    -4, 0, 2, -2, 1, 0, 0, -5,
    0, -1, 0, 0, -2, 2, -1, 0,
    0, 0, -7, -2, -4, 0, -5, 0,
    0, -8, 0, 6, -2, 0, -3, 0,
    1, 0, -2, 0, -2, -5, 0, -2,
    0, 0, 0, 0, 0, 2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, 0, 0, 2, -2, 0, 0, 0,
    -2, -1, 0, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -10, 0, 4,
    0, 0, -1, 0, 0, 0, 0, 0,
    0, -2, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 6, 0, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -9, 0, 2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -2, 0,
    -6, 0, 0, 4, -5, 0, 2, -6,
    3, -2, -2, -5, -2, 1, -5, -3,
    -5, 0, -2, -7, 0, -3, 0, 0,
    0, -2, 2, 0, 0, 3, 0, 3,
    -6, 0, -5, -3, -4, -3, -6, -3,
    -4, -3, -4, -6, -4, 0, 0, 0,
    -2, 0, 2, -2, -4, -3, 4, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, -2, 0, 0, 0, -1, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, 0, 0, 0, 2, 3, 0, 0,
    0, 0, 2, 0, -4, -5, -5, -2,
    5, 0, 2, -2, 0, 4, -2, 0,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 8, 2, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, 0, 0, 0,
    0, 0, 0, 0, -6, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 4, 0, 0, 0, 0, 3,
    0, 1, 2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 1, 0,
    0, 0, 0, 1, 1, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -9, 0, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0,
    0, 3, 0, 4, 0, 0, 0, 0,
    0, -10, -9, 0, 7, 5, 3, -6,
    1, 7, 0, 6, 0, 3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 8, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -10, 0, 0, 0, 0, -5, 0, -10,
    -3, 0, 0, 0, -6, 0, 6, -8,
    -7, 0, 0, -7, 0, -7, -7, 0,
    0, -21, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -15, 1, 8,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, -3, -8,
    0, -9, -14, -2, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, -5, 0, -3, -4, -3, 0,
    -2, 0, 0, 0, 0, -5, 0, -5,
    0, -6, -4, 0, -2, -5, -5, -3,
    0, -6, 0, -5, -2, 0, 0, 0,
    -2, 0, 0, 1, 0, 0, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, 0, -3, 0,
    0, 0, 0, 13, 0, 0, 0, 0,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, 6, 0, 0, 0, 0, -13, -2,
    -1, -7, -8, 0, 0, -11, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 2,
    0, -3, 0, 1, 0, 0, -1, 0,
    0, 0, 0, 0, 0, -1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, 0, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 3, 0, 0,
    -1, 0, 0, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 71,
    .right_class_cnt     = 60,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 3,
    .bpp = 2,
    .kern_classes = 1,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t montserrat_10_data = {
#else
lv_font_t montserrat_10_data = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 11,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if MONTSERRAT_10_DATA*/

