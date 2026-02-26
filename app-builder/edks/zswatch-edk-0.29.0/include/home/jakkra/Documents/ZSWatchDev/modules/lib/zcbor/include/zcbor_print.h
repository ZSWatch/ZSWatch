/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZCBOR_PRINT_H__
#define ZCBOR_PRINT_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef ZCBOR_PRINT_FUNC
#include <stdio.h>
#define zcbor_do_print(...) printf(__VA_ARGS__)
#else
#define zcbor_do_print(...) ZCBOR_PRINT_FUNC(__VA_ARGS__)
#endif

#ifdef ZCBOR_VERBOSE
#define zcbor_trace_raw(state) (zcbor_do_print("rem: %zu, cur: 0x%x, ec: 0x%zx, err: %d",\
	(size_t)state->payload_end - (size_t)state->payload, *state->payload, state->elem_count, \
	state->constant_state ? state->constant_state->error : 0))
#define zcbor_trace(state, appendix) do { \
	zcbor_trace_raw(state); \
	zcbor_do_print(", %s\n", appendix); \
} while(0)
#define zcbor_trace_file(state) do { \
	zcbor_trace_raw(state); \
	zcbor_do_print(", %s:%d\n", __FILE__, __LINE__); \
} while(0)

#define zcbor_log_assert(expr, ...) \
do { \
	zcbor_do_print("ASSERTION \n  \"" #expr \
		"\"\nfailed at %s:%d with message:\n  ", \
		__FILE__, __LINE__); \
	zcbor_do_print(__VA_ARGS__);\
} while(0)
#define zcbor_log(...) zcbor_do_print(__VA_ARGS__)
#else
#define zcbor_trace(state, appendix)
#define zcbor_trace_file(state) ((void)state)
#define zcbor_log_assert(...)
#define zcbor_log(...)
#endif

#ifdef ZCBOR_ASSERTS
#define zcbor_assert(expr, ...) \
do { \
	if (!(expr)) { \
		zcbor_log_assert(expr, __VA_ARGS__); \
		ZCBOR_FAIL(); \
	} \
} while(0)
#define zcbor_assert_state(expr, ...) \
do { \
	if (!(expr)) { \
		zcbor_log_assert(expr, __VA_ARGS__); \
		ZCBOR_ERR(ZCBOR_ERR_ASSERTION); \
	} \
} while(0)
#else
#define zcbor_assert(expr, ...)
#define zcbor_assert_state(expr, ...)
#endif

void zcbor_print_compare_lines(const uint8_t *str1, const uint8_t *str2, size_t size);

void zcbor_print_compare_strings(const uint8_t *str1, const uint8_t *str2, size_t size);

void zcbor_print_compare_strings_diff(const uint8_t *str1, const uint8_t *str2, size_t size);

const char *zcbor_error_str(int error);

void zcbor_print_error(int error);

#ifdef __cplusplus
}
#endif

#endif /* ZCBOR_PRINT_H__ */
