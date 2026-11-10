/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2026 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Replaces memcpy, memmove and memset from the picolibc in the Zephyr SDK, which is built
 * for size and moves one byte at a time. These move a word at a time, which makes LVGL
 * rendering noticeably faster. The destination is word aligned first; the source may stay
 * unaligned, which Cortex-M33 handles for single word loads.
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

// Keep GCC from turning the loops below back into calls to memcpy/memset.
#define ZSW_STRING_FUNC __attribute__((optimize("O2", "no-tree-loop-distribute-patterns")))

typedef uint32_t __attribute__((aligned(1), may_alias)) unaligned_u32_t;

ZSW_STRING_FUNC void *memcpy(void *restrict dst, const void *restrict src, size_t n)
{
    uint8_t *d = dst;
    const uint8_t *s = src;

    while (n > 0 && ((uintptr_t)d & 3) != 0) {
        *d++ = *s++;
        n--;
    }

    uint32_t *dw = (uint32_t *)d;
    const unaligned_u32_t *sw = (const unaligned_u32_t *)s;

    while (n >= 16) {
        uint32_t a = sw[0];
        uint32_t b = sw[1];
        uint32_t c = sw[2];
        uint32_t e = sw[3];
        dw[0] = a;
        dw[1] = b;
        dw[2] = c;
        dw[3] = e;
        dw += 4;
        sw += 4;
        n -= 16;
    }
    while (n >= 4) {
        *dw++ = *sw++;
        n -= 4;
    }

    d = (uint8_t *)dw;
    s = (const uint8_t *)sw;
    while (n > 0) {
        *d++ = *s++;
        n--;
    }

    return dst;
}

ZSW_STRING_FUNC void *memmove(void *dst, const void *src, size_t n)
{
    uint8_t *d = dst;
    const uint8_t *s = src;

    if (d <= s || d >= s + n) {
        // No overlap that a forward copy would overwrite before reading.
        while (n > 0 && ((uintptr_t)d & 3) != 0) {
            *d++ = *s++;
            n--;
        }
        uint32_t *dw = (uint32_t *)d;
        const unaligned_u32_t *sw = (const unaligned_u32_t *)s;
        while (n >= 4) {
            *dw++ = *sw++;
            n -= 4;
        }
        d = (uint8_t *)dw;
        s = (const uint8_t *)sw;
        while (n > 0) {
            *d++ = *s++;
            n--;
        }
    } else {
        d += n;
        s += n;
        while (n > 0 && ((uintptr_t)d & 3) != 0) {
            *--d = *--s;
            n--;
        }
        uint32_t *dw = (uint32_t *)d;
        const unaligned_u32_t *sw = (const unaligned_u32_t *)s;
        while (n >= 4) {
            *--dw = *--sw;
            n -= 4;
        }
        d = (uint8_t *)dw;
        s = (const uint8_t *)sw;
        while (n > 0) {
            *--d = *--s;
            n--;
        }
    }

    return dst;
}

ZSW_STRING_FUNC void *memset(void *dst, int c, size_t n)
{
    uint8_t *d = dst;
    uint8_t v = (uint8_t)c;

    while (n > 0 && ((uintptr_t)d & 3) != 0) {
        *d++ = v;
        n--;
    }

    uint32_t w = v * 0x01010101U;
    uint32_t *dw = (uint32_t *)d;

    while (n >= 16) {
        dw[0] = w;
        dw[1] = w;
        dw[2] = w;
        dw[3] = w;
        dw += 4;
        n -= 16;
    }
    while (n >= 4) {
        *dw++ = w;
        n -= 4;
    }

    d = (uint8_t *)dw;
    while (n > 0) {
        *d++ = v;
        n--;
    }

    return dst;
}
