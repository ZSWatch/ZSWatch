#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/shell/shell.h>
#include <zephyr/settings/settings.h>
#include <zephyr/input/input.h>
#include <zephyr/zbus/zbus.h>
#ifdef CONFIG_RETENTION_BOOT_MODE
#include <zephyr/retention/bootmode.h>
#endif

#include "zsw_settings.h"
#include <zsw_coredump.h>
#include <zsw_cpu_freq.h>
#include "fuel_gauge/zsw_pmic.h"
#include "managers/zsw_power_manager.h"
#include "managers/zsw_app_manager.h"
#include "drivers/zsw_vibration_motor.h"
#include "drivers/zsw_display_control.h"
#include "ui/zsw_ui_controller.h"
#include "events/battery_event.h"
#include "events/pressure_event.h"

ZBUS_CHAN_DECLARE(battery_sample_data_chan);
ZBUS_CHAN_DECLARE(pressure_data_chan);

#ifdef CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED

static int cmd_battery(const struct shell *sh, size_t argc, char **argv)
{
    struct battery_sample_event sample;
    int ret = zsw_pmic_get_full_state(&sample);

    if (ret != 0) {
        shell_error(sh, "Failed to read battery status");
        return ret;
    }

    shell_print(sh, "Battery Status:");
    shell_print(sh, "  Voltage:     %d mV", sample.mV);
    shell_print(sh, "  Level:       %d%%", (int)sample.percent);
    shell_print(sh, "  Temperature: %.1f C", sample.temperature);
    shell_print(sh, "  Current:     %.2f mA", sample.avg_current * 1000);
    shell_print(sh, "  Charging:    %s", sample.is_charging ? "Yes" : "No");
    shell_print(sh, "  Time to empty: %.0f min", sample.tte);
    shell_print(sh, "  Time to full:  %.0f min", sample.ttf);
    shell_print(sh, "  VBUS:        %s", zsw_pmic_get_vbus_connected() ? "Connected" : "Disconnected");

    return 0;
}

SHELL_CMD_ARG_REGISTER(battery, NULL, "Show battery voltage, level, and charging status", cmd_battery, 1, 0);

#endif

static int cmd_factory_reset(const struct shell *sh, size_t argc, char **argv)
{
    int ret;

    shell_print(sh, "WARNING: This will erase all settings and reboot the device!");
    shell_print(sh, "Erasing settings...");

    ret = zsw_settings_erase_all();
    if (ret != 0) {
        shell_error(sh, "Failed to erase settings: %d", ret);
        return ret;
    }

    shell_print(sh, "Settings erased successfully.");
    shell_print(sh, "Rebooting in 2 seconds...");
    k_msleep(2000);

    sys_reboot(SYS_REBOOT_COLD);

    return 0;
}

SHELL_CMD_ARG_REGISTER(factory_reset, NULL, "Erase all settings and reboot device", cmd_factory_reset, 1, 0);

static int cmd_power_status(const struct shell *sh, size_t argc, char **argv)
{
    const char *state_str[] = {"Active", "Inactive", "Not worn/Stationary"};
    zsw_power_manager_state_t current_state = zsw_power_manager_get_state();
    uint32_t ms_to_inactive = zsw_power_manager_get_ms_to_inactive();

    shell_print(sh, "Power Status:");
    shell_print(sh, "  State: %s", state_str[current_state]);
    shell_print(sh, "  Time to sleep: %u seconds", ms_to_inactive / 1000);
    shell_print(sh, "  Total uptime: %llu seconds", k_uptime_get() / 1000);
    return 0;
}

static int cmd_power_wake(const struct shell *sh, size_t argc, char **argv)
{
    bool woke_up = zsw_power_manager_reset_idle_timout();
    shell_print(sh, "Idle timeout reset. %s", woke_up ? "Device woken up" : "Device already awake");
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_power,
                               SHELL_CMD_ARG(status, NULL, "Show power status and sleep timer", cmd_power_status, 1, 0),
                               SHELL_CMD_ARG(wake, NULL, "Reset idle timeout to keep device awake", cmd_power_wake, 1, 0),
                               SHELL_SUBCMD_SET_END
                              );

SHELL_CMD_REGISTER(power, &sub_power, "Power management commands", NULL);

