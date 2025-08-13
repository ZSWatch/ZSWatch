
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/task_wdt/task_wdt.h>
#include <zephyr/logging/log.h>

#if defined(CONFIG_TASK_WDT) && !defined(CONFIG_ARCH_POSIX)
LOG_MODULE_REGISTER(zsw_wdt, CONFIG_ZSW_APP_LOG_LEVEL);

#define TASK_WDT_FEED_INTERVAL_MS  3000

static int kernel_wdt_id;

static void run_wdt_work(struct k_work *item);

K_WORK_DELAYABLE_DEFINE(wdt_work, run_wdt_work);

static void run_wdt_work(struct k_work *item)
{
    task_wdt_feed(kernel_wdt_id);
    k_work_schedule(&wdt_work, K_MSEC(TASK_WDT_FEED_INTERVAL_MS));
}

static int zsw_wdt_init(void)
{
    LOG_DBG("Initializing ZSW Watchdog Timer");

    const struct device *hw_wdt_dev = DEVICE_DT_GET(DT_ALIAS(watchdog0));
    if (!device_is_ready(hw_wdt_dev)) {
        LOG_DBG("Hardware watchdog %s is not ready; ignoring it.",
                hw_wdt_dev->name);
        hw_wdt_dev = NULL;
    }

    task_wdt_init(hw_wdt_dev);
    kernel_wdt_id = task_wdt_add(TASK_WDT_FEED_INTERVAL_MS * 5, NULL, NULL);

    k_work_schedule(&wdt_work, K_NO_WAIT);

    LOG_INF("ZSW Watchdog Timer initialized successfully");
    return 0;
}

SYS_INIT(zsw_wdt_init, APPLICATION, 99);
#endif