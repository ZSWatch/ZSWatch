/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZCBOR_TAGS_H__
#define ZCBOR_TAGS_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Values defined by RFCs via www.iana.org/assignments/cbor-tags/cbor-tags.xhtml */
enum zcbor_tag {
	ZCBOR_TAG_TIME_TSTR =            0, ///! text string       [RFC8949]  Standard date/time string
	ZCBOR_TAG_TIME_NUM =             1, ///! integer or float  [RFC8949]  Epoch-based date/time
	ZCBOR_TAG_UBIGNUM_BSTR =         2, ///! byte string       [RFC8949]  Unsigned bignum
	ZCBOR_TAG_BIGNUM_BSTR =          3, ///! byte string       [RFC8949]  Negative bignum
	ZCBOR_TAG_DECFRAC_ARR =          4, ///! array             [RFC8949]  Decimal fraction
	ZCBOR_TAG_BIGFLOAT_ARR =         5, ///! array             [RFC8949]  Bigfloat
	ZCBOR_TAG_COSE_ENCRYPT0 =       16, ///! COSE_Encrypt0     [RFC9052]  COSE Single Recipient Encrypted Data Object
	ZCBOR_TAG_COSE_MAC0 =           17, ///! COSE_Mac0         [RFC9052]  COSE MAC w/o Recipients Object
	ZCBOR_TAG_COSE_SIGN1 =          18, ///! COSE_Sign1        [RFC9052]  COSE Single Signer Data Object
	ZCBOR_TAG_2BASE64URL =          21, ///! (any)             [RFC8949]  Expected conversion to base64url encoding
	ZCBOR_TAG_2BASE64 =             22, ///! (any)             [RFC8949]  Expected conversion to base64 encoding
	ZCBOR_TAG_2BASE16 =             23, ///! (any)             [RFC8949]  Expected conversion to base16 encoding
	ZCBOR_TAG_BSTR =                24, ///! byte string       [RFC8949]  Encoded CBOR data item
	ZCBOR_TAG_URI_TSTR =            32, ///! text string       [RFC8949]  URI
	ZCBOR_TAG_BASE64URL_TSTR =      33, ///! text string       [RFC8949]  base64url
	ZCBOR_TAG_BASE64_TSTR =         34, ///! text string       [RFC8949]  base64
	ZCBOR_TAG_REGEX =               35, ///! text string       [RFC7049]  Regular expression (UTF-8)
	ZCBOR_TAG_MIME_TSTR =           36, ///! text string       [RFC8949]  MIME message
	ZCBOR_TAG_LANG_TSTR =           38, ///! array             [RFC9290]  Text string with language tag
	ZCBOR_TAG_MULTI_DIM_ARR_R =     40, ///! array of arrays   [RFC8746]  Multi-dimensional array, row-major order
	ZCBOR_TAG_HOMOG_ARR =           41, ///! array             [RFC8746]  Homogeneous array
	ZCBOR_TAG_YANG_BITS =           42, ///! text string       [RFC9254]  YANG bits datatype; see Section 6.7.
	ZCBOR_TAG_YANG_ENUM =           43, ///! text string       [RFC9254]  YANG enumeration datatype; see Section 6.6.
	ZCBOR_TAG_YANG_IDENTITYREF =    44, ///! uint/tstr         [RFC9254]  YANG identityref datatype; see Section 6.10.
	ZCBOR_TAG_YANG_INSTANCE_ID =    45, ///! uint/tstr/array   [RFC9254]  YANG instance-identifier datatype; see Section 6.13.
	ZCBOR_TAG_SID =                 46, ///! uint              [RFC9254]  YANG Schema Item iDentifier (sid); see Section 3.2.
	ZCBOR_TAG_IPV4 =                52, ///! bstr or array     [RFC9164]  IPv4
	ZCBOR_TAG_IPV6 =                54, ///! bstr or array     [RFC9164]  IPv6
	ZCBOR_TAG_CWT =                 61, ///! CWT               [RFC8392]  CBOR Web Token
	ZCBOR_TAG_TYPED_ARR_U8 =        64, ///! byte string       [RFC8746]  uint8 Typed Array
	ZCBOR_TAG_TYPED_ARR_U16_BE =    65, ///! byte string       [RFC8746]  uint16, big endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_U32_BE =    66, ///! byte string       [RFC8746]  uint32, big endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_U64_BE =    67, ///! byte string       [RFC8746]  uint64, big endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_U8_CA =     68, ///! byte string       [RFC8746]  uint8 Typed Array, clamped arithmetic
	ZCBOR_TAG_TYPED_ARR_U16_LE =    69, ///! byte string       [RFC8746]  uint16, little endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_U32_LE =    70, ///! byte string       [RFC8746]  uint32, little endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_U64_LE =    71, ///! byte string       [RFC8746]  uint64, little endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_S8 =        72, ///! byte string       [RFC8746]  sint8 Typed Array
	ZCBOR_TAG_TYPED_ARR_S16_BE =    73, ///! byte string       [RFC8746]  sint16, big endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_S32_BE =    74, ///! byte string       [RFC8746]  sint32, big endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_S64_BE =    75, ///! byte string       [RFC8746]  sint64, big endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_S16_LE =    77, ///! byte string       [RFC8746]  sint16, little endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_S32_LE =    78, ///! byte string       [RFC8746]  sint32, little endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_S64_LE =    79, ///! byte string       [RFC8746]  sint64, little endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_F16_BE =    80, ///! byte string       [RFC8746]  IEEE 754 binary16, big endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_F32_BE =    81, ///! byte string       [RFC8746]  IEEE 754 binary32, big endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_F64_BE =    82, ///! byte string       [RFC8746]  IEEE 754 binary64, big endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_F128_BE =   83, ///! byte string       [RFC8746]  IEEE 754 binary128, big endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_F16_LE =    84, ///! byte string       [RFC8746]  IEEE 754 binary16, little endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_F32_LE =    85, ///! byte string       [RFC8746]  IEEE 754 binary32, little endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_F64_LE =    86, ///! byte string       [RFC8746]  IEEE 754 binary64, little endian, Typed Array
	ZCBOR_TAG_TYPED_ARR_F128_LE =   87, ///! byte string       [RFC8746]  IEEE 754 binary128, little endian, Typed Array
	ZCBOR_TAG_COSE_ENCRYPT =        96, ///! COSE_Encrypt      [RFC9052]  COSE Encrypted Data Object
	ZCBOR_TAG_COSE_MAC =            97, ///! COSE_Mac          [RFC9052]  COSE MACed Data Object
	ZCBOR_TAG_COSE_SIGN =           98, ///! COSE_Sign         [RFC9052]  COSE Signed Data Object
	ZCBOR_TAG_EPOCH_DAYS =         100, ///! integer           [RFC8943]  Number of days since the epoch date 1970-01-01
	ZCBOR_TAG_REL_OID_BER_SDNV =   110, ///! bstr/array/map    [RFC9090]  relative object identifier (BER encoding); SDNV [RFC6256] sequence
	ZCBOR_TAG_OID_BER =            111, ///! bstr/array/map    [RFC9090]  object identifier (BER encoding)
	ZCBOR_TAG_PEN_REL_OID_BER =    112, ///! bstr/array/map    [RFC9090]  object identifier (BER encoding), relative to 1.3.6.1.4.1
	ZCBOR_TAG_DOTS_SIG_CHAN_OBJ =  271, ///! DOTS sig chan obj [RFC9132]  DDoS Open Threat Signaling (DOTS) signal channel object
	ZCBOR_TAG_FULL_DATE_STR =     1004, ///! tstr (UTF-8)      [RFC8943]  Full-date string
	ZCBOR_TAG_MULTI_DIM_ARR_C =   1040, ///! array of arrays   [RFC8746]  Multi-dimensional array, column-major order
	ZCBOR_TAG_CBOR =             55799, ///! (any)             [RFC8949]  Self-described CBOR
	ZCBOR_TAG_CBOR_SEQ_FILE =    55800, ///! tagged bstr       [RFC9277]  indicates that the file contains CBOR Sequences
	ZCBOR_TAG_CBOR_FILE_LABEL =  55801, ///! tagged bstr       [RFC9277]  indicates that the file starts with a CBOR-Labeled Non-CBOR Data label.
	ZCBOR_TAG_COAP_CT =     1668546817, ///! bstr or (any)     [RFC9277]  Start of range: the representation of content-format ct < 65025 is indicated by tag number TN(ct) = 0x63740101 + (ct / 255) * 256 + ct % 255
	ZCBOR_TAG_COAP_CT_END = 1668612095, ///! bstr or (any)     [RFC9277]  End of range:   the representation of content-format ct < 65025 is indicated by tag number TN(ct) = 0x63740101 + (ct / 255) * 256 + ct % 255
};


#ifdef __cplusplus
}
#endif

#endif /* ZCBOR_TAGS_H__ */
