#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "cJSON.h"
#include "ble/ble_http.c"

static int64_t now_ms;
static int lock_depth, callback_count, send_result;
static bool fast_reply, start_next, fail_allocation;
static ble_http_status_code_t last_status;
static char last_body[2048], last_request[4096];
static unsigned int sent_id;

int64_t k_uptime_get(void) { return now_ms; }
k_spinlock_key_t k_spin_lock(struct k_spinlock *lock)
{ (void)lock; assert(lock_depth++ == 0); return 0; }
void k_spin_unlock(struct k_spinlock *lock, k_spinlock_key_t key)
{ (void)lock; (void)key; assert(--lock_depth == 0); }
int k_work_reschedule(struct k_work_delayable *work, int64_t delay)
{ assert(lock_depth == 1); work->due = now_ms + delay; return 1; }
void *k_malloc(size_t size) { return fail_allocation ? NULL : malloc(size); }
void k_free(void *ptr) { free(ptr); }

static void send_ble_data_event(const struct ble_data_event *event)
{
    struct zbus_channel channel = { .msg = event };
    zbus_ble_comm_data_callback(&channel);
}
/* Extract only the actual parser entrypoint, not a duplicate test implementation. */
#include "http_parser.inc"

static void callback(ble_http_status_code_t status, char *response)
{
    assert(lock_depth == 0);
    callback_count++;
    last_status = status;
    snprintf(last_body, sizeof(last_body), "%s", response ? response : "");
    if (start_next) {
        start_next = false;
        assert(zsw_ble_http_get("http://fixture/next", callback) == 0);
    }
}
static int reply(unsigned int id, const char *field)
{
    char packet[8192];
    snprintf(packet, sizeof(packet), "{\"t\":\"http\",\"id\":\"%u\",%s}", id, field);
    return parse_httpstate(packet, strlen(packet));
}
int ble_comm_send(const uint8_t *data, uint16_t len)
{
    assert(lock_depth == 0 && len < sizeof(last_request));
    memcpy(last_request, data, len); last_request[len] = 0;
    cJSON *root = cJSON_Parse(last_request);
    assert(root != NULL);
    sent_id = strtoul(cJSON_GetObjectItem(root, "id")->valuestring, NULL, 10);
    cJSON_Delete(root);
    if (fast_reply) {
        fast_reply = false;
        assert(reply(sent_id, "\"resp\":\"{\\\"ok\\\":true}\"") == 0);
    }
    return send_result;
}
static void timeout(void) { ble_http_timeout_handler(&ble_http_timeout_work.work); }
static void reset(void)
{
    request_pending = false; ble_http_cb = NULL;
    now_ms = callback_count = send_result = lock_depth = 0;
    fast_reply = start_next = fail_allocation = false;
    last_body[0] = 0;
}
int main(void)
{
    reset(); fast_reply = true;
    assert(zsw_ble_http_get("http://fixture/ok", callback) == 0);
    assert(callback_count == 1 && last_status == BLE_HTTP_STATUS_OK);
    assert(strcmp(last_body, "{\"ok\":true}") == 0);
    assert(reply(sent_id, "\"resp\":\"duplicate\"") == 0);
    now_ms = 10000; timeout(); assert(callback_count == 1);

    reset(); assert(zsw_ble_http_get("http://fixture/error", callback) == 0);
    assert(reply(sent_id, "\"err\":\"relay unavailable\"") == 0);
    assert(callback_count == 1 && last_status == BLE_HTTP_STATUS_ERROR && !last_body[0]);
    now_ms = 10000; timeout(); assert(callback_count == 1);

    const char *empty[] = { "\"resp\":\"\"", "\"other\":true", "\"err\":\"\"", "\"resp\":null" };
    for (unsigned i = 0; i < sizeof(empty) / sizeof(empty[0]); i++) {
        reset(); assert(zsw_ble_http_get("http://fixture/empty", callback) == 0);
        assert(reply(sent_id, empty[i]) == 0);
        assert(callback_count == 1 && last_status == BLE_HTTP_STATUS_ERROR);
    }

    reset(); assert(zsw_ble_http_get("http://fixture/timeout", callback) == 0);
    unsigned old_id = sent_id;
    assert(zsw_ble_http_get("http://fixture/busy", callback) == -EBUSY);
    now_ms = 10000; start_next = true; timeout();
    assert(callback_count == 1 && last_status == BLE_HTTP_STATUS_TIMEOUT && request_pending);
    assert(reply(old_id, "\"resp\":\"late\"") == 0);
    timeout(); /* Old queued timeout must use the new deadline. */
    assert(callback_count == 1 && request_pending && ble_http_timeout_work.due == 20000);
    assert(reply(sent_id, "\"resp\":\"next\"") == 0);
    assert(callback_count == 2 && last_status == BLE_HTTP_STATUS_OK && !request_pending);

    reset(); assert(zsw_ble_http_get("http://fixture/late", callback) == 0);
    now_ms = 10001;
    assert(reply(sent_id, "\"resp\":\"past deadline before timeout worker\"") == 0);
    timeout(); assert(callback_count == 1 && last_status == BLE_HTTP_STATUS_TIMEOUT);

    reset(); assert(zsw_ble_http_get("http://fixture/reentrant", callback) == 0);
    now_ms = 500; start_next = true;
    assert(reply(sent_id, "\"resp\":\"first\"") == 0);
    assert(callback_count == 1 && request_pending && request_deadline == 10500);
    now_ms = 10000; timeout(); assert(request_pending && callback_count == 1);
    now_ms = 10500; timeout(); assert(!request_pending && callback_count == 2);

    reset(); send_result = -EIO;
    assert(zsw_ble_http_get("http://fixture/send-failed", callback) == -EIO);
    now_ms = 10000; timeout(); assert(callback_count == 0 && !request_pending);
    send_result = 0; assert(zsw_ble_http_get("http://fixture/retry", callback) == 0);
    assert(reply(sent_id, "\"resp\":\"retry\"") == 0 && callback_count == 1);
    assert(zsw_ble_http_get(NULL, callback) == -EINVAL);
    assert(zsw_ble_http_get("http://fixture", NULL) == -EINVAL);
    fail_allocation = true; assert(zsw_ble_http_get("http://fixture", callback) == -ENOMEM);

    reset(); request_id = 65534;
    assert(zsw_ble_http_get("http://fixture/large-id", callback) == 0 && sent_id == 65535);
    assert(reply(sent_id, "\"resp\":\"line\\nquote\\\"slash\\\\\"") == 0);
    assert(strcmp(last_body, "line\nquote\"slash\\") == 0);
    assert(zsw_ble_http_get("http://fixture/wrap", callback) == 0 && sent_id == 0);
    char bad[] = "{\"t\":\"http\",\"id\":\"invalid\",\"resp\":\"bad\"}";
    assert(parse_httpstate(bad, strlen(bad)) == -EINVAL && request_pending);
    char numeric[] = "{\"t\":\"http\",\"id\":0,\"resp\":\"numeric\"}";
    assert(parse_httpstate(numeric, strlen(numeric)) == 0 && !request_pending);

    reset(); assert(zsw_ble_http_get("http://fixture/oversized", callback) == 0);
    char oversized[5000]; memset(oversized, 'x', sizeof(oversized));
    oversized[0] = '"'; memcpy(oversized + 1, "resp\":\"", 7);
    oversized[sizeof(oversized) - 2] = '"'; oversized[sizeof(oversized) - 1] = 0;
    assert(reply(sent_id, oversized) == 0);
    assert(callback_count == 1 && last_status == BLE_HTTP_STATUS_ERROR);
    return 0;
}
