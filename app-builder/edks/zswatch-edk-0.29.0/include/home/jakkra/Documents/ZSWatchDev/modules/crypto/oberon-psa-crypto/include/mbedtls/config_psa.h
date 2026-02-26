/**
 * \file mbedtls/config_psa.h
 * \brief PSA crypto configuration options (set of defines)
 *
 *  This set of compile-time options takes settings defined in
 *  include/mbedtls/mbedtls_config.h and include/psa/crypto_config.h and uses
 *  those definitions to define symbols used in the library code.
 *
 *  Users and integrators should not edit this file, please edit
 *  include/mbedtls/mbedtls_config.h for MBEDTLS_XXX settings or
 *  include/psa/crypto_config.h for PSA_WANT_XXX settings.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

/*
 * NOTICE: This file has been modified by Oberon microsystems AG.
 */

#ifndef MBEDTLS_CONFIG_PSA_H
#define MBEDTLS_CONFIG_PSA_H

#include "psa/crypto_legacy.h"

#include "psa/crypto_adjust_config_synonyms.h"

#if defined(MBEDTLS_PSA_CRYPTO_CONFIG)

/* Require built-in implementations based on PSA requirements */

/* We need this to have a complete list of requirements
 * before we deduce what built-ins are required. */
#include "psa/crypto_adjust_config_key_pair_types.h"

#endif /* MBEDTLS_PSA_CRYPTO_CONFIG */

#include "psa/crypto_adjust_auto_enabled.h"

#include "psa/crypto_driver_config.h"

#endif /* MBEDTLS_CONFIG_PSA_H */
