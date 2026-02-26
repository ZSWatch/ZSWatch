#ifndef NRF54L15DK_NRF54L15_CONFIG_H__
#define NRF54L15DK_NRF54L15_CONFIG_H__

#define LOOPBACK_PIN_1A NRF_GPIO_PIN_MAP(1,13)
#define LOOPBACK_PIN_1B NRF_GPIO_PIN_MAP(1,12)
#define LOOPBACK_PIN_2A NRF_GPIO_PIN_MAP(1,11)
#define LOOPBACK_PIN_2B NRF_GPIO_PIN_MAP(1,10)
#define LOOPBACK_PIN_3A NRF_GPIO_PIN_MAP(1,9)
#define LOOPBACK_PIN_3B NRF_GPIO_PIN_MAP(1,8)
#define LOOPBACK_PIN_4A NRF_GPIO_PIN_MAP(2,6)
#define LOOPBACK_PIN_4B NRF_GPIO_PIN_MAP(1,4)

#define LED0_PIN NRF_GPIO_PIN_MAP(2,9)
#define LED1_PIN NRF_GPIO_PIN_MAP(1,10)
#define LED2_PIN NRF_GPIO_PIN_MAP(2,7)
#define LED3_PIN NRF_GPIO_PIN_MAP(1,14)

/* EXAMPLE-SPECIFIC DEFINES: */

/* NRFX_EGU */
/** @brief Symbol specifying EGU instance to be used in the nrfx_egu example. */
#define EGU_INST_IDX 10

/* NRFX_GPPI */
/** @brief Symbol specifying TIMER instance to be used in the nrfx_gppi examples. */
#define GPPI_TIMER_INST_IDX 20

/** @brief Symbol specifying GPIOTE instance to be used in the nrfx_gppi examples. */
#define GPPI_GPIOTE_INST_IDX 20

/** @brief Symbol specifying output pin associated with primary task in the nrfx_gpi examples. */
#define GPPI_OUTPUT_PIN_PRIMARY LED1_PIN

/** @brief Symbol specifying output pin associated with fork task in the nrfx_gppi examples. */
#define GPPI_OUTPUT_PIN_FORK LED3_PIN

/* NRFX_PWM */
/** @brief Symbol specifying PWM instance to be used in the nrfx_pwm examples. */
#define PWM_INST_IDX 20

/**
 * @note On nRF54L15 only two out of four LED pins are connected to port 1,
 *       which is accessible by PWM.
 */

/** @brief Symbol specifying a pin connected to LED to be used in the nrfx_pwm examples. */
#define PWM_LED1_PIN LED1_PIN
/** @brief Symbol specifying a pin connected to LED to be used in the nrfx_pwm examples. */
#define PWM_LED2_PIN NRF_PWM_PIN_NOT_CONNECTED
/** @brief Symbol specifying a pin connected to LED to be used in the nrfx_pwm examples. */
#define PWM_LED3_PIN LED3_PIN
/** @brief Symbol specifying a pin connected to LED to be used in the nrfx_pwm examples. */
#define PWM_LED4_PIN NRF_PWM_PIN_NOT_CONNECTED

/* NRFX_SAADC */
/** @brief Symbol specifying GPIOTE instance to be used in the nrfx_saadc examples. */
#define SAADC_GPIOTE_INST_IDX 20

/** @brief Symbol specifying timer instance to be used in the nrfx_saadc examples. */
#define SAADC_TIMER_INST_IDX 20
/**
 * @brief Symbol specifying the maximum number of SAADC channels that can be used in the
 *        nrfx_saadc examples.
 *
 * @note In order to keep the loopbacks consistent for all the examples only two SAADC
 *       channels are used on nRF54L15 DK.
 */
#define SAADC_MAX_CHANNELS 2

/** @brief Symbol specifying analog input to be observed by SAADC channel 0 in the nrfx_saadc examples. */
#define SAADC_CH0_AIN NRFX_ANALOG_EXTERNAL_AIN6
/** @brief Symbol specifying analog input to be observed by SAADC channel 1 in the nrfx_saadc examples. */
#define SAADC_CH1_AIN NRFX_ANALOG_EXTERNAL_AIN4

/** @brief Symbol specifying GPIO pin connected to SAADC channel 0 in the nrfx_saadc examples. */
#define SAADC_CH0_LOOPBACK_PIN LOOPBACK_PIN_1B
/** @brief Symbol specifying GPIO pin connected to SAADC channel 1 in the nrfx_saadc examples. */
#define SAADC_CH1_LOOPBACK_PIN LOOPBACK_PIN_2B