static int cmd_vibration_run_pattern(const struct shell *sh, size_t argc, char **argv)
{
    zsw_vibration_pattern_t pattern;

    if (argc < 2) {
        shell_error(sh, "Missing pattern argument");
        return -EINVAL;
    }

    if (strcmp(argv[1], "click") == 0) {
        pattern = ZSW_VIBRATION_PATTERN_CLICK;
    } else if (strcmp(argv[1], "notification") == 0) {
        pattern = ZSW_VIBRATION_PATTERN_NOTIFICATION;
    } else if (strcmp(argv[1], "alarm") == 0) {
        pattern = ZSW_VIBRATION_PATTERN_ALARM;
    } else {
        shell_error(sh, "Unknown pattern '%s'", argv[1]);
        shell_print(sh, "Valid patterns: click, notification, alarm");
        return -EINVAL;
    }

    int ret = zsw_vibration_run_pattern(pattern);
    if (ret != 0) {
        shell_error(sh, "Failed to start vibration pattern (%d)", ret);
        return ret;
    }

    shell_print(sh, "Pattern started: %s", argv[1]);
    return 0;
}

static int cmd_vibration_help(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    shell_help(sh);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_vibration,
                               SHELL_CMD_ARG(run_pattern, NULL, "Run vibration pattern <click|notification|alarm>", cmd_vibration_run_pattern, 2, 0),
                               SHELL_SUBCMD_SET_END
                              );

SHELL_CMD_REGISTER(vibration, &sub_vibration, "Vibration motor commands", cmd_vibration_help);

static int cmd_display_set_brightness(const struct shell *sh, size_t argc, char **argv)
{
    long brightness;
    char *endptr;

    if (argc < 2) {
        shell_error(sh, "Missing brightness value (0-100)");
        return -EINVAL;
    }

    brightness = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || brightness < 0 || brightness > 100) {
        shell_error(sh, "Invalid brightness '%s' (expected 0-100)", argv[1]);
        return -EINVAL;
    }

    zsw_display_control_set_brightness((uint8_t)brightness);
    shell_print(sh, "Brightness set to %ld%%", brightness);
    return 0;
}

static int cmd_display_get_brightness(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    shell_print(sh, "Current brightness: %u%%", zsw_display_control_get_brightness());
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_display,
                               SHELL_CMD_ARG(set_brightness, NULL, "Set display brightness percent", cmd_display_set_brightness, 2, 0),
                               SHELL_CMD_ARG(get_brightness, NULL, "Get current display brightness", cmd_display_get_brightness, 1, 0),
                               SHELL_SUBCMD_SET_END
                              );

SHELL_CMD_REGISTER(display, &sub_display, "Display control commands", NULL);

static int cmd_coredump_init(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    int ret = zsw_coredump_init();
    if (ret != 0) {
        shell_error(sh, "Coredump init failed (%d)", ret);
        return ret;
    }

    shell_print(sh, "Coredump initialized");
    return 0;
}

static int cmd_coredump_to_log(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    int ret = zsw_coredump_to_log();
    if (ret != 0) {
        shell_error(sh, "Failed to print coredump (%d)", ret);
    }
    return ret;
}

static int cmd_coredump_erase(const struct shell *sh, size_t argc, char **argv)
{
    long index;
    char *endptr;

    if (argc < 2) {
        shell_error(sh, "Missing coredump index");
        return -EINVAL;
    }

    index = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || index < 0) {
        shell_error(sh, "Invalid index '%s'", argv[1]);
        return -EINVAL;
    }

    zsw_coredump_erase((int)index);
    shell_print(sh, "Erased coredump index %ld", index);
    return 0;
}

static int cmd_coredump_summary(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    zsw_coredump_sumary_t summary = {0};
    int dumps = 0;
    int ret = zsw_coredump_get_summary(&summary, 1, &dumps);

    if (ret == -ENODATA) {
        shell_print(sh, "No stored coredumps");
        return 0;
    }

    if (ret != 0) {
        shell_error(sh, "Failed to read summary (%d)", ret);
        return ret;
    }

    if (dumps == 0) {
        shell_print(sh, "No stored coredumps");
        return 0;
    }

    shell_print(sh, "Stored coredumps: %d", dumps);
    shell_print(sh, "  Timestamp: %s", summary.datetime);
    shell_print(sh, "  File: %s", summary.file);
    shell_print(sh, "  Line: %d", summary.line);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_coredump,
                               SHELL_CMD_ARG(init, NULL, "Initialize coredump handling", cmd_coredump_init, 1, 0),
                               SHELL_CMD_ARG(log, NULL, "Print stored coredump to log", cmd_coredump_to_log, 1, 0),
                               SHELL_CMD_ARG(erase, NULL, "Erase stored coredump by index", cmd_coredump_erase, 2, 0),
                               SHELL_CMD_ARG(summary, NULL, "Show summary of stored coredump", cmd_coredump_summary, 1, 0),
                               SHELL_SUBCMD_SET_END
                              );

