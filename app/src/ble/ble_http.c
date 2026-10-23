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
#define HTTP_MAX_PENDING_REQUESTS 5

typedef struct {
    ble_http_callback cb;
    int64_t deadline;
    uint16_t id;
} http_request_t;

static void zbus_ble_comm_data_callback(const struct zbus_channel *chan);
static void ble_http_timeout_handler(struct k_work *work);

ZBUS_LISTENER_DEFINE(ble_http_lis, zbus_ble_comm_data_callback);
ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_CHAN_ADD_OBS(ble_comm_data_chan, ble_http_lis, 1);

static struct k_spinlock request_lock;
static http_request_t requests[HTTP_MAX_PENDING_REQUESTS];
static uint16_t request_id;

K_WORK_DELAYABLE_DEFINE(ble_http_timeout_work, ble_http_timeout_handler);

static void ble_http_timeout_handler(struct k_work *work)
{
    ARG_UNUSED(work);
    ble_http_callback expired[HTTP_MAX_PENDING_REQUESTS];
    int num_expired = 0;
    int64_t next_deadline = INT64_MAX;

    k_spinlock_key_t key = k_spin_lock(&request_lock);
    int64_t now = k_uptime_get();
    for (int i = 0; i < HTTP_MAX_PENDING_REQUESTS; i++) {
        if (!requests[i].cb) {
            continue;
        }
        if (requests[i].deadline <= now) {
            expired[num_expired++] = requests[i].cb;
            requests[i].cb = NULL;
        } else if (requests[i].deadline < next_deadline) {
            next_deadline = requests[i].deadline;
        }
    }
    if (next_deadline != INT64_MAX) {
        k_work_reschedule(&ble_http_timeout_work, K_MSEC(next_deadline - now));
    }
    k_spin_unlock(&request_lock, key);

    for (int i = 0; i < num_expired; i++) {
        LOG_WRN("HTTP Timeout");
        expired[i](BLE_HTTP_STATUS_TIMEOUT, NULL);
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
    for (int i = 0; i < HTTP_MAX_PENDING_REQUESTS; i++) {
        if (requests[i].cb && event->data.data.http_response.id == requests[i].id) {
            expired = k_uptime_get() >= requests[i].deadline;
            cb = requests[i].cb;
            requests[i].cb = NULL;
            break;
        }
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
    http_request_t *slot = NULL;
    for (int i = 0; i < HTTP_MAX_PENDING_REQUESTS; i++) {
        if (!requests[i].cb) {
            slot = &requests[i];
            break;
        }
    }
    if (!slot) {
        k_spin_unlock(&request_lock, key);
        k_free(request);
        return -EBUSY;
    }
    uint16_t id = ++request_id;
    slot->id = id;
    slot->cb = cb;
    slot->deadline = k_uptime_get() + HTTP_TIMEOUT_SECONDS * 1000;
    /* Publish the callback and timeout before sending: a fast reply can arrive during send.
     * Deadlines only grow, so an already scheduled timeout fires first and reschedules.
     */
    k_work_schedule(&ble_http_timeout_work, K_SECONDS(HTTP_TIMEOUT_SECONDS));
    k_spin_unlock(&request_lock, key);

    snprintf(request, capacity, GB_HTTP_REQUEST_FMT, url, (unsigned int)id);
    int ret = ble_comm_send((const uint8_t *)request, strlen(request));
    k_free(request);
    if (ret != 0) {
        key = k_spin_lock(&request_lock);
        if (slot->cb && slot->id == id) {
            slot->cb = NULL;
        }
        k_spin_unlock(&request_lock, key);
    }
    return ret;
}
