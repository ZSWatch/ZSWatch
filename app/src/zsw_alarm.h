#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/rtc.h>

#define ZSW_MAX_ALARMS 20
#define ZSW_ALARM_INVALID_ID (ZSW_MAX_ALARMS + 1)

typedef void(*alarm_cb)(void *user_data);

typedef struct {
    struct rtc_time expiry_time;
    bool used;
    bool enabled;
    alarm_cb cb;
    void *user_data;
} zsw_alarm_t;

/* Adds a new one shot alarm.
 *
 * Parameters:
 * - expiry_time: The time at which the alarm should expire. Passing only HH:MM:SS will cause alarm to trigger same or next day,
 *                depending if expiry_time is before or after the current time.
 * - callback: The function to be called when the alarm expires.
 * - user_data: A pointer to user-defined data to be passed to the callback function.
 *
 * Returns:
 * - 0 on success, or a negative error code on failure.
 */
int zsw_alarm_add(struct rtc_time expiry_time, alarm_cb callback, void *user_data);

/* Adds a new timer alarm.
 *
 * Parameters:
 * - hour: The number of hours for the timer.
 * - min: The number of minutes for the timer.
 * - sec: The number of seconds for the timer.
 * - callback: The function to be called when the timer expires.
 * - user_data: A pointer to user-defined data to be passed to the callback function.
 *
 * Returns:
 * - alarm handle on success, or a negative error code on failure.
 */
int zsw_alarm_add_timer(uint16_t hour, uint16_t min, uint16_t sec, alarm_cb callback, void *user_data);

/* Gets the remaining time for the specified alarm.
 *
 * Parameters:
 * - alarm_id: The ID of the alarm.
 * - hour: A pointer to store the remaining hours.
 * - min: A pointer to store the remaining minutes.
 * - sec: A pointer to store the remaining seconds.
 *
 * Returns:
 * - alarm handle on success, or a negative error code on failure.
 */
int zsw_alarm_get_remaining(uint32_t alarm_id, uint32_t *hour, uint32_t *min, uint32_t *sec);

/* Sets the enabled status of the specified alarm.
 *
 * Parameters:
 * - alarm_id: The ID of the alarm.
 * - enabled: The new enabled status of the alarm.
 *
 * Returns:
 * - 0 on success, or a negative error code on failure.
 */
int zsw_alarm_set_enabled(uint32_t alarm_id, bool enabled);

/* Gets the enabled status of the specified alarm.
 *
 * Parameters:
 * - alarm_id: The ID of the alarm.
 * - enabled: A pointer to store the enabled status of the alarm.
 *
 * Returns:
 * - 0 on success, or a negative error code on failure.
 */
int zsw_alarm_get_enabled(uint32_t alarm_id, bool *enabled);

/* Removes the specified alarm.
 *
 * Parameters:
 * - alarm_id: The ID of the alarm to be removed.
 *
 * Returns:
 * - 0 on success, or a negative error code on failure.
 */
int zsw_alarm_remove(uint32_t alarm_id);