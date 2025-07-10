#include "ble_http.h"
#include <zephyr/kernel.h>
#include <string.h>
#include <stdio.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <events/ble_event.h>
#include <cJSON.h>

LOG_MODULE_REGISTER(ble_http, LOG_LEVEL_DBG);

#define GB_HTTP_REQUEST_FMT "{\"t\":\"http\", \"url\":\"%s\", id:\"%d\"} \n"

#define HTTP_TIMEOUT_SECONDS 10

static void zbus_ble_comm_data_callback(const struct zbus_channel *chan);
static void ble_http_timeout_handler(struct k_work *work);

ZBUS_LISTENER_DEFINE(ble_http_lis, zbus_ble_comm_data_callback);
ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_CHAN_ADD_OBS(ble_comm_data_chan, ble_http_lis, 1);

static bool request_pending;
static uint16_t request_id;
static ble_http_callback ble_http_cb;

K_WORK_DELAYABLE_DEFINE(ble_http_timeout_work, ble_http_timeout_handler);

static void ble_http_timeout_handler(struct k_work *work)
{
    LOG_WRN("HTTP Timeout");
    request_pending = false;
    ble_http_cb(BLE_HTTP_STATUS_TIMEOUT, NULL);
}

static void zbus_ble_comm_data_callback(const struct zbus_channel *chan)
{
    const struct ble_data_event *event = zbus_chan_const_msg(chan);

    if (event->data.type == BLE_COMM_DATA_TYPE_HTTP) {
        if (event->data.data.http_response.id != request_id) {
            LOG_WRN("Not the expected response ID, was: %d, expected: %d", event->data.data.http_response.id, request_id);
            return;
        }
        struct k_work_sync sync;
        k_work_cancel_delayable_sync(&ble_http_timeout_work, &sync);
        request_pending = false;

        if (strlen(event->data.data.http_response.err) > 0) {
            LOG_WRN("HTTP request failed: %s", event->data.data.http_response.err);
        } else if (strlen(event->data.data.http_response.response) > 0) {
            char *fixed_rsp = k_malloc(strlen(event->data.data.http_response.response) + 1);
            __ASSERT(fixed_rsp, "Failed to allocate memory for fixed_rsp");
            // As the response from Gadgetbride contains two characters like this[\\, "] instead of just one character [\"],
            // we need to remove them for it to be avalid JSON accepted by cJSON
            int i;
            int j;
            for (i = 0, j = 0; i < strlen(event->data.data.http_response.response) - 1;) {
                if (event->data.data.http_response.response[i] == '\\' && event->data.data.http_response.response[i + 1] == '"') {
                    fixed_rsp[j] = '\"';
                    j++;
                    i += 2;
                } else {
                    fixed_rsp[j] = event->data.data.http_response.response[i];
                    j++;
                    i++;
                }
            }
            fixed_rsp[j - 1] = '\0'; // Remove the last " as it belongs not to the data
            ble_http_cb(BLE_HTTP_STATUS_OK, fixed_rsp);
            k_free(fixed_rsp);
        }
    }
}

int zsw_ble_http_get(char *url, ble_http_callback cb)
{
    int ret;
    char *request;

    if (request_pending) {
        // TODO implement a queue for requests
        return -EBUSY;
    }

    request_id++;

    request = k_calloc(1, strlen(url) + strlen(GB_HTTP_REQUEST_FMT) + 1);
    __ASSERT(request, "Failed to allocate memory for request URL");
    memset(request, 0, strlen(url) + strlen(GB_HTTP_REQUEST_FMT) + 1);

    snprintf(request, strlen(url) + strlen(GB_HTTP_REQUEST_FMT) + 1, GB_HTTP_REQUEST_FMT, url, request_id);
    ret = ble_comm_send(request, strlen(request));
    k_free(request);
    if (ret != 0) {
        return ret;
    }

    ble_http_cb = cb;
    request_pending = true;

    k_work_schedule(&ble_http_timeout_work, K_SECONDS(HTTP_TIMEOUT_SECONDS));

    return 0;
}