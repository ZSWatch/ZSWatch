#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>

#include "events/periodic_event.h"

#define PERIODIC_FAST_INTERVAL_MS 100
#define PERIODIC_MID_INTERVAL_MS 1000
#define PERIODIC_SLOW_INTERVAL_MS 10000

static void handle_slow_timeout(struct k_work *item);
static void handle_fast_timeout(struct k_work *item);
static void handle_mid_timeout(struct k_work *item);

ZBUS_CHAN_DECLARE(periodic_event_10s_chan);
ZBUS_CHAN_DECLARE(periodic_event_1s_chan);
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
            ret =  k_work_reschedule(work, K_MSEC(PERIODIC_MID_INTERVAL_MS));
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
    struct k_work_delayable *work = NULL;
    int ret = zbus_chan_rm_obs(chan, obs, K_MSEC(100));
    if (ret == 0 && sys_slist_is_empty(&chan->data->observers)) {
        work = (struct k_work_delayable *)zbus_chan_user_data(chan);
        __ASSERT(k_work_delayable_is_pending(work), "Periodic slow work is not pending");
        ret = k_work_cancel_delayable(work);
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
    zbus_chan_claim(&periodic_event_10s_chan, K_FOREVER);
    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_event_10s_chan);
    k_work_init_delayable(work, handle_slow_timeout);
    zbus_chan_finish(&periodic_event_10s_chan);

    zbus_chan_claim(&periodic_event_1s_chan, K_FOREVER);
    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_event_1s_chan);
    k_work_init_delayable(work, handle_mid_timeout);
    zbus_chan_finish(&periodic_event_1s_chan);

    zbus_chan_claim(&periodic_event_100ms_chan, K_FOREVER);
    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_event_100ms_chan);
    k_work_init_delayable(work, handle_fast_timeout);
    zbus_chan_finish(&periodic_event_100ms_chan);
    return 0;
}

SYS_INIT(zsw_timer_init, APPLICATION, 1);