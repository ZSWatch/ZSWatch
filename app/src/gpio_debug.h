#ifndef __GPIO_DEBUG_H
#define __GPIO_DEBUG_H

#include <zephyr/kernel.h>

/**
 * @brief Button id
 */
typedef enum gpioWatchId_t {
    TOUCH_RST,
    TOUCH_SDA,
    TOUCH_SCL,
    TOUCH_INT,
    DISPLAY_EN,
    DISPLAY_RST,
    BTN_1,
    DISPLAY_DC,
    QSPI_1_SO,
    V5_REG_EN,
    MAX30_INT,
    QSPI_0_SO,
    DRV_VIB_PWM,
    DRV_VIB_EN,
    DISPLAY_DATA,
    BAT_MON,
    BAT_MON_EN,
    INT1_LIS,
    INT2_LIS,
    BTN_2,
    QSPI_2_WP,
    DISPLAY_CLK,
    DISPLAY_CS,
    BTN_3,
    SDA_ANNA,
    SCL_ANNA,
    DISPLAY_BLK,
    QSPI_CS,
    QSPI_3_RST,
    QSPI_CLK,
} gpioWatchId_t;

void gpio_debug_init(void);
void gpio_debug_test_all(void);
void gpio_debug_test(gpioWatchId_t gpioId, int val);

#endif