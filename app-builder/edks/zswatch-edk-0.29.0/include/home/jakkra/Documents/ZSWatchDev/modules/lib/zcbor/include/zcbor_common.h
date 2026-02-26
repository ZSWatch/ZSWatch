/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZCBOR_COMMON_H__
#define ZCBOR_COMMON_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_tags.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ZCBOR_STRINGIFY_PRE(x) #x
#define ZCBOR_STRINGIFY(s) ZCBOR_STRINGIFY_PRE(s)

#define ZCBOR_VERSION_MAJOR 0
#define ZCBOR_VERSION_MINOR 9
#define ZCBOR_VERSION_BUGFIX 1

/** The version string with dots and not prefix. */
#define ZCBOR_VERSION_STR   ZCBOR_STRINGIFY(ZCBOR_VERSION_MAJOR) \
			"." ZCBOR_STRINGIFY(ZCBOR_VERSION_MINOR) \
			"." ZCBOR_STRINGIFY(ZCBOR_VERSION_BUGFIX)

/** Monotonically increasing integer representing the version. */
#define ZCBOR_VERSION    ((ZCBOR_VERSION_MAJOR << 24) \
			+ (ZCBOR_VERSION_MINOR << 16) \
			+ (ZCBOR_VERSION_BUGFIX << 8))

/** Convenience type that allows pointing to strings directly inside the payload
 *  without the need to copy out.
 */
struct zcbor_string {
	const uint8_t *value;
	size_t len;
};


/** Type representing a string fragment.
 *
 * Don't modify any member variables, or subsequent calls may fail.
**/
struct zcbor_string_fragment {
	struct zcbor_string fragment; ///! Location and length of the fragment.
	size_t offset;                ///! The offset in the full string at which this fragment belongs.
	size_t total_len;             ///! The total length of the string this fragment is a part of.
};


/** Size to use in struct zcbor_string_fragment when the real size is unknown. */
#define ZCBOR_STRING_FRAGMENT_UNKNOWN_LENGTH SIZE_MAX

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) < (b)) ? (b) : (a))
#endif

#ifndef ZCBOR_ARRAY_SIZE
#define ZCBOR_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

/* Endian-dependent offset of smaller integer in a bigger one. */
#ifdef ZCBOR_BIG_ENDIAN
#define ZCBOR_ECPY_OFFS(dst_len, src_len) ((dst_len) - (src_len))
#else
#define ZCBOR_ECPY_OFFS(dst_len, src_len) (0)
#endif /* ZCBOR_BIG_ENDIAN */

#if SIZE_MAX <= UINT64_MAX
/** The ZCBOR_SUPPORTS_SIZE_T will be defined if processing of size_t type variables directly
 * with zcbor_size_ functions is supported.
**/
#define ZCBOR_SUPPORTS_SIZE_T
#else
#warning "zcbor: Unsupported size_t encoding size"
#endif

struct zcbor_state_constant;

/** The zcbor_state_t structure is used for both encoding and decoding. */
typedef struct {
union {
	uint8_t *payload_mut;
	uint8_t const *payload; /**< The current place in the payload. Will be
	                             updated when an element is correctly
	                             processed. */
};
	uint8_t const *payload_bak; /**< Temporary backup of payload. */
	size_t elem_count; /**< The current element is part of a LIST or a MAP,
	                        and this keeps count of how many elements are
	                        expected. This will be checked before processing
	                        and decremented if the element is correctly
	                        processed. */
	uint8_t const *payload_end; /**< The end of the payload. This will be
	                                 checked against payload before
	                                 processing each element. */
	bool payload_moved; /**< Is set to true while the state is stored as a backup
	                         if @ref zcbor_update_state is called, since that function
	                         updates the payload_end of all backed-up states. */

/* This is the "decode state", the part of zcbor_state_t that is only used by zcbor_decode.c. */
struct {
	bool indefinite_length_array; /**< Is set to true if the decoder is currently
	                                   decoding the contents of an indefinite-
	                                   length array. */
	bool counting_map_elems; /**< Is set to true while the number of elements of the
	                              current map are being counted. */
#ifdef ZCBOR_MAP_SMART_SEARCH
	uint8_t *map_search_elem_state; /**< Optional flags to use when searching unordered
	                                     maps. If this is not NULL and map_elem_count
	                                     is non-zero, this consists of one flag per element
	                                     in the current map. The n-th bit can be set to 0
	                                     to indicate that the n-th element in the
	                                     map should not be searched. These are manipulated
	                                     via zcbor_elem_processed() or
	                                     zcbor_unordered_map_search(), and should not be
	                                     manipulated directly. */
#else
	size_t map_elems_processed; /**< The number of elements of an unordered map
	                                 that have been processed. */
#endif
	size_t map_elem_count; /**< Number of elements in the current unordered map.
	                            This also serves as the number of bits (not bytes)
	                            in the map_search_elem_state array (when applicable). */
} decode_state;
	struct zcbor_state_constant *constant_state; /**< The part of the state that is
	                                                  not backed up and duplicated. */
} zcbor_state_t;

