#ifndef __ZSW_PMIC__H__
#define __ZSW_PMIC__H__
#include "events/battery_event.h"

int zsw_pmic_get_full_state(struct battery_sample_event *sample);

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

#endif /* __ZSW_PMIC__H__ */
