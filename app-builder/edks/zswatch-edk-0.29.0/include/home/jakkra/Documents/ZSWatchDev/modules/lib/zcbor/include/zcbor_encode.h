/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZCBOR_ENCODE_H__
#define ZCBOR_ENCODE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "zcbor_common.h"

#ifdef __cplusplus
extern "C" {
#endif


/** The zcbor_encode library provides functions for encoding CBOR data elements.
 *
 * See The README for an introduction to CBOR, including the meaning of pint,
 * nint, bstr etc.
 */


/** See @ref zcbor_new_state() */
void zcbor_new_encode_state(zcbor_state_t *state_array, size_t n_states,
		uint8_t *payload, size_t payload_len, size_t elem_count);

/** Convenience macro for declaring and initializing an encoding state with backups.
 *
 *  This gives you a state variable named @p name. The variable functions like
 *  a pointer.
 *
 *  @param[in]  name          The name of the new state variable.
 *  @param[in]  num_backups   The number of backup slots to keep in the state.
 *  @param[in]  payload       The payload to work on.
 *  @param[in]  payload_size  The size (in bytes) of @p payload.
 *  @param[in]  elem_count    The starting elem_count (typically 1).
 */
#define ZCBOR_STATE_E(name, num_backups, payload, payload_size, elem_count) \
zcbor_state_t name[((num_backups) + 2)]; \
do { \
	zcbor_new_encode_state(name, ZCBOR_ARRAY_SIZE(name), payload, payload_size, elem_count); \
} while(0)


/** The following applies to all _put and _encode functions listed directly below.
 *
 * The difference between _put and _encode is only in the argument type,
 * but when a @ref zcbor_encoder_t is needed, such as for  @ref zcbor_multi_encode,
 * the _encode variant must be used.
 *
 * @param[inout] state  The current state of the encoding.
 * @param[in]    input  The value to encode.
 *
 * @retval true   Everything is ok.
 * @retval false  If the payload is exhausted. Or an unexpected error happened.
 *                Use zcbor_peek_error() to see the error code.
 */
bool zcbor_int32_put(zcbor_state_t *state, int32_t input); /* pint/nint */
bool zcbor_int64_put(zcbor_state_t *state, int64_t input); /* pint/nint */
bool zcbor_uint32_put(zcbor_state_t *state, uint32_t input); /* pint */
bool zcbor_uint64_put(zcbor_state_t *state, uint64_t input); /* pint */
bool zcbor_size_put(zcbor_state_t *state, size_t input); /* pint */
bool zcbor_tag_put(zcbor_state_t *state, uint32_t tag); /* CBOR tag */
bool zcbor_bool_put(zcbor_state_t *state, bool input); /* boolean CBOR simple value */
bool zcbor_nil_put(zcbor_state_t *state, const void *unused); /* 'nil' CBOR simple value */
bool zcbor_undefined_put(zcbor_state_t *state, const void *unused); /* 'undefined' CBOR simple value */
bool zcbor_float16_put(zcbor_state_t *state, float input); /* IEEE754 float16 */
bool zcbor_float16_bytes_put(zcbor_state_t *state, uint16_t input); /* IEEE754 float16 raw bytes */
bool zcbor_float32_put(zcbor_state_t *state, float input); /* IEEE754 float32 */
bool zcbor_float64_put(zcbor_state_t *state, double input); /* IEEE754 float64 */

bool zcbor_int32_encode(zcbor_state_t *state, const int32_t *input); /* pint/nint */
bool zcbor_int64_encode(zcbor_state_t *state, const int64_t *input); /* pint/nint */
bool zcbor_uint32_encode(zcbor_state_t *state, const uint32_t *input); /* pint */
bool zcbor_uint64_encode(zcbor_state_t *state, const uint64_t *input); /* pint */
bool zcbor_size_encode(zcbor_state_t *state, const size_t *input); /* pint */
bool zcbor_int_encode(zcbor_state_t *state, const void *input_int, size_t int_size);
bool zcbor_uint_encode(zcbor_state_t *state, const void *input_uint, size_t uint_size);
bool zcbor_bstr_encode(zcbor_state_t *state, const struct zcbor_string *input); /* bstr */
bool zcbor_tstr_encode(zcbor_state_t *state, const struct zcbor_string *input); /* tstr */
bool zcbor_tag_encode(zcbor_state_t *state, uint32_t *tag); /* CBOR tag. Note that zcbor_tag_encode()'s argument was changed to be a pointer. See also zcbor_tag_put(). */
bool zcbor_bool_encode(zcbor_state_t *state, const bool *input); /* boolean CBOR simple value */
bool zcbor_float16_encode(zcbor_state_t *state, const float *input); /* IEEE754 float16 */
bool zcbor_float16_bytes_encode(zcbor_state_t *state, const uint16_t *input); /* IEEE754 float16 raw bytes */
bool zcbor_float32_encode(zcbor_state_t *state, const float *input); /* IEEE754 float32 */
bool zcbor_float64_encode(zcbor_state_t *state, const double *input); /* IEEE754 float64 */

/** Encode a list/map header.
 *
 * The contents of the list/map can be encoded via subsequent function calls.
 * If ZCBOR_CANONICAL is defined, a state backup is created to keep track of the
 * element count.
 * When all members have been encoded, call @ref zcbor_list_end_encode /
 * @ref zcbor_map_end_encode to close the list/map.
 *
 * @param[inout] state    The current state of the encoding.
 * @param[in]    max_num  The maximum number of members in the list/map.
 *                        This serves as a size hint for the header. Must be
 *                        equal to the max_num provided to the corresponding
 *                        @ref zcbor_list_end_encode / @ref zcbor_map_end_encode
 *                        call.
 *                        Only used when ZCBOR_CANONICAL is defined.
 */
bool zcbor_list_start_encode(zcbor_state_t *state, size_t max_num);
bool zcbor_map_start_encode(zcbor_state_t *state, size_t max_num);

/** Encode the end of a list/map. Do some checks and deallocate backup.
 *
 *  - Default: Adds a list terminator (0xFF) to mark the
 *    end of the list/map.
 *  - If ZCBOR_CANONICAL is defined: Instead encodes the number of members in
 *    the list/map header. If the header ends up a different size than expected,
 *    the list/map contents are moved using memmove().
 *
 * Use @ref zcbor_list_map_end_force_encode to forcibly consume the backup if
 * something has gone wrong.
 *
 * @param[inout] state    The current state of the encoding.
 * @param[in]    max_num  The maximum number of members in the list/map. Must be
 *                        equal to the max_num provided to the corresponding
 *                        @ref zcbor_list_start_encode call.
 *                        Only used when ZCBOR_CANONICAL is defined.
 */
bool zcbor_list_end_encode(zcbor_state_t *state, size_t max_num);
bool zcbor_map_end_encode(zcbor_state_t *state, size_t max_num);
bool zcbor_list_map_end_force_encode(zcbor_state_t *state);

/** Encode 0 or more elements with the same type and constraints.
 *
 * The encoded values are taken from the @p input array.
 *
 * The following is an example of encoding a list containing 3 INTS followed by
 * 0 to 2 bstrs:
 *
 * @code{c}
 *     uint32_t ints[3] = <initialize>;
 *     struct zcbor_string bstrs[2] = <initialize>;
 *     bool res;
 *
 *     res = zcbor_list_start_encode(state, 5);
 *     res = res && zcbor_multi_encode(3, zcbor_uint32_encode, state,
 *                  ints, sizeof(uint32_t));
 *     res = res && zcbor_multi_encode(2, zcbor_bstr_encode, state,
 *                  bstrs, sizeof(struct zcbor_string));
 *     res = res && zcbor_list_end_encode(state, 5);
 *     // check res
 * @endcode
 *
 * The @ref zcbor_encoder_t type is designed to be compatible with all single-
 * value encoder functions in this library, e.g. @ref zcbor_uint32_encode,
 * @ref zcbor_tstr_put, @ref zcbor_nil_put, etc. For _put() functions,
 * @p input will be used as a value instead of an array/pointer, so
 * @p input_len will determine how much the value changes for each call.
 * To encode the same value multiple times, use a @p input_len of 0.
 * This function can also be used with custom decoder functions, such as those
 * generated by the zcbor.py script, which for example encodes larger chunks of
 * the data at once.
 *
 * @param[in]  num_encode    The actual number of elements.
 * @param[in]  encoder       The encoder function to call under the hood. This
 *                           function will be called with the provided arguments
 *                           repeatedly until the function fails (returns false)
 *                           or until it has been called @p max_encode times.
 *                           The input pointer is moved @p input_len bytes for
 *                           each call to @p encoder, i.e. @p input refers to an
 *                           array of input variables.
 * @param[in]  input         Source of the encoded values. Must be an array of
 *                           at least @p max_encode elements.
 * @param[in]  input_len     The length of the input variables. Must be the
 *                           length of the individual elements in input.
 *
 * @retval true   If at least @p min_encode variables were correctly encoded.
 * @retval false  If @p encoder failed before having encoded @p min_encode
 *                values.
 */
bool zcbor_multi_encode(size_t num_encode, zcbor_encoder_t encoder,
		zcbor_state_t *state, const void *input, size_t result_len);

/** Works like @ref zcbor_multi_encode
 *
 * But first checks that @p num_encode is between @p min_encode and @p max_encode.
 */
bool zcbor_multi_encode_minmax(size_t min_encode, size_t max_encode,
		const size_t *num_encode, zcbor_encoder_t encoder,
		zcbor_state_t *state, const void *input, size_t input_len);


/* Supplementary string (bstr/tstr) encoding functions: */

/** Encode a char/uint8_t pointer as a bstr/tstr.
 *
 * @param[inout] state   The current state of the encoding.
 * @param[in]    str     The value to encode. A pointer to the string/array.
 *                       _term() uses strnlen(), so @p str must be null-terminated.
 *                       _lit() uses sizeof()-1, so @p str must be a (null-terminated) string literal.
 *                       _arr() uses sizeof(), so @p str must be a uint8_t array (not null-terminated).
 * @param[in]    len     (if present) The length of the string pointed to by @p str
 * @param[in]    maxlen  (if present) The maximum length of the string pointed to by @p str.
 *                       This value is passed to strnlen.
 */
bool zcbor_bstr_encode_ptr(zcbor_state_t *state, const char *str, size_t len);
bool zcbor_tstr_encode_ptr(zcbor_state_t *state, const char *str, size_t len);
bool zcbor_bstr_put_term(zcbor_state_t *state, char const *str, size_t maxlen);
bool zcbor_tstr_put_term(zcbor_state_t *state, char const *str, size_t maxlen);
#define zcbor_bstr_put_lit(state, str) zcbor_bstr_encode_ptr(state, str, sizeof(str) - 1)
#define zcbor_tstr_put_lit(state, str) zcbor_tstr_encode_ptr(state, str, sizeof(str) - 1)
#define zcbor_bstr_put_arr(state, str) zcbor_bstr_encode_ptr(state, str, sizeof(str))
#define zcbor_tstr_put_arr(state, str) zcbor_tstr_encode_ptr(state, str, sizeof(str))

/** Encode a bstr header.
 *
 * The rest of the string can be encoded as CBOR.
 * A state backup is created to keep track of the element count.
 * Call @ref zcbor_bstr_end_encode when done encoding the contents of the bstr.
 *
 * @param[inout] state   The current state of the encoding.
 *
 * @retval true   Header encoded correctly
 * @retval false  Header encoded incorrectly, or backup failed.
 */
bool zcbor_bstr_start_encode(zcbor_state_t *state);

/** Finalize encoding a CBOR-encoded bstr.
 *
 * This writes the final size of the bstr to the header.
 * Restore element count from backup.
 */
bool zcbor_bstr_end_encode(zcbor_state_t *state, struct zcbor_string *result);

#ifdef __cplusplus
}
#endif

#endif /* ZCBOR_ENCODE_H__ */
