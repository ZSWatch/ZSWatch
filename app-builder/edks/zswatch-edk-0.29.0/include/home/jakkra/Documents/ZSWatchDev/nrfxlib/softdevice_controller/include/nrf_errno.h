/*
 * Copyright (c) Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Nordic-5-Clause
 */

#ifndef NRF_ERRNO_H__
#define NRF_ERRNO_H__

/**
 * @file
 * @defgroup nrf_errno_ble Integer values for errno
 * @ingroup sdc
 *
 * Used by system calls to indicates the latest error.
 * @{
 */
#ifdef __cplusplus
extern "C" {
#endif

#define NRF_EPERM            1
#define NRF_ENOENT           2
#define NRF_EIO              5
#define NRF_EBADF            9
#define NRF_ENOMEM          12
#define NRF_EACCES          13
#define NRF_EFAULT          14
#define NRF_EINVAL          22
#define NRF_EMFILE          24
#define NRF_EAGAIN          35
#define NRF_EPROTOTYPE      41
#define NRF_ENOPROTOOPT     42
#define NRF_EPROTONOSUPPORT 43
#define NRF_ESOCKTNOSUPPORT 44
#define NRF_EOPNOTSUPP      45
#define NRF_EAFNOSUPPORT    47
#define NRF_EADDRINUSE      48
#define NRF_ENETDOWN        50
#define NRF_ENETUNREACH     51
#define NRF_ECONNRESET      54
#define NRF_EISCONN         56
#define NRF_ENOTCONN        57
#define NRF_ETIMEDOUT       60
#define NRF_ENOBUFS         105

#define NRF_EHOSTDOWN       112 //< Host is down.
#define NRF_EINPROGRESS     115 //< Operation in progress.
#define NRF_ECANCELED       125 //< Operation canceled.

#define NRF_ENOKEY          126
#define NRF_EKEYEXPIRED     127
#define NRF_EKEYREVOKED     128
#define NRF_EKEYREJECTED    129

#ifdef __cplusplus
}
#endif

/** @} end of nrf_errno_ble */

#endif /* NRF_ERRNO_H__ */