struct zcbor_state_constant {
	zcbor_state_t *backup_list;
	size_t current_backup;
	size_t num_backups;
	int error;
#ifdef ZCBOR_STOP_ON_ERROR
	bool stop_on_error;
#endif
	bool enforce_canonical; /**< Fail when decoding if data is non-canonical.
	                             The default/initial value follows ZCBOR_CANONICAL */
	bool manually_process_elem; /**< Whether an (unordered map) element should be automatically
	                                 marked as processed when found via @ref zcbor_search_map_key. */
#ifdef ZCBOR_MAP_SMART_SEARCH
	uint8_t *map_search_elem_state_end; /**< The end of the @ref map_search_elem_state buffer. */
#endif
};

#ifdef ZCBOR_CANONICAL
#define ZCBOR_ENFORCE_CANONICAL_DEFAULT true
#else
#define ZCBOR_ENFORCE_CANONICAL_DEFAULT false
#endif

#define ZCBOR_ENFORCE_CANONICAL(state) (state->constant_state \
	? state->constant_state->enforce_canonical : ZCBOR_ENFORCE_CANONICAL_DEFAULT)

#define ZCBOR_MANUALLY_PROCESS_ELEM_DEFAULT false

#define ZCBOR_MANUALLY_PROCESS_ELEM(state) (state->constant_state \
	? state->constant_state->manually_process_elem : ZCBOR_MANUALLY_PROCESS_ELEM_DEFAULT)

/** Function pointer type used with zcbor_multi_decode.
 *
 * This type is compatible with all decoding functions here and in the generated
 * code, except for zcbor_multi_decode.
 */
typedef bool(zcbor_encoder_t)(zcbor_state_t *, const void *);
typedef bool(zcbor_decoder_t)(zcbor_state_t *, void *);

/** Enumeration representing the major types available in CBOR.
 *
 * The major type is represented in the 3 first bits of the header byte.
 */
typedef enum
{
	ZCBOR_MAJOR_TYPE_PINT   = 0, ///! Positive Integer
	ZCBOR_MAJOR_TYPE_NINT   = 1, ///! Negative Integer
	ZCBOR_MAJOR_TYPE_BSTR   = 2, ///! Byte String
	ZCBOR_MAJOR_TYPE_TSTR   = 3, ///! Text String
	ZCBOR_MAJOR_TYPE_LIST   = 4, ///! List
	ZCBOR_MAJOR_TYPE_MAP    = 5, ///! Map
	ZCBOR_MAJOR_TYPE_TAG    = 6, ///! Semantic Tag
	ZCBOR_MAJOR_TYPE_SIMPLE = 7, ///! Simple values and floats
} zcbor_major_type_t;

/** Extract the major type, i.e. the first 3 bits of the header byte. */
#define ZCBOR_MAJOR_TYPE(header_byte) ((zcbor_major_type_t)(((header_byte) >> 5) & 0x7))

/** Extract the additional info, i.e. the last 5 bits of the header byte. */
#define ZCBOR_ADDITIONAL(header_byte) ((header_byte) & 0x1F)

/** Convenience macro for failing out of a decoding/encoding function.
*/
#define ZCBOR_FAIL() \
do {\
	zcbor_log("ZCBOR_FAIL "); \
	zcbor_trace_file(state); \
	return false; \
} while(0)

