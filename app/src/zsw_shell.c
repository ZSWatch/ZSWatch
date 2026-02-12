#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/shell/shell.h>
#include <zephyr/settings/settings.h>
#include <zephyr/retention/bootmode.h>

#include "zsw_settings.h"
#include <zsw_coredump.h>
#include <zsw_cpu_freq.h>
#include "fuel_gauge/zsw_pmic.h"
#include "managers/zsw_power_manager.h"
#include "drivers/zsw_vibration_motor.h"
#include "drivers/zsw_display_control.h"

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
