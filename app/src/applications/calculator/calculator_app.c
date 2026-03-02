#include <zephyr/kernel.h>
#include <zephyr/init.h>

#ifdef CONFIG_ZSW_LLEXT_APPS
#include <zephyr/llext/symbol.h>
#endif

#include "calculator_ui.h"
#include "managers/zsw_app_manager.h"
#include "llext/zsw_llext_iflash.h"
#include "ui/utils/zsw_ui_utils.h"
#include "smf_calculator_thread.h"

static void calculator_app_start(lv_obj_t *root, lv_group_t *group);
static void calculator_app_stop(void);

static application_t app = {
    .name = "Calc",
    .icon = ZSW_LV_IMG_USE(statistic_icon),
    .start_func = calculator_app_start,
    .stop_func = calculator_app_stop,
    .category = ZSW_APP_CATEGORY_TOOLS,
};

static void calculator_app_start(lv_obj_t *root, lv_group_t *group)
{
    calculator_ui_show(root);
}

static void calculator_app_stop(void)
{
    calculator_ui_remove();
}

static int calculator_app_add(void)
{
    calculator_ui_init();
    calculator_smf_init();
    zsw_app_manager_add_application(&app);
    return 0;
}

#ifdef CONFIG_ZSW_LLEXT_APPS
int app_entry(void)
{
    LLEXT_TRAMPOLINE_APP_FUNCS(&app);
    calculator_app_add();
    return 0;
}
EXPORT_SYMBOL(app_entry);
#else
SYS_INIT(calculator_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
#endif
