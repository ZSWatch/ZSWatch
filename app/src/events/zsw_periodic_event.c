#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

#include "zsw_clock.h"
#include "events/periodic_event.h"

#define PERIODIC_FAST_INTERVAL_MS 100
#define PERIODIC_SLOW_INTERVAL_MS 10000

#if CONFIG_RTC_UPDATE
#include <zephyr/drivers/rtc.h>

static const struct device *const rtc = DEVICE_DT_GET(DT_ALIAS(rtc));

static void handle_mid_timeout(const struct device *dev, void *user_data);
#else
#define PERIODIC_MID_INTERVAL_MS 1000
#endif

ZBUS_CHAN_DECLARE(periodic_event_1s_chan);
ZBUS_CHAN_DECLARE(periodic_event_10s_chan);
ZBUS_CHAN_DECLARE(periodic_event_100ms_chan);

int zsw_periodic_chan_add_obs(const struct zbus_channel *chan, const struct zbus_observer *obs)
{
    struct k_work_delayable *work = NULL;
    int ret;

    ret = zbus_chan_add_obs(chan, obs, K_MSEC(100));

    if (ret != 0) {
        return ret;
    }

    zbus_chan_claim(chan, K_FOREVER);
    work = (struct k_work_delayable *)zbus_chan_user_data(chan);
    __ASSERT(work != NULL, "Invalid channel");
    if (!k_work_delayable_is_pending(work)) {
        if (chan == &periodic_event_10s_chan) {
            ret =  k_work_reschedule(work, K_MSEC(PERIODIC_SLOW_INTERVAL_MS));
        } else if (chan == &periodic_event_1s_chan) {
#if CONFIG_RTC_UPDATE
            ret = rtc_update_set_callback(rtc, handle_mid_timeout, NULL);
#else
            ret =  k_work_reschedule(work, K_MSEC(PERIODIC_MID_INTERVAL_MS));
#endif
        } else if (chan == &periodic_event_100ms_chan) {
            ret =  k_work_reschedule(work, K_MSEC(PERIODIC_FAST_INTERVAL_MS));
        } else {
            __ASSERT(false, "Unknown channel");
        }
    }
    zbus_chan_finish(chan);

    return ret;
}

int zsw_periodic_chan_rm_obs(const struct zbus_channel *chan, const struct zbus_observer *obs)
{
    int ret = zbus_chan_rm_obs(chan, obs, K_MSEC(100));
    if (ret == 0 && sys_slist_is_empty(&chan->data->observers)) {
#if CONFIG_RTC_UPDATE
        if (chan == &periodic_event_1s_chan) {
            ret = rtc_update_set_callback(rtc, NULL, NULL);
        } else
#endif
        {
            struct k_work_delayable *work = NULL;
            work = (struct k_work_delayable *)zbus_chan_user_data(chan);
            __ASSERT(k_work_delayable_is_pending(work), "Periodic slow work is not pending");
            ret = k_work_cancel_delayable(work);
        }
    }
    return ret;
}

static void handle_slow_timeout(struct k_work *item)
{
    struct periodic_event evt = {
    };
    struct k_work_delayable *work = NULL;
    zbus_chan_claim(&periodic_event_10s_chan, K_FOREVER);
    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_event_10s_chan);
    k_work_reschedule(work, K_MSEC(PERIODIC_SLOW_INTERVAL_MS));
    zbus_chan_finish(&periodic_event_10s_chan);

    zbus_chan_pub(&periodic_event_10s_chan, &evt, K_MSEC(250));
}

#if CONFIG_RTC_UPDATE
static void handle_mid_timeout(const struct device *dev, void *user_data)
{
    struct periodic_event evt = {
    };

    zbus_chan_pub(&periodic_event_1s_chan, &evt, K_MSEC(250));
}
#else
static void handle_mid_timeout(struct k_work *item)
{
    struct periodic_event evt = {
    };
    struct k_work_delayable *work = NULL;
    zbus_chan_claim(&periodic_event_1s_chan, K_FOREVER);
    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_event_1s_chan);
    k_work_reschedule(work, K_MSEC(PERIODIC_MID_INTERVAL_MS));
    zbus_chan_finish(&periodic_event_1s_chan);

    zbus_chan_pub(&periodic_event_1s_chan, &evt, K_MSEC(250));
}
#endif

static void handle_fast_timeout(struct k_work *item)
{
    struct periodic_event evt = {
    };
    struct k_work_delayable *work = NULL;
    zbus_chan_claim(&periodic_event_100ms_chan, K_FOREVER);
    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_event_100ms_chan);
    k_work_reschedule(work, K_MSEC(PERIODIC_FAST_INTERVAL_MS));
    zbus_chan_finish(&periodic_event_100ms_chan);

    zbus_chan_pub(&periodic_event_100ms_chan, &evt, K_MSEC(250));
}

static int zsw_timer_init(void)
{
    struct k_work_delayable *work = NULL;

    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_event_10s_chan);
    k_work_init_delayable(work, handle_slow_timeout);

#ifndef CONFIG_RTC_UPDATE
    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_event_1s_chan);
    k_work_init_delayable(work, handle_mid_timeout);
#endif

    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_event_100ms_chan);
    k_work_init_delayable(work, handle_fast_timeout);

    return 0;
}

SYS_INIT(zsw_timer_init, APPLICATION, 1);