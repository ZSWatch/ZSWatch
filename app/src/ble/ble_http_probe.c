/* SPDX-License-Identifier: GPL-3.0-only */
#include "ble_http.h"

#include <errno.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

LOG_MODULE_REGISTER(ble_http_probe, LOG_LEVEL_INF);

/* Fixed destinations avoid accidental location/content-bearing lab requests.
 * loopback requires an explicitly configured ADB reverse to the local fixture.
 * remote is the existing Trivia request, independent of phone location.
 */
static char loopback_url[] = "http://127.0.0.1:18879/companion-hil/ok";
static char remote_url[] = "https://opentdb.com/api.php?amount=1&difficulty=easy&type=boolean";

static void probe_response(ble_http_status_code_t status, char *response)
{
    /* No response contents, URL, coordinates or shell pointer outlive the call. */
    LOG_INF("HTTP probe callback status=%d bytes=%u", status,
            response ? (unsigned int)strlen(response) : 0U);
}

static int cmd_http_probe(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    char *url;
    if (strcmp(argv[1], "loopback") == 0) {
        url = loopback_url;
    } else if (strcmp(argv[1], "remote") == 0) {
        url = remote_url;
    } else {
        shell_error(sh, "Choose loopback or remote");
        return -EINVAL;
    }

    int ret = zsw_ble_http_get(url, probe_response);
    shell_print(sh, "HTTP probe admission rc=%d", ret);
    return ret;
}

SHELL_CMD_ARG_REGISTER(http_probe, NULL, "Companion lab HTTP request: loopback|remote",
                       cmd_http_probe, 2, 0);
