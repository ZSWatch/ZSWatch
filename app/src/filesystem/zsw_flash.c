#include <zephyr/sys/reboot.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/retention/bootmode.h>

int zsw_flash_erase_external(void)
{
    struct flash_pages_info flash_get_page;
    const struct device *flash_dev = DEVICE_DT_GET_OR_NULL(DT_CHOSEN(nordic_pm_ext_flash));
    bootmode_clear();
    if (flash_dev) {
        flash_get_page_info_by_idx(flash_dev, 0, &flash_get_page);
        flash_erase(flash_dev, 0, flash_get_page_count(flash_dev) * flash_get_page.size);
        sys_reboot(SYS_REBOOT_COLD);
        return 0;
    } else {
        return -ENODEV;
    }
}