#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/drivers/uart.h>
#include <errno.h>

#include "zsw_usb_manager.h"
#include "zsw_xip_manager.h"

LOG_MODULE_REGISTER(zsw_usb_manager, LOG_LEVEL_INF);

#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)

#define USB_BOOT_TIMEOUT K_SECONDS(20)

#if DT_NODE_HAS_STATUS(DT_NODELABEL(cdc_acm_uart0), okay)
#define ZSW_USB_ACM_NODE DT_NODELABEL(cdc_acm_uart0)
#else
#error "cdc_acm_uart0 node is required for USB manager"
#endif

static const struct device *const cdc_acm_dev = DEVICE_DT_GET(ZSW_USB_ACM_NODE);

static void boot_timeout_handler(struct k_work *work);
static void xip_release_work_handler(struct k_work *work);

static K_MUTEX_DEFINE(state_lock);
static K_WORK_DELAYABLE_DEFINE(boot_timeout_work, boot_timeout_handler);
static K_WORK_DELAYABLE_DEFINE(xip_release_work, xip_release_work_handler);

static int usb_refcount;

static void boot_timeout_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    uint32_t dtr = 0U;
    bool host_connected = (uart_line_ctrl_get(cdc_acm_dev, UART_LINE_CTRL_DTR, &dtr) == 0) && (dtr);

    if (!host_connected) {
        zsw_usb_manager_disable();
        LOG_INF("USB boot guard timed out without host; releasing USB");
    } else {
        LOG_INF("USB CDC ACM host detected");
    }
}

static void xip_release_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    if (zsw_usb_manager_is_enabled()) {
        return;
    }

    zsw_xip_disable();
}

int _zsw_usb_manager_enable(const char *requester)
{
    int ret = 0;

    k_mutex_lock(&state_lock, K_FOREVER);
    usb_refcount++;
    LOG_INF("USB power on (%s => %d)", requester, usb_refcount);

    if (usb_refcount == 1) {
        k_work_cancel_delayable(&xip_release_work);
        zsw_xip_enable();
        ret = usb_enable(NULL);
        __ASSERT(ret == 0, "usb_enable failed (%d)", ret);
        LOG_INF("Enable USB stack");
    }

    k_mutex_unlock(&state_lock);

    return ret;
}

int _zsw_usb_manager_disable(const char *requester)
{
    int ret = 0;

    k_mutex_lock(&state_lock, K_FOREVER);

    if (usb_refcount == 0) {
        LOG_ERR("USB disable called with zero refcount by %s", requester);
        k_mutex_unlock(&state_lock);
        return -EALREADY;
    }

    usb_refcount--;

    if (usb_refcount == 0) {
        ret = usb_disable();
        __ASSERT(ret == 0, "usb_disable failed (%d)", ret);
        LOG_INF("Disable USB stack");
        // The usb_disable() call above takes some time to fully complete, and
        // finishes in another context. So we need to wait before disabling XIP.
        // Seems as the USB user callback can't be used as it's not getting called when disabled.
        // So we use a delayed work instead.
        k_work_reschedule(&xip_release_work, K_MSEC(50));
    }

    k_mutex_unlock(&state_lock);

    return ret;
}

bool zsw_usb_manager_is_enabled(void)
{
    int ret = 0;

    k_mutex_lock(&state_lock, K_FOREVER);
    ret = usb_refcount > 0;
    k_mutex_unlock(&state_lock);

    return ret;
}

static int usb_manager_init(void)
{
    if (!device_is_ready(cdc_acm_dev)) {
        LOG_WRN("CDC ACM device not ready; USB manager inactive");
        return 0;
    }

    zsw_usb_manager_enable();
    k_work_schedule(&boot_timeout_work, USB_BOOT_TIMEOUT);

    return 0;
}

SYS_INIT(usb_manager_init, APPLICATION, 50);

#endif /* CONFIG_USB_DEVICE_STACK */