SHELL_CMD_REGISTER(coredump, &sub_coredump, "Coredump management commands", NULL);

static int cmd_cpu_get_freq(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    zsw_cpu_freq_t freq = zsw_cpu_get_freq();
    const char *freq_str = (freq == ZSW_CPU_FREQ_FAST) ? "fast" : "slow";

    shell_print(sh, "CPU frequency profile: %s", freq_str);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_cpu,
                               SHELL_CMD_ARG(freq, NULL, "Show current CPU frequency profile", cmd_cpu_get_freq, 1, 0),
                               SHELL_SUBCMD_SET_END
                              );

SHELL_CMD_REGISTER(cpu, &sub_cpu, "CPU frequency commands", cmd_cpu_get_freq);

#ifdef CONFIG_RETENTION_BOOT_MODE

static void boot_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);
    sys_reboot(SYS_REBOOT_COLD);
}

K_WORK_DELAYABLE_DEFINE(boot_work, boot_work_handler);

static int cmd_boot(const struct shell *sh, size_t argc, char **argv)
{
    if (argc < 2 || strcmp(argv[1], "start") != 0) {
        shell_error(sh, "Usage: boot start");
        return -EINVAL;
    }

    shell_print(sh, "Preparing to enter serial recovery");
    int rc = bootmode_set(BOOT_MODE_TYPE_BOOTLOADER);
    if (rc != 0) {
        shell_error(sh, "Failed to set boot mode (%d)", rc);
        return rc;
    }

    shell_print(sh, "Rebooting into bootloader...");
    k_work_schedule(&boot_work, K_MSEC(500));

    return 0;
}

SHELL_CMD_REGISTER(boot, NULL, "Enter bootloader mode (start)", cmd_boot);

#endif /* CONFIG_RETENTION_BOOT_MODE */

/* --- app management commands --- */

static int cmd_app_list(const struct shell *sh, size_t argc, char **argv)
{
    int num_apps = zsw_app_manager_get_num_apps();

    shell_print(sh, "Registered apps (%d):", num_apps);
    for (int i = 0; i < num_apps; i++) {
        application_t *app = zsw_app_manager_get_app(i);
        if (app) {
            const char *state_str;
            switch (app->current_state) {
                case ZSW_APP_STATE_STOPPED:    state_str = "stopped"; break;
                case ZSW_APP_STATE_UI_VISIBLE: state_str = "visible"; break;
                case ZSW_APP_STATE_UI_HIDDEN:  state_str = "hidden";  break;
                default:                       state_str = "unknown"; break;
            }
            shell_print(sh, "  [%d] %s (%s)%s", i, app->name, state_str,
                        app->hidden ? " [hidden]" : "");
        }
    }
    return 0;
}

static int cmd_app_launch(const struct shell *sh, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_error(sh, "Usage: app launch <name>");
        return -EINVAL;
    }

    /* Concatenate all remaining args to support app names with spaces */
    static char app_name_buf[64];
    app_name_buf[0] = '\0';
    for (int i = 1; i < argc; i++) {
        if (i > 1) {
            strncat(app_name_buf, " ", sizeof(app_name_buf) - strlen(app_name_buf) - 1);
        }
        strncat(app_name_buf, argv[i], sizeof(app_name_buf) - strlen(app_name_buf) - 1);
    }

    if (zsw_ui_controller_get_state() != ZSW_UI_STATE_WATCHFACE) {
        shell_error(sh, "Cannot launch app: not on watchface");
        return -EBUSY;
    }

    shell_print(sh, "Launching app: %s", app_name_buf);
    zsw_ui_controller_launch_app(app_name_buf);
    return 0;
}

static int cmd_app_close(const struct shell *sh, size_t argc, char **argv)
{
    if (zsw_ui_controller_get_state() != ZSW_UI_STATE_APP_MANAGER) {
        shell_error(sh, "No app running");
        return -EINVAL;
    }

    shell_print(sh, "Closing current app");
    zsw_app_manager_exit_app();
    return 0;
}

