/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_BOOT_SERIAL_EXTENTIONS_
#define H_BOOT_SERIAL_EXTENTIONS_

#include <zephyr/kernel.h>
#include <zephyr/sys/util_macro.h>
#include <zephyr/sys/iterable_sections.h>

/**
 * Callback handler prototype for boot serial extensions.
 *
 * @param[in]   hdr       MCUmgr header
 * @param[in]   buffer    Buffer with first MCUmgr message
 * @param[in]   len       Length of data in buffer
 * @param[out]  cs        Response
 *
 * @return      MGMT_ERR_ENOTSUP to run other handlers, other MGMT_ERR_* value
 *              when expected handler has ran.
 */
typedef int (*bs_custom_handler_cb)(const struct nmgr_hdr *hdr,
                                    const char *buffer, int len,
                                    zcbor_state_t *cs);

struct mcuboot_bs_custom_handlers {
    const bs_custom_handler_cb handler;
};

/* Used to create an iterable section containing a boot serial handler
 * function
 */
#define MCUMGR_HANDLER_DEFINE(name, _handler)                          \
        STRUCT_SECTION_ITERABLE(mcuboot_bs_custom_handlers, name) = {  \
            .handler = _handler,                                       \
        }

#endif /* H_BOOT_SERIAL_EXTENTIONS_ */
