#include <zsw_periodic_event.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <events/periodic_event.h>

static void handle_slow_timeout(struct k_work *item);
static void handle_fast_timeout(struct k_work *item);

ZBUS_CHAN_DECLARE(periodic_event_slow_chan);
ZBUS_CHAN_DECLARE(periodic_event_fast_chan);

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
        if (chan == &periodic_event_slow_chan) {
            ret =  k_work_reschedule(work, K_MSEC(1000));
        } else if (chan == &periodic_event_fast_chan) {
            ret =  k_work_reschedule(work, K_MSEC(100));
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
    zbus_chan_claim(&periodic_event_slow_chan, K_FOREVER);
    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_event_slow_chan);
    k_work_reschedule(work, K_MSEC(1000));
    zbus_chan_finish(&periodic_event_slow_chan);

    zbus_chan_pub(&periodic_event_slow_chan, &evt, K_MSEC(250));
}

static void handle_fast_timeout(struct k_work *item)
{
    struct periodic_event evt = {
    };
    struct k_work_delayable *work = NULL;
    zbus_chan_claim(&periodic_event_fast_chan, K_FOREVER);
    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_event_fast_chan);
    k_work_reschedule(work, K_MSEC(100));
    zbus_chan_finish(&periodic_event_fast_chan);

    zbus_chan_pub(&periodic_event_fast_chan, &evt, K_MSEC(250));
}

static int zsw_timer_init(void)
{
    struct k_work_delayable *work = NULL;
    zbus_chan_claim(&periodic_event_slow_chan, K_FOREVER);
    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_event_slow_chan);
    k_work_init_delayable(work, handle_slow_timeout);
    zbus_chan_finish(&periodic_event_slow_chan);

    zbus_chan_claim(&periodic_event_fast_chan, K_FOREVER);
    work = (struct k_work_delayable *)zbus_chan_user_data(&periodic_event_fast_chan);
    k_work_init_delayable(work, handle_fast_timeout);
    zbus_chan_finish(&periodic_event_fast_chan);
    return 0;
}

SYS_INIT(zsw_timer_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);