/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2020 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


#ifndef OBERON_HELPERS_H
#define OBERON_HELPERS_H

#include "psa/crypto.h"

/**
 * Variable length constant time comparison.
 *
 * @param x      Memory region to compare with @p y.
 * @param y      Memory region to compare with @p x.
 * @param length Number of bytes to compare, @p length > 0.
 *
 * @returns Zero if @p x and @p y are equal, non-zero otherwise.
 */
int oberon_ct_compare(const void *x, const void *y, size_t length);
    
/**
 * Variable length constant time compare to zero.
 *
 * @param x      Memory region that will be compared.
 * @param length Number of bytes to compare, @p length > 0.
 *
 * @returns Zero if @p x is all zeroes, non-zero otherwise.
 */
int oberon_ct_compare_zero(const void *x, size_t length);

/**
 * Variable length bitwise xor.
 *
 * @param[out] r      Memory region to store the result.
 * @param      x      Memory region containing the first argument.
 * @param      y      Memory region containing the second argument.
 * @param      length Number of bytes in both arguments, @p length > 0.
 *
 * @remark @p r may be same as @p x or @p y.
 */
void oberon_xor(void *r, const void *x, const void *y, size_t length);


/**@name Synchronization primitives for randomness drivers.
*
* This group of functions is used to protect the global state of the
* randomness drivers from being accessed simultaneously by multiple threads.
*/
/**@{*/
/**
* Define this preprocessor symbol if synchronization is necessary.
*/
//#define OBERON_USE_MUTEX

/**
* The mutex state type.
*/
//struct oberon_mutex_type;

/**
* Mutex initialization.
*
* The mutex state @p mutex is initialized by this function.
*
* @param[out] mutex Mutex state.
*/
//void oberon_mutex_init(struct oberon_mutex_type *mutex);

/**
* Mutex lock.
*
* The mutex state @p mutex is locked by this function.
*
* @param   mutex Mutex state.
* @returns Zero if the function succeeds, non-zero otherwise.
*/
//int oberon_mutex_lock(struct oberon_mutex_type *mutex);

/**
* Mutex unlock.
*
* The mutex state @p mutex is unlocked by this function.
*
* @param   mutex Mutex state.
* @returns Zero if the function succeeds, non-zero otherwise.
*/
//int oberon_mutex_unlock(struct oberon_mutex_type *mutex);

/**
* Mutex unlock.
*
* The mutex state @p mutex is freed by this function.
*
* @param mutex Mutex state.
*/
//void oberon_mutex_free(struct oberon_mutex_type *mutex);
/**@}*/


#ifdef MBEDTLS_THREADING_C

#include "mbedtls/threading.h"

#define OBERON_USE_MUTEX           1
#define oberon_mutex_type          mbedtls_threading_mutex_t
#define oberon_mutex_init(mutex)   mbedtls_mutex_init(mutex)
#define oberon_mutex_lock(mutex)   mbedtls_mutex_lock(mutex)
#define oberon_mutex_unlock(mutex) mbedtls_mutex_unlock(mutex)
#define oberon_mutex_free(mutex)   mbedtls_mutex_free(mutex)

#endif /* MBEDTLS_THREADING_C */


/**
 * Generate random bytes.
 *
 * @param[out] output Output buffer for the generated data.
 * @param      size   Number of bytes to generate and output.
 *
 * @retval #PSA_SUCCESS
 * @retval #PSA_ERROR_NOT_SUPPORTED
 * @retval #PSA_ERROR_INSUFFICIENT_ENTROPY
 * @retval #PSA_ERROR_INSUFFICIENT_MEMORY
 * @retval #PSA_ERROR_COMMUNICATION_FAILURE
 * @retval #PSA_ERROR_HARDWARE_FAILURE
 * @retval #PSA_ERROR_CORRUPTION_DETECTED
 * @retval #PSA_ERROR_BAD_STATE
 */
//psa_status_t oberon_generate_random(uint8_t *output, size_t size);
#define oberon_generate_random(output, size) psa_generate_random(output, size)


#endif