#define ZCBOR_FAIL_IF(expr) \
do {\
	if (expr) { \
		zcbor_log("ZCBOR_FAIL_IF(" #expr ") "); \
		ZCBOR_FAIL(); \
	} \
} while(0)

#define ZCBOR_ERR(err) \
do { \
	zcbor_log("ZCBOR_ERR(%d) ", err); \
	zcbor_error(state, err); \
	ZCBOR_FAIL(); \
} while(0)

#define ZCBOR_ERR_IF(expr, err) \
do {\
	if (expr) { \
		zcbor_log("ZCBOR_ERR_IF(" #expr ", %d) ", err); \
		ZCBOR_ERR(err); \
	} \
} while(0)

#define ZCBOR_CHECK_PAYLOAD() \
	ZCBOR_ERR_IF(state->payload >= state->payload_end, ZCBOR_ERR_NO_PAYLOAD)

#ifdef ZCBOR_STOP_ON_ERROR
#define ZCBOR_CHECK_ERROR()  \
do { \
	if (!zcbor_check_error(state)) { \
		ZCBOR_FAIL(); \
	} \
} while(0)
#else
#define ZCBOR_CHECK_ERROR()
#endif

#define ZCBOR_VALUE_IN_HEADER 23 ///! Values below this are encoded directly in the header.
#define ZCBOR_VALUE_IS_1_BYTE 24 ///! The next 1 byte contains the value.
#define ZCBOR_VALUE_IS_2_BYTES 25 ///! The next 2 bytes contain the value.
#define ZCBOR_VALUE_IS_4_BYTES 26 ///! The next 4 bytes contain the value.
#define ZCBOR_VALUE_IS_8_BYTES 27 ///! The next 8 bytes contain the value.
#define ZCBOR_VALUE_IS_INDEFINITE_LENGTH 31 ///! The list or map has indefinite length, and will instead be terminated by a 0xFF token.

#define ZCBOR_BOOL_TO_SIMPLE ((uint8_t)20) ///! In CBOR, false/true have the values 20/21
#define ZCBOR_NIL_VAL ((uint8_t)22)
#define ZCBOR_UNDEF_VAL ((uint8_t)23)

#define ZCBOR_IS_FLOAT(header_byte) (((header_byte) >= 0xF9) && ((header_byte) <= 0xFB))

#define ZCBOR_FLAG_RESTORE 1UL ///! Restore from the backup. Overwrite the current state with the state from the backup.
#define ZCBOR_FLAG_CONSUME 2UL ///! Consume the backup. Remove the backup from the stack of backups.
#define ZCBOR_FLAG_KEEP_PAYLOAD 4UL ///! Keep the pre-restore payload after restoring.
#define ZCBOR_FLAG_KEEP_DECODE_STATE 8UL ///! Keep the pre-restore decode state (everything only used for decoding)

#define ZCBOR_SUCCESS 0
#define ZCBOR_ERR_NO_BACKUP_MEM 1
#define ZCBOR_ERR_NO_BACKUP_ACTIVE 2
#define ZCBOR_ERR_LOW_ELEM_COUNT 3
#define ZCBOR_ERR_HIGH_ELEM_COUNT 4
#define ZCBOR_ERR_INT_SIZE 5
#define ZCBOR_ERR_FLOAT_SIZE 6
#define ZCBOR_ERR_ADDITIONAL_INVAL 7 ///! > 27
#define ZCBOR_ERR_NO_PAYLOAD 8
#define ZCBOR_ERR_PAYLOAD_NOT_CONSUMED 9
#define ZCBOR_ERR_WRONG_TYPE 10
#define ZCBOR_ERR_WRONG_VALUE 11
#define ZCBOR_ERR_WRONG_RANGE 12
#define ZCBOR_ERR_ITERATIONS 13
#define ZCBOR_ERR_ASSERTION 14
#define ZCBOR_ERR_PAYLOAD_OUTDATED 15 ///! Because of a call to @ref zcbor_update_state
#define ZCBOR_ERR_ELEM_NOT_FOUND 16
#define ZCBOR_ERR_MAP_MISALIGNED 17
#define ZCBOR_ERR_ELEMS_NOT_PROCESSED 18
#define ZCBOR_ERR_NOT_AT_END 19
#define ZCBOR_ERR_MAP_FLAGS_NOT_AVAILABLE 20
#define ZCBOR_ERR_INVALID_VALUE_ENCODING 21 ///! When ZCBOR_CANONICAL is defined, and the incoming data is not encoded with minimal length, or uses indefinite length array.
#define ZCBOR_ERR_CONSTANT_STATE_MISSING 22
#define ZCBOR_ERR_UNKNOWN 31

/** The largest possible elem_count. */
#define ZCBOR_MAX_ELEM_COUNT SIZE_MAX

/** Initial value for elem_count for when it just needs to be large. */
#define ZCBOR_LARGE_ELEM_COUNT (ZCBOR_MAX_ELEM_COUNT - 15)


/** Take a backup of the current state. Overwrite the current elem_count. */
bool zcbor_new_backup(zcbor_state_t *state, size_t new_elem_count);

/** Consult the most recent backup. In doing so, check whether elem_count is
 *  less than or equal to max_elem_count.
 *  Also, take action based on the flags (See ZCBOR_FLAG_*).
 */
bool zcbor_process_backup(zcbor_state_t *state, uint32_t flags, size_t max_elem_count);

/** Convenience function for starting encoding/decoding of a union.
 *
 *  That is, for attempting to encode, or especially decode, multiple options.
 *  Makes a new backup.
 */
bool zcbor_union_start_code(zcbor_state_t *state);

/** Convenience function before encoding/decoding one element of a union.
 *
 *  Call this before attempting each option.
 *  Restores the backup, without consuming it.
 */
bool zcbor_union_elem_code(zcbor_state_t *state);

/** Convenience function before encoding/decoding one element of a union.
 *
 *  Consumes the backup without restoring it.
 */
bool zcbor_union_end_code(zcbor_state_t *state);

/** Initialize a state with backups.
 *  As long as n_states is more than 1, one of the states in the array is used
 *  as a struct zcbor_state_constant object.
 *  If there is no struct zcbor_state_constant (n_states == 1), error codes are
 *  not available.
 *  This means that you get a state with (n_states - 2) backups.
 *  payload, payload_len, elem_count, and elem_state are used to initialize the first state.
 *  The elem_state is only needed for unordered maps, when ZCBOR_MAP_SMART_SEARCH is enabled.
 *  It is ignored otherwise.
 */
void zcbor_new_state(zcbor_state_t *state_array, size_t n_states,
		const uint8_t *payload, size_t payload_len, size_t elem_count,
		uint8_t *elem_state, size_t elem_state_bytes);

/** Do boilerplate entry function procedure.
 *  Initialize states, call function, and check the result.
 */
int zcbor_entry_function(const uint8_t *payload, size_t payload_len,
	void *result, size_t *payload_len_out, zcbor_state_t *state, zcbor_decoder_t func,
	size_t n_states, size_t elem_count);

#ifdef ZCBOR_STOP_ON_ERROR
/** Check stored error and fail if present, but only if stop_on_error is true.
 *
 * @retval true   No error found
 * @retval false  An error was found
 */
static inline bool zcbor_check_error(const zcbor_state_t *state)
{
	struct zcbor_state_constant *cs = state->constant_state;
	return !(cs && cs->stop_on_error && cs->error);
}
#endif

/** Return the current error state, replacing it with SUCCESS. */
static inline int zcbor_pop_error(zcbor_state_t *state)
{
	if (!state->constant_state) {
		return ZCBOR_SUCCESS;
	}
	int err = state->constant_state->error;

	state->constant_state->error = ZCBOR_SUCCESS;
	return err;
}

/** Look at current error state without altering it */
static inline int zcbor_peek_error(const zcbor_state_t *state)
{
	if (!state->constant_state) {
		return ZCBOR_SUCCESS;
	} else {
		return state->constant_state->error;
	}
}

/** Write the provided error to the error state. */
static inline void zcbor_error(zcbor_state_t *state, int err)
{
#ifdef ZCBOR_STOP_ON_ERROR
	if (zcbor_check_error(state))
#endif
	{
		if (state->constant_state) {
			state->constant_state->error = err;
		}
	}
}

/** Whether the current payload is exhausted. */
static inline bool zcbor_payload_at_end(const zcbor_state_t *state)
{
	return (state->payload == state->payload_end);
}

/** Update the current payload pointer (and payload_end).
 *
 *  For use when the payload is divided into multiple chunks.
 *
 *  This function also updates all backups to the new payload_end.
 *  This sets a flag so that @ref zcbor_process_backup fails if a backup is
 *  processed with the flag @ref ZCBOR_FLAG_RESTORE, but without the flag
 *  @ref ZCBOR_FLAG_KEEP_PAYLOAD since this would cause an invalid state.
 *
 *  @param[inout]  state              The current state, will be updated with
 *                                    the new payload pointer.
 *  @param[in]     payload            The new payload chunk.
 *  @param[in]     payload_len        The length of the new payload chunk.
 */
void zcbor_update_state(zcbor_state_t *state,
		const uint8_t *payload, size_t payload_len);

/** Check that the provided fragments are complete and in the right order.
 *
 *  If the total length is not known, the total_len can have the value
 *  @ref ZCBOR_STRING_FRAGMENT_UNKNOWN_LENGTH. If so, all fragments will be
 *  updated with the actual total length.
 *
 *  @param[in]  fragments      An array of string fragments. Cannot be NULL.
 *  @param[in]  num_fragments  The number of fragments in @p fragments.
 *
 *  @retval  true   If the fragments are in the right order, and there are no
 *                  fragments missing.
 *  @retval  false  If not all fragments have the same total_len, or gaps are
 *                  found, or if any fragment value is NULL.
 */
bool zcbor_validate_string_fragments(struct zcbor_string_fragment *fragments,
		size_t num_fragments);

/** Assemble the fragments into a single string.
 *
 *  The fragments are copied in the order they appear, without regard for
 *  offset or total_len. To ensure that the fragments are correct, first
 *  validate with @ref zcbor_validate_string_fragments.
 *
 *  @param[in]     fragments      An array of string fragments. Cannot be NULL.
 *  @param[in]     num_fragments  The number of fragments in @p fragments.
 *  @param[out]    result         The buffer to place the assembled string into.
 *  @param[inout]  result_len     In: The length of the @p result.
 *                                Out: The length of the assembled string.
 *
 *  @retval  true   On success.
 *  @retval  false  If the assembled string would be larger than the buffer.
 *                  The buffer might still be written to.
 */
bool zcbor_splice_string_fragments(struct zcbor_string_fragment *fragments,
		size_t num_fragments, uint8_t *result, size_t *result_len);

/** Compare two struct zcbor_string instances.
 *
 *  @param[in] str1  A string
 *  @param[in] str2  A string to compare to @p str1
 *
 *  @retval true   if the strings are identical
 *  @retval false  if length or contents don't match, or one one or both strings is NULL.
 */
bool zcbor_compare_strings(const struct zcbor_string *str1,
		const struct zcbor_string *str2);

/** Calculate the length of a CBOR string, list, or map header.
 *
 *  This can be used to find the start of the CBOR object when you have a
 *  pointer to the start of the contents. The function assumes that the header
 *  will be the shortest it can be.
 *
 *  @param[in] num_elems  The number of elements in the string, list, or map.
 *
 *  @return  The length of the header in bytes (1-9).
 */
size_t zcbor_header_len(uint64_t value);

/** Like @ref zcbor_header_len but for integer of any size <= 8. */
size_t zcbor_header_len_ptr(const void *const value, size_t value_len);

/** If a string (header + payload) is encoded into the rest of the payload, how long would it be?
 *
 *  Note that a string with this length doesn't necessarily fill the rest of the
 *  payload. For some payload lengths, e.g. 25, it's impossible to encode a
 *  string of that total length.
 *
 *  @param[in] state  The current state.
 *
 *  @return  The length of the string (payload, not including header) in bytes.
 */
size_t zcbor_remaining_str_len(zcbor_state_t *state);

/** Convert a float16 value to float32.
 *
 *  @param[in] input  The float16 value stored in a uint16_t.
 *
 *  @return  The resulting float32 value.
 */
float zcbor_float16_to_32(uint16_t input);

/** Convert a float32 value to float16.
 *
 *  @param[in] input  The float32 value.
 *
 *  @return  The resulting float16 value as a uint16_t.
 */
uint16_t zcbor_float32_to_16(float input);

#ifdef ZCBOR_MAP_SMART_SEARCH
static inline size_t zcbor_round_up(size_t x, size_t align)
{
	return (((x) + (align) - 1) / (align) * (align));
}

#define ZCBOR_BITS_PER_BYTE 8
/** Calculate the number of bytes needed to hold @p num_flags 1 bit flags
 */
static inline size_t zcbor_flags_to_bytes(size_t num_flags)
{
	return zcbor_round_up(num_flags, ZCBOR_BITS_PER_BYTE) / ZCBOR_BITS_PER_BYTE;
}

/** Calculate the number of zcbor_state_t instances needed to hold @p num_flags 1 bit flags
 */
static inline size_t zcbor_flags_to_states(size_t num_flags)
{
	return zcbor_round_up(num_flags, sizeof(zcbor_state_t) * ZCBOR_BITS_PER_BYTE)
			/ (sizeof(zcbor_state_t) * ZCBOR_BITS_PER_BYTE);
}

#define ZCBOR_FLAG_STATES(n_flags) zcbor_flags_to_states(n_flags)

#else
#define ZCBOR_FLAG_STATES(n_flags) 0
#endif

size_t strnlen(const char *, size_t);

#ifdef __cplusplus
}
#endif

#endif /* ZCBOR_COMMON_H__ */
