/*
 * Copyright (c) 2024 Glenn Andrews
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SMF_CALCULATOR_THREAD_H_
#define SMF_CALCULATOR_THREAD_H_

#include <zephyr/kernel.h>

#define CALCULATOR_MAX_DIGITS 15

/* Add one byte for '-' sign, one for trailing '\0' */
#define CALCULATOR_STRING_LENGTH (CALCULATOR_MAX_DIGITS + 2)

enum calculator_events {
    DIGIT_0,
    DIGIT_1_9,
    DECIMAL_POINT,
    OPERATOR,
    EQUALS,
    CANCEL_ENTRY,
    CANCEL_BUTTON,
};

struct calculator_event {
    enum calculator_events event_id;
    char operand;
};

/* Reset the calculator and show its initial value. */
void calculator_smf_init(void);

/* Run the state machine for one button press. Call from the LVGL context. */
int post_calculator_event(const struct calculator_event *event);

#endif /* SMF_CALCULATOR_THREAD_H_ */
