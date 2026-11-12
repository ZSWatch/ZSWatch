#!/usr/bin/env python3
# Copyright (c) 2026 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0
"""Generates the landscape of the Diorama watchface: the skyline of each layer, the snow, the
cottage and the trees that stand in it, and the clouds.

A layer is stored as the display row of its top edge for every column. The layers are wider
than the display, the extra columns come into view when the watch is tilted. What stands in the
landscape is stored as runs of pixels of one color, a row at a time.

Run from app/: python scripts/generate_diorama_terrain.py
"""

import math
import random

OUT = "src/ui/watchfaces/diorama/diorama_terrain.h"

SIZE = 240
MARGIN = 16
WIDTH = SIZE + 2 * MARGIN
# First row of the lake. A column with this value has no land in that layer.
LAKE_Y = 170
# Rows below the top edge of the farthest layer that can have snow
SNOW_ROWS = 40

M32 = 0xFFFFFFFF


def imul(a, b):
    return ((a & M32) * (b & M32)) & M32


def make_rng(seed):
    state = [seed & M32]

    def rng():
        state[0] = (imul(state[0], 1664525) + 1013904223) & M32
        return state[0] / 4294967296

    return rng


def clamp(v, lo, hi):
    return lo if v < lo else hi if v > hi else v


def lerp(a, b, t):
    return a + (b - a) * t


def smooth(t):
    return t * t * (3 - 2 * t)


def js_round(v):
    return math.floor(v + 0.5)


def hash2(x, y, seed):
    h = imul(x, 374761393) ^ imul(y, 668265263) ^ imul(seed, 2246822519)
    h = imul(h ^ (h >> 13), 1274126177)
    return (h ^ (h >> 16)) / 4294967296


def noise1(x, seed):
    i = math.floor(x)
    return lerp(hash2(i, 0, seed), hash2(i + 1, 0, seed), smooth(x - i))


def fbm1(x, seed, octaves):
    v = 0.0
    a = 0.5
    f = 1
    n = 0.0
    for o in range(octaves):
        v += a * noise1(x * f, seed + o * 17)
        n += a
        a *= 0.5
        f *= 2
    return v / n


def noise2(x, y, seed):
    xi = math.floor(x)
    yi = math.floor(y)
    fx = smooth(x - xi)
    fy = smooth(y - yi)
    return lerp(lerp(hash2(xi, yi, seed), hash2(xi + 1, yi, seed), fx),
                lerp(hash2(xi, yi + 1, seed), hash2(xi + 1, yi + 1, seed), fx), fy)


def line(fn):
    return [fn(x) for x in range(WIDTH)]


def pines(top, rng, lo_h, hi_h, spacing, keep=None):
    """Spruce standing on the skyline top: tiered triangles."""
    out = list(top)
    x = rng() * spacing
    while x < WIDTH:
        h = lerp(lo_h, hi_h, rng())
        half = h * lerp(0.24, 0.32, rng())
        tiers = max(2, math.floor(h / 5))
        foot = top[min(WIDTH - 1, math.floor(x))]
        if keep is None or keep(x):
            px = math.floor(x - half) - 1
            while px <= x + half + 1:
                if 0 <= px < WIDTH:
                    d = abs(px + 0.5 - x) / half
                    if d < 1:
                        # Each tier of branches reaches a bit further out than a plain triangle
                        tier = math.floor(d * tiers)
                        frac = d * tiers - tier
                        up = 1 - (tier + frac * 0.55 + 0.45) / tiers if tier < tiers - 1 else (1 - d) * 0.6
                        up = max(up, 1 - d - 0.02)
                        out[px] = min(out[px], foot - h * max(0, up) - 1)
                px += 1
        x += spacing * lerp(0.55, 1.45, rng())
    return out


def bumps(top, rng, lo_r, hi_r, spacing, keep=None, flat=0):
    """Round crowns: low mountain birch, bushes and boulders."""
    out = list(top)
    x = rng() * spacing
    while x < WIDTH:
        r = lerp(lo_r, hi_r, rng())
        foot = top[min(WIDTH - 1, math.floor(x))]
        if keep is None or keep(x):
            px = math.floor(x - r)
            while px <= x + r:
                if 0 <= px < WIDTH:
                    d = (px + 0.5 - x) / r
                    if abs(d) < 1:
                        out[px] = min(out[px], foot - r * (flat or 1) * math.sqrt(1 - d * d) - (0 if flat else r * 0.5))
                px += 1
        x += spacing * lerp(0.5, 1.6, rng())
    return out


