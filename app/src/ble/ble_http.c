#include "ble_http.h"
#include <zephyr/kernel.h>
#include <string.h>
#include <stdio.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <events/ble_event.h>

LOG_MODULE_REGISTER(ble_http, LOG_LEVEL_DBG);

#define GB_HTTP_REQUEST_FMT "{\"t\":\"http\",\"url\":\"%s\",\"id\":\"%u\"}\n"
#define HTTP_TIMEOUT_SECONDS 10

static void zbus_ble_comm_data_callback(const struct zbus_channel *chan);
static void ble_http_timeout_handler(struct k_work *work);

ZBUS_LISTENER_DEFINE(ble_http_lis, zbus_ble_comm_data_callback);
ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_CHAN_ADD_OBS(ble_comm_data_chan, ble_http_lis, 1);

static struct k_spinlock request_lock;
static bool request_pending;
static uint16_t request_id;
static int64_t request_deadline;
static ble_http_callback ble_http_cb;

K_WORK_DELAYABLE_DEFINE(ble_http_timeout_work, ble_http_timeout_handler);

static void ble_http_timeout_handler(struct k_work *work)
{
    ARG_UNUSED(work);
    ble_http_callback cb = NULL;
    k_spinlock_key_t key = k_spin_lock(&request_lock);
    if (request_pending) {
        int64_t remaining = request_deadline - k_uptime_get();
        if (remaining > 0) {
            /* An old queued timeout may run after a callback starts a new request. */
            k_work_reschedule(&ble_http_timeout_work, K_MSEC(remaining));
        } else {
            cb = ble_http_cb;
            request_pending = false;
            ble_http_cb = NULL;
        }
    }
    k_spin_unlock(&request_lock, key);
    if (cb) {
        LOG_WRN("HTTP Timeout");
        cb(BLE_HTTP_STATUS_TIMEOUT, NULL);
    }
}

static void zbus_ble_comm_data_callback(const struct zbus_channel *chan)
{
    const struct ble_data_event *event = zbus_chan_const_msg(chan);
    if (event->data.type != BLE_COMM_DATA_TYPE_HTTP) {
        return;
    }

    ble_http_callback cb = NULL;
    bool expired = false;
    k_spinlock_key_t key = k_spin_lock(&request_lock);
    if (request_pending && event->data.data.http_response.id == request_id) {
        expired = k_uptime_get() >= request_deadline;
        cb = ble_http_cb;
        request_pending = false;
        ble_http_cb = NULL;
    }
    k_spin_unlock(&request_lock, key);
    if (!cb) {
        return; /* Late, duplicate or unrelated replies never finish another request. */
    }

    /* Do not synchronously cancel work: its callback may itself issue an HTTP request.
     * The deadline guard makes a queued timeout harmless and keeps callbacks outside the lock.
     */
    if (expired) {
        cb(BLE_HTTP_STATUS_TIMEOUT, NULL);
    } else if (event->data.data.http_response.err[0] || !event->data.data.http_response.response[0]) {
        LOG_WRN("HTTP request failed or returned an empty body");
        cb(BLE_HTTP_STATUS_ERROR, NULL);
    } else {
        cb(BLE_HTTP_STATUS_OK, (char *)event->data.data.http_response.response);
    }
}

int zsw_ble_http_get(char *url, ble_http_callback cb)
{
    if (!url || !cb) {
        return -EINVAL;
    }
    /* Five digits for the uint16_t ID, even after the request counter grows. */
    size_t capacity = strlen(url) + sizeof(GB_HTTP_REQUEST_FMT) + 5;
    char *request = k_malloc(capacity);
    if (!request) {
        return -ENOMEM;
    }

    k_spinlock_key_t key = k_spin_lock(&request_lock);
    if (request_pending) {
        k_spin_unlock(&request_lock, key);
        k_free(request);
        return -EBUSY;
    }
    uint16_t id = ++request_id;
    ble_http_cb = cb;
    request_pending = true;
    request_deadline = k_uptime_get() + HTTP_TIMEOUT_SECONDS * 1000;
    /* Publish the callback and timeout before sending: a fast reply can arrive during send. */
    k_work_reschedule(&ble_http_timeout_work, K_SECONDS(HTTP_TIMEOUT_SECONDS));
    k_spin_unlock(&request_lock, key);

    snprintf(request, capacity, GB_HTTP_REQUEST_FMT, url, (unsigned int)id);
    int ret = ble_comm_send((const uint8_t *)request, strlen(request));
    k_free(request);
    if (ret != 0) {
        key = k_spin_lock(&request_lock);
        if (request_pending && request_id == id) {
            request_pending = false;
            ble_http_cb = NULL;
        }
        k_spin_unlock(&request_lock, key);
    }
    return ret;
}