static int cmd_app_state(const struct shell *sh, size_t argc, char **argv)
{
    const char *state_str;
    switch (zsw_ui_controller_get_state()) {
        case ZSW_UI_STATE_INIT:        state_str = "init"; break;
        case ZSW_UI_STATE_WATCHFACE:   state_str = "watchface"; break;
        case ZSW_UI_STATE_APP_MANAGER: state_str = "app_manager"; break;
        default:                       state_str = "unknown"; break;
    }

    const char *running_app = zsw_ui_controller_get_running_app_name();
    shell_print(sh, "UI state: %s", state_str);
    if (running_app) {
        shell_print(sh, "Running app: %s", running_app);
    }
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_app,
    SHELL_CMD_ARG(list,   NULL, "List all registered apps", cmd_app_list, 1, 0),
    SHELL_CMD_ARG(launch, NULL, "Launch app by name: app launch <name>", cmd_app_launch, 2, 10),
    SHELL_CMD_ARG(close,  NULL, "Close the currently running app", cmd_app_close, 1, 0),
    SHELL_CMD_ARG(state,  NULL, "Show current UI state and running app", cmd_app_state, 1, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(app, &sub_app, "App management commands", NULL);

/* --- input simulation commands --- */

static int cmd_input_button(const struct shell *sh, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_error(sh, "Usage: input button <1|2|3|4>");
        return -EINVAL;
    }

    uint16_t key_code;
    switch (atoi(argv[1])) {
        case 1: key_code = INPUT_KEY_1; break;
        case 2: key_code = INPUT_KEY_2; break;
        case 3: key_code = INPUT_KEY_3; break;
        case 4: key_code = INPUT_KEY_4; break;
        default:
            shell_error(sh, "Invalid button (expected 1-4)");
            return -EINVAL;
    }

    input_report_key(NULL, key_code, 0, false, K_FOREVER);
    k_msleep(50);
    input_report_key(NULL, key_code, 1, false, K_FOREVER);

    shell_print(sh, "Button %s pressed", argv[1]);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_input,
    SHELL_CMD_ARG(button, NULL, "Simulate button press: input button <1|2|3|4>",
                  cmd_input_button, 2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(input, &sub_input, "Input simulation commands", NULL);

/* --- event injection commands --- */

static int cmd_event_battery(const struct shell *sh, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_error(sh, "Usage: event battery <percent> [charging:0|1]");
        return -EINVAL;
    }

    int percent = atoi(argv[1]);
    bool charging = (argc > 2 && strcmp(argv[2], "1") == 0);

    struct battery_sample_event evt = {
        .mV = 3700 + (percent * 5),
        .percent = percent,
        .temperature = 25.0f,
        .avg_current = charging ? -0.1f : 0.01f,
        .tte = charging ? 0 : (float)(percent * 6),
        .ttf = charging ? (float)((100 - percent) * 3) : 0,
        .is_charging = charging,
        .pmic_data_valid = true,
    };

    int ret = zbus_chan_pub(&battery_sample_data_chan, &evt, K_MSEC(100));
    if (ret != 0) {
        shell_error(sh, "Failed to publish battery event: %d", ret);
        return ret;
    }

    shell_print(sh, "Injected battery: %d%%, charging=%s", percent, charging ? "yes" : "no");
    return 0;
}

static int cmd_event_pressure(const struct shell *sh, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_error(sh, "Usage: event pressure <hPa> [temperature_C]");
        return -EINVAL;
    }

    struct pressure_event evt = {
        .pressure = strtof(argv[1], NULL),
        .temperature = (argc > 2) ? strtof(argv[2], NULL) : 25.0f,
    };

    int ret = zbus_chan_pub(&pressure_data_chan, &evt, K_MSEC(100));
    if (ret != 0) {
        shell_error(sh, "Failed to publish pressure event: %d", ret);
        return ret;
    }

    shell_print(sh, "Injected pressure: %.2f hPa, temp: %.1f C",
                (double)evt.pressure, (double)evt.temperature);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_event,
    SHELL_CMD_ARG(battery,  NULL, "Inject battery event: event battery <percent> [charging:0|1]",
                  cmd_event_battery, 2, 1),
    SHELL_CMD_ARG(pressure, NULL, "Inject pressure event: event pressure <hPa> [temp_C]",
                  cmd_event_pressure, 2, 1),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(event, &sub_event, "Event injection commands", NULL);