def massif(x, c, wl, wr, h, p):
    d = abs((x - c) / (wl if x < c else wr))
    return h * (1 - d ** p) if d < 1 else 0


def cloud(rng, width, puffs):
    """A cloud as the rows above and below its base line, for every column."""
    top = [0.0] * width
    bottom = [0.0] * width
    for i in range(puffs):
        cx = (i + 0.5) * width / puffs + rng.uniform(-2, 2)
        edge = min(i, puffs - 1 - i)
        r = width / puffs * rng.uniform(0.75, 1.0) * (0.75 + 0.35 * min(edge, 2))
        for x in range(width):
            d = (x + 0.5 - cx) / r
            if abs(d) < 1:
                h = r * math.sqrt(1 - d * d)
                top[x] = max(top[x], h * 0.95)
                bottom[x] = max(bottom[x], h * 0.28)
    return [round(v) for v in top], [round(v) for v in bottom]


# What the pixels of the things in the landscape are colored with. The watch works the colors
# out for the light of the moment: see sprite_colors() in diorama_scene.c.
KIND_LIT = "LIT"                    # A color of its own, in the light of the moment
KIND_WINDOW = "WINDOW"              # Lit from the inside when it is dark
KIND_CROWN = "CROWN"                # The leaves of a birch
KIND_CROWN_SHADE = "CROWN_SHADE"
KIND_GRASS = "GRASS"                # param: how far down towards the water
KIND_STONE = "STONE"                # param: how pale
KIND_LAMP_GLOW = "LAMP_GLOW"        # Light around a window when it is dark, param: strength of 32


class Palette:
    def __init__(self):
        self.entries = []

    def get(self, kind, rgb=0, param=0):
        entry = (kind, rgb, param)
        if entry not in self.entries:
            self.entries.append(entry)
        return self.entries.index(entry)


class Sprite:
    """Pixels in the columns of a layer, drawn like on a canvas: what comes later covers."""

    def __init__(self, palette, layer_tops):
        self.palette = palette
        self.tops = layer_tops
        self.pixels = {}
        self.glow = {}

    def ground(self, layer, x):
        return js_round(self.tops[layer][clamp(x + MARGIN, 0, WIDTH - 1)])

    def put(self, x, y, color):
        if 0 <= x + MARGIN < WIDTH and 0 <= y < SIZE:
            self.pixels[(x + MARGIN, y)] = color

    def rect(self, x, y, w, h, color):
        for yy in range(y, y + h):
            for xx in range(x, x + w):
                self.put(xx, yy, color)

    def roof(self, x, y, w, h, color):
        """A roof seen from the side: narrower towards the ridge."""
        for r in range(h):
            self.rect(x + (h - 1 - r), y + r, w - 2 * (h - 1 - r), 1, color)

    def disc(self, cx, cy, r, color):
        for y in range(math.floor(cy - r), math.floor(cy + r) + 1):
            for x in range(math.floor(cx - r), math.floor(cx + r) + 1):
                if r + 0.5 - math.hypot(x - cx, y - cy) >= 0.5:
                    self.put(x, y, color)

    def lamp_glow(self, cx, cy, radius, strength):
        for y in range(cy - radius, cy + radius + 1):
            for x in range(cx - radius, cx + radius + 1):
                d2 = ((x - cx) ** 2 + (y - cy) ** 2) / (radius * radius)
                if d2 < 1 and y < LAKE_Y:
                    a = (1 - d2) * (1 - d2) * strength
                    before = self.glow.get((x + MARGIN, y), 0.0)
                    self.glow[(x + MARGIN, y)] = 1 - (1 - before) * (1 - a)

    def runs(self):
        """Rows of (x, length, color). The light is laid over the pixels, so it comes last in a row."""
        rows = {}
        for pixels in (self.pixels, {k: self.palette.get(KIND_LAMP_GLOW, param=round(a * 32))
                                     for k, a in self.glow.items() if round(a * 32) > 0}):
            for (x, y) in sorted(pixels, key=lambda k: (k[1], k[0])):
                color = pixels[(x, y)]
                row = rows.setdefault(y, [])
                if row and row[-1][2] == color and row[-1][0] + row[-1][1] == x and row[-1][1] < 127 and \
                        row[-1][3] is pixels:
                    row[-1][1] += 1
                else:
                    row.append([x, 1, color, pixels])
        return rows