/* NRFX_SPIM */
/** @brief Symbol specifying SPIM instance to be used in nrfx_spim examples. */
#define SPIM_INST_IDX 21

/* NRFX_SPIM_SPIS */
/** @brief Symbol specifying SPIM instance to be used in nrfx_spim_spis examples. */
#define SPIM_SPIS_SPIM_INST_IDX 20
/** @brief Symbol specifying SPIS instance to be used in nrfx_spim_spis examples. */
#define SPIM_SPIS_SPIS_INST_IDX 21

/** @brief Symbol specifying master's pin number for MOSI in nrfx_spim_spis examples. */
#define SPIM_SPIS_MOSI_PIN_MASTER LOOPBACK_PIN_2A
/** @brief Symbol specifying slave's pin number for MOSI in nrfx_spim_spis examples. */
#define SPIM_SPIS_MOSI_PIN_SLAVE LOOPBACK_PIN_2B
/** @brief Symbol specifying master's pin number for MISO in nrfx_spim_spis examples. */
#define SPIM_SPIS_MISO_PIN_MASTER LOOPBACK_PIN_3A
/** @brief Symbol specifying slave's pin number for MISO in nrfx_spim_spis examples. */
#define SPIM_SPIS_MISO_PIN_SLAVE LOOPBACK_PIN_3B
/** @brief Symbol specifying master's pin number for SCK in nrfx_spim_spis examples. */
#define SPIM_SPIS_SCK_PIN_MASTER LOOPBACK_PIN_4B
/** @brief Symbol specifying slave's pin number for SCK in nrfx_spim_spis examples. */
#define SPIM_SPIS_SCK_PIN_SLAVE LOOPBACK_PIN_4A
/** @brief Symbol specifying master's pin number for SS in nrfx_spim_spis examples. */
#define SPIM_SPIS_SS_PIN_MASTER LOOPBACK_PIN_1A
/** @brief Symbol specifying slave's pin number for CSN in nrfx_spim_spis examples. */
#define SPIM_SPIS_CSN_PIN_SLAVE LOOPBACK_PIN_1B

/* NRFX_TIMER */
/** @brief Symbol specifying timer instance to be used in nrfx_timer/timer example. */
#define TIMER_INST_IDX 00

/** @brief Symbol specifying timer instance to be used in timer mode (T) in nrfx_timer/counter example. */
#define TIMER_T_INST_IDX 00
/** @brief Symbol specifying timer instance to be used in counter mode (C)  in nrfx_timer/counter example. */
#define TIMER_C_INST_IDX 20

/* NRFX_TWIM_TWIS */
/** @brief Symbol specifying TWIM instance to be used in nrfx_twim_twis examples. */
#define TWIM_INST_IDX 21
/** @brief Symbol specifying TWIS instance to be used in nrfx_twim_twis examples. */
#define TWIS_INST_IDX 22

/** @brief Symbol specifying pin number of master SCL to be used in nrfx_twim_twis examples. */
#define TWIM_TWIS_MASTER_SCL_PIN LOOPBACK_PIN_1A
/** @brief Symbol specifying pin number of slave SCL to be used in nrfx_twim_twis examples. */
#define TWIM_TWIS_SLAVE_SCL_PIN LOOPBACK_PIN_1B
/** @brief Symbol specifying pin number of master SDA to be used in nrfx_twim_twis examples. */
#define TWIM_TWIS_MASTER_SDA_PIN LOOPBACK_PIN_2A
/** @brief Symbol specifying pin number of slave SDA to be used in nrfx_twim_twis examples. */
#define TWIM_TWIS_SLAVE_SDA_PIN LOOPBACK_PIN_2B

/* NRFX_UARTE */
/** @brief Symbol specifying UARTE instance to be used in nrfx_uarte examples. */
#define UARTE_INST_IDX 21

/** @brief Symbol specifying TX pin number of UARTE to be used in nrfx_uarte examples. */
#define UARTE_TX_PIN LOOPBACK_PIN_1A
/** @brief Symbol specifying RX pin number of UARTE to be used in nrfx_uarte examples. */
#define UARTE_RX_PIN LOOPBACK_PIN_1B

#endif // NRF54L15DK_NRF54L15_CONFIG_H__
