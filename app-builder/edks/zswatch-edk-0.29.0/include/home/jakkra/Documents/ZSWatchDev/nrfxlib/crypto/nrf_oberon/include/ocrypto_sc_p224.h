/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef OCRYPTO_SC_P224_H
#define OCRYPTO_SC_P224_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**@cond */

// P-224 scalar modulo group order
typedef struct {
    uint32_t w[7]; // little endian
} ocrypto_sc_p224;


// modulo P-224 number
typedef struct {
  uint32_t w[7]; // little endian
} ocrypto_mod_p224;
/**@endcond */

// scalar to 28 byte value
void ocrypto_sc_p224_to28bytes(uint8_t r[28], const ocrypto_sc_p224 *x);

// 28 byte value to scalar
// return 0 if x < group order
int ocrypto_sc_p224_from28bytes(ocrypto_sc_p224 *r, const uint8_t x[28]);

// reduce 36 bytes to range 0 < r < group order
void ocrypto_sc_p224_from36bytes(ocrypto_sc_p224 *r, const uint8_t x[36]);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef OCRYPTO_SC_P224_H */