def c_array(name, values, ctype="uint8_t"):
    lines = []
    for i in range(0, len(values), 20):
        lines.append("    " + ", ".join(str(v) for v in values[i:i + 20]) + ",")
    return "static const {} {}[{}] = {{\n{}\n}};\n".format(ctype, name, len(values), "\n".join(lines))


def main():
    rng = make_rng(37)

    # Fells with snow fields far away, a bare fell in front of them, heath with spruce and
    # birch, and the shore: a headland on the left and tall spruce on the right
    far = line(lambda x: 148 - max(massif(x, 60, 90, 80, 34, 2), massif(x, 190, 70, 100, 42, 2), 4) -
               3 * fbm1(x / 30, 41, 2))
    fell = line(lambda x: 151 - 6 - 16 * fbm1(x / 64, 21, 3) - 4 * math.sin(x / 52 + 2))
    # The tree line: small spruce only where the fell is low
    mid = pines(fell, rng, 3, 6, 3.4, lambda x: fell[math.floor(x)] > 137)
    heath_base = line(lambda x: 160 - 8 * fbm1(x / 80, 31, 2))
    heath = bumps(heath_base, rng, 2, 4.5, 6, lambda x: noise1(x / 26, 5) > 0.4, 0.9)
    heath = pines(heath, rng, 8, 15, 11, lambda x: noise1(x / 34, 9) > 0.52)

    def shore_base(x):
        d = (x - WIDTH / 2) / (WIDTH / 2)
        # A low headland reaches towards the middle on the left, the right shore stays at the edge
        if d < 0:
            return 176 - clamp((-d - 0.02) * 90, 0, 14) - max(0, -d - 0.6) * 30
        return 176 - max(0, d - 0.44) * 44

    near = line(shore_base)
    near = bumps(near, rng, 3, 7, 10, lambda x: shore_base(x) < 171, 0.75)
    near = bumps(near, make_rng(5), 5, 9, 17, lambda x: shore_base(x) < 165 and (x < 44 or x > WIDTH / 2))
    near = pines(near, make_rng(3), 26, 44, 15, lambda x: x > WIDTH * 0.8)

    layers = [far, mid, heath, near]
    layers = [[max(0, min(LAKE_Y, js_round(v))) for v in layer] for layer in layers]

    # Snow: a cap on the top edge, and a field further down where the snow stays in the gullies
    snow_cap = []
    snow_field = []
    snow_field_rows = []
    for x in range(WIDTH):
        mask = [d < 1 + 4 * noise1(x / 9, 8) or (d < 20 and noise2(x / 4.5, d / 12, 4) > 0.6 + d / 120)
                for d in range(SNOW_ROWS)]
        cap = 0
        while mask[cap]:
            cap += 1
        best = (0, 0)
        d = cap + 1
        while d < SNOW_ROWS:
            if mask[d]:
                start = d
                while d < SNOW_ROWS and mask[d]:
                    d += 1
                if d - start > best[1]:
                    best = (start, d - start)
            d += 1
        snow_cap.append(cap)
        snow_field.append(best[0])
        snow_field_rows.append(best[1])

    palette = Palette()

    def lit(rgb):
        return palette.get(KIND_LIT, rgb)

    white = lit(0xF4F0E6)

    # Birches stand on the ground of the heath, in front of the spruce
    trees = Sprite(palette, layers)
    brng = make_rng(77)
    crown = palette.get(KIND_CROWN)
    shade = palette.get(KIND_CROWN_SHADE)
    x = 86.0
    while x < 214:
        bx = js_round(x)
        r = 3.2 + brng() * 2.2
        h = 7 + js_round(brng() * 4)
        y = js_round(heath_base[bx + MARGIN]) + 2 + js_round(brng() * 5)
        trees.rect(bx, y - h, 1, h, white)
        trees.rect(bx, y - h // 2, 1, 1, lit(0x4A4A4A))
        trees.disc(bx + 1, y - h - r * 0.4, r * 0.8, shade)
        trees.disc(bx, y - h - r * 0.7, r, crown)
        trees.disc(bx - 1, y - h - r * 1.5, r * 0.65, crown)
        x += 9 + brng() * 22

    # The cottage on the headland
    home = Sprite(palette, layers)
    for sx in range(-20, 130):
        top = home.ground(3, sx)
        if top >= 169:
            continue
        # Grass, and a pale strip of stones along the water
        for sy in range(max(top + 1, 161), 168):
            home.put(sx, sy, palette.get(KIND_GRASS, param=sy - 161))
        home.put(sx, 168, palette.get(KIND_STONE, param=141))
        home.put(sx, 169, palette.get(KIND_STONE, param=205))
    x, y = 64, 164
    red = lit(0xB23A2E)
    roof = lit(0x3B3C46)
    chimney = (x + 23, y - 27)
    home.rect(x + 22, y - 25, 3, 6, lit(0x8A4A40))
    home.rect(x, y - 13, 30, 13, red)
    home.roof(x - 2, y - 20, 34, 7, roof)
    home.rect(x, y - 13, 1, 13, white)
    home.rect(x + 29, y - 13, 1, 13, white)
    for wx in (4, 20):
        home.rect(x + wx, y - 10, 6, 6, white)
        home.rect(x + wx + 1, y - 9, 4, 4, palette.get(KIND_WINDOW, 0x2C3848))
        home.rect(x + wx + 3, y - 9, 1, 4, palette.get(KIND_WINDOW, 0xF4F0E6))
    home.rect(x + 12, y - 9, 6, 9, white)
    home.rect(x + 13, y - 8, 4, 8, lit(0x6A2A24))
    home.lamp_glow(x + 7, y - 7, 10, 0.36)
    home.lamp_glow(x + 23, y - 7, 10, 0.36)
    # The flag
    home.rect(x + 37, y - 26, 1, 26, white)
    home.rect(x + 38, y - 26, 8, 5, lit(0x2E6FB8))
    home.rect(x + 40, y - 26, 2, 5, lit(0xF6CF3C))
    home.rect(x + 38, y - 24, 8, 1, lit(0xF6CF3C))

    # On the water: the jetty and a rowing boat
    water = Sprite(palette, layers)
    water.rect(100, 170, 36, 2, lit(0x8A7460))
    for px in (112, 120, 128, 134):
        water.rect(px, 172, 1, 4, lit(0x4A3C32))
    water.rect(139, 174, 17, 1, white)
    water.rect(140, 175, 15, 2, lit(0x7A4A34))
    water.rect(141, 177, 13, 1, lit(0x5A3626))

    # The layer a group moves with, or -1 for what lies on the water
    groups = [(2, trees), (3, home), (-1, water)]

    out = [
        "/*",
        " * Generated by scripts/generate_diorama_terrain.py, do not edit.",
        " * SPDX-License-Identifier: GPL-3.0-only",
        " */",
        "",
        "#pragma once",
        "",
        "#include <stdint.h>",
        "",
        "#define DIORAMA_TERRAIN_WIDTH   {}".format(WIDTH),
        "#define DIORAMA_TERRAIN_MARGIN  {}".format(MARGIN),
        "#define DIORAMA_LAKE_Y          {}".format(LAKE_Y),
        "#define DIORAMA_NUM_LAYERS      {}".format(len(layers)),
        "",
        "/* Row of the top edge of each layer, from the farthest to the nearest */",
        "static const uint8_t diorama_layer_top[DIORAMA_NUM_LAYERS][DIORAMA_TERRAIN_WIDTH] = {",
    ]
    for layer in layers:
        out.append("    {")
        for i in range(0, WIDTH, 20):
            out.append("        " + ", ".join(str(v) for v in layer[i:i + 20]) + ",")
        out.append("    },")
    out.append("};")
    out.append("")
    out.append("/* Highest and lowest top edge of each layer */")
    out.append("static const uint8_t diorama_layer_min_top[DIORAMA_NUM_LAYERS] = {{ {} }};".format(
        ", ".join(str(min(layer)) for layer in layers)))
    out.append("static const uint8_t diorama_layer_max_top[DIORAMA_NUM_LAYERS] = {{ {} }};".format(
        ", ".join(str(max(layer)) for layer in layers)))
    out.append("")
    out.append("/* Rows of snow below the top edge of the farthest layer */")
    out.append(c_array("diorama_snow", snow_cap))
    out.append("/* A field of snow further down: the row below the top edge that it starts at, and its rows */")
    out.append(c_array("diorama_snow_field", snow_field))
    out.append(c_array("diorama_snow_field_rows", snow_field_rows))

    out.append("/* What the pixels of the things in the landscape are colored with */")
    out.append("typedef enum {")
    for kind in (KIND_LIT, KIND_WINDOW, KIND_CROWN, KIND_CROWN_SHADE, KIND_GRASS, KIND_STONE, KIND_LAMP_GLOW):
        out.append("    DIORAMA_COLOR_{},".format(kind))
    out.append("} diorama_color_kind_t;")
    out.append("")
    out.append("typedef struct {")
    out.append("    uint8_t kind;")
    out.append("    uint8_t param;")
    out.append("    uint32_t rgb;")
    out.append("} diorama_sprite_color_t;")
    out.append("")

    all_runs = []
    all_colors = []
    group_lines = []
    for index, (layer, sprite) in enumerate(groups):
        rows = sprite.runs()
        first, last = min(rows), max(rows)
        starts = []
        for y in range(first, last + 1):
            starts.append(len(all_runs))
            for (x, length, color, _) in rows.get(y, []):
                all_runs.append(x | (length << 9))
                all_colors.append(color)
        starts.append(len(all_runs))
        out.append(c_array("diorama_sprite_rows{}".format(index), starts, "uint16_t"))
        group_lines.append("    {{ {}, {}, {}, diorama_sprite_rows{} }},".format(layer, first, last - first + 1, index))

    out.append("#define DIORAMA_NUM_SPRITE_COLORS {}".format(len(palette.entries)))
    out.append("")
    out.append("static const diorama_sprite_color_t diorama_sprite_colors[DIORAMA_NUM_SPRITE_COLORS] = {")
    for (kind, rgb, param) in palette.entries:
        out.append("    {{ DIORAMA_COLOR_{}, {}, 0x{:06X} }},".format(kind, param, rgb))
    out.append("};")
    out.append("")
    out.append("/* Runs of pixels of one color: the column of the layer in the low 9 bits, then the length */")
    out.append(c_array("diorama_sprite_runs", all_runs, "uint16_t"))
    out.append(c_array("diorama_sprite_run_colors", all_colors))
    out.append("typedef struct {")
    out.append("    /* The layer the pixels move with, or -1 for what lies on the water */")
    out.append("    int8_t layer;")
    out.append("    uint8_t first_row;")
    out.append("    uint8_t num_rows;")
    out.append("    /* Index of the first run of each row, and one past the last run */")
    out.append("    const uint16_t *rows;")
    out.append("} diorama_sprite_group_t;")
    out.append("")
    out.append("#define DIORAMA_NUM_SPRITE_GROUPS {}".format(len(groups)))
    out.append("")
    out.append("static const diorama_sprite_group_t diorama_sprite_groups[DIORAMA_NUM_SPRITE_GROUPS] = {")
    out.extend(group_lines)
    out.append("};")
    out.append("")
    out.append("/* The chimney of the cottage: the column on the display and the row the smoke starts at, with the watch held flat */")
    out.append("#define DIORAMA_CHIMNEY_X        {}".format(chimney[0]))
    out.append("#define DIORAMA_CHIMNEY_Y        {}".format(chimney[1]))
    out.append("#define DIORAMA_CHIMNEY_LAYER    3")
    out.append("")

    crng = random.Random(7)
    clouds = [cloud(crng, 58, 5), cloud(crng, 40, 4), cloud(crng, 74, 6)]
    out.append("#define DIORAMA_NUM_CLOUD_SHAPES {}".format(len(clouds)))
    out.append("")
    for i, (top, bottom) in enumerate(clouds):
        out.append(c_array("diorama_cloud{}_top".format(i), top))
        out.append(c_array("diorama_cloud{}_bottom".format(i), bottom))
    out.append("typedef struct {")
    out.append("    uint8_t width;")
    out.append("    const uint8_t *top;")
    out.append("    const uint8_t *bottom;")
    out.append("} diorama_cloud_shape_t;")
    out.append("")
    out.append("static const diorama_cloud_shape_t diorama_cloud_shapes[DIORAMA_NUM_CLOUD_SHAPES] = {")
    for i, (top, _) in enumerate(clouds):
        out.append("    {{ {}, diorama_cloud{}_top, diorama_cloud{}_bottom }},".format(len(top), i, i))
    out.append("};")
    out.append("")

    with open(OUT, "w") as f:
        f.write("\n".join(out))
    print("wrote {}: {} runs, {} colors".format(OUT, len(all_runs), len(palette.entries)))


if __name__ == "__main__":
    main()
