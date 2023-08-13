#ifndef __DISPLAY_CONTROL_H_
#define __DISPLAY_CONTROL_H_
#include <inttypes.h>
#include <stdbool.h>

void display_control_init(void);
int display_control_sleep_ctrl(bool on);
int display_control_pwr_ctrl(bool on);
void display_control_set_brightness(uint8_t percent);
uint8_t display_control_get_brightness(void);
#endif