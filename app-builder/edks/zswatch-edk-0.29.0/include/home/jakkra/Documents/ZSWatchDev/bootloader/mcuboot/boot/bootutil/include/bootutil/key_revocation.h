/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef H_KEY_REVOCATION_
#define H_KEY_REVOCATION_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BOOT_KEY_REVOKE_OK 0
#define BOOT_KEY_REVOKE_NOT_READY 1
#define BOOT_KEY_REVOKE_INVALID 2
#define BOOT_KEY_REVOKE_FAILED 2


void allow_revoke(void);

int revoke(void);

#ifdef __cplusplus
}
#endif

#endif
