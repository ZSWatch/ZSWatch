/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2024, STMicroelectronics
 *
 */

#include <string.h>

/**
 * @internal
 *
 * @brief Copies a string to a destination buffer with size limitation and returns the length of
 *        the destination string.
 *
 * This function copies up to `s_size - 1` characters from the source string `src`
 * to the destination buffer `dst`, ensuring that the destination buffer is
 * null-terminated. The function returns the length of the `dst` string.
 * If the length of `src` string is greater than or equal to `d_size`, the destination
 * buffer will be truncated.
 *
 * @param dst    Destination buffer where the string will be copied.
 * @param d_size Size of the destination buffer.
 * @param src    Source string to be copied.
 * @param s_size Size of the source buffer.
 * @return       The length of the string contained in the `dst` buffer.
 *
 * @note If the size of the destination buffer is 0, the function does not copy any characters and
 *       the destination buffer is not null-terminated.
 * @note The function ensures that the destination buffer is always null-terminated if `size` is
 *       greater than 0.
 * @note The function ensures that no data is read past the end of the 'src' buffer.
 */
size_t safe_strcpy(char *dst, size_t d_size, const char *src, size_t s_size);

