#ifndef __ZSW_PMIC__H__
#define __ZSW_PMIC__H__
#include "events/battery_event.h"
#include <errno.h>
#include <zephyr/kernel.h>

#ifdef CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED

int zsw_pmic_get_full_state(struct battery_sample_event *sample);

/**
 * @brief Get whether VBUS (USB power) is connected to the PMIC.
 *
 * @return true if VBUS is connected, false otherwise.
 */
int zsw_pmic_get_vbus_connected(void);

/**
 * @brief Get a human-readable string for the charger status register value.
 *
 * @param status Raw BCHGCHARGESTATUS register value.
 * @return Pointer to a static string describing the charger status.
 */
const char *zsw_pmic_charger_status_str(int status);

/**
 * @brief Get a human-readable string for the charger error register value.
 *
 * @param error Raw BCHGERRREASON register value.
 * @return Pointer to a static string describing the charger error.
 */
const char *zsw_pmic_charger_error_str(int error);

/**
 * @brief Powers down everything. Enter lowest power mode.
 *
 * This function is used to power down the PMIC and enter it's "ship mode".
 * This consumes the least amount of power possible.
 *
 * To wakeup after this, the SW3 (Bottom right) needs to be held down for 10s.
 *
 * @return 0 if successful, -ENOTSUP if not possible due to charger connected, otherwise an error code.
 */
int zsw_pmic_power_down(void);

/**
 * @brief Power cycle the PMIC, causing a system reset.
 *
 * @return 0 if successful, otherwise an error code.
 */
int zsw_pmic_reset(void);

#else /* !CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED */

static inline int zsw_pmic_get_full_state(struct battery_sample_event *sample)
{
    ARG_UNUSED(sample);
    return -ENOTSUP;
}

static inline int zsw_pmic_get_vbus_connected(void)
{
    return 0;
}

static inline const char *zsw_pmic_charger_status_str(int status)
{
    ARG_UNUSED(status);
    return "N/A";
}

static inline const char *zsw_pmic_charger_error_str(int error)
{
    ARG_UNUSED(error);
    return "N/A";
}

static inline int zsw_pmic_power_down(void)
{
    return -ENOTSUP;
}

static inline int zsw_pmic_reset(void)
{
    return -ENOTSUP;
}

#endif /* CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED */

#endif /* __ZSW_PMIC__H__ */
