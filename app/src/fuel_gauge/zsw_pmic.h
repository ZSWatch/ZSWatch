#ifndef __ZSW_PMIC__H__
#define __ZSW_PMIC__H__
#include "events/battery_event.h"

int zsw_pmic_get_full_state(struct battery_sample_event *sample);

#endif /* __ZSW_PMIC__H__ */
