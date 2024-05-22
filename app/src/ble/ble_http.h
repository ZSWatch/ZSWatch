#include <stdint.h>
#include "cJSON.h"

typedef enum {
    BLE_HTTP_STATUS_OK,
    BLE_HTTP_STATUS_TIMEOUT,
    BLE_HTTP_STATUS_BUSY,
} ble_http_status_code_t;

/**
 * @brief Callback function for HTTP GET requests.
 *
 * This callback function is invoked when an HTTP GET request has been completed.
 *
 * @param status    The status of the HTTP GET request.
 * @param response  The response data in JSON format. The cJSON object parameter will be deleted automatically after the callback function returns.
 */
typedef void (*ble_http_callback)(ble_http_status_code_t status, cJSON *response);

/**
 * @brief Sends an HTTP GET request to the specified URL.
 *
 * This function sends an HTTP GET request to the specified URL and invokes the provided callback function
 * when the response is received. The callback function is passed the status code and the response data in
 * JSON format. The cJSON object parameter will be deleted automatically after the callback function returns.
 * Hence it shall not attempt to deleted in the callback.
 *
 * @param url The URL to send the GET request to.
 * @param cb The callback function to invoke when the response is received.
 * @return Returns 0 on success, or a negative error code on failure.
 */
int zsw_ble_http_get(char *url, ble_http_callback cb);