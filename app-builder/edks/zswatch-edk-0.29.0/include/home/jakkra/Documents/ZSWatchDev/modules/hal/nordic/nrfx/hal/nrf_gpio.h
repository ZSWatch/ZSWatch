/*
 * Copyright (c) 2015 - 2025, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NRF_GPIO_H__
#define NRF_GPIO_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NRF_P0
#define NRF_P0 NRF_GPIO
#endif

#define GPIO_PORT_NUM(periph_name, prefix, i, _)    i,
#define GPIO_REG(periph_name, prefix, i, _)         NRFX_CONCAT(NRF_, periph_name, prefix, i),
#define GPIO_NUM_OF_PINS(periph_name, prefix, i, _) \
    NRFX_CONCAT(periph_name, prefix, i, _PIN_NUM)

#define GPIO_PORT_NUM_LIST {NRFX_FOREACH_PRESENT(P, GPIO_PORT_NUM, (), (), _)}
#define GPIO_REG_LIST      {NRFX_FOREACH_PRESENT(P, GPIO_REG, (), (), _)}
#define NUMBER_OF_PINS     (NRFX_FOREACH_PRESENT(P, GPIO_NUM_OF_PINS, (+), (0), _))

#if !defined(GPIO_REG_LIST)
#error "Not supported."
#endif

#if defined(GPIO_PIN_CNF_DRIVE0_Msk)
#define GPIO_PIN_CNF_DRIVE1_OFFSET (GPIO_PIN_CNF_DRIVE1_Pos - GPIO_PIN_CNF_DRIVE0_Pos)
#endif

/*
 * Macro for generating case code blocks that return token NRF_<periph_name><prefix><i>
 * for case value equal to <i>.
 *
 * Used by NRF_INTERNAL_GPIO_PORT_EXTRACT.
 */
#define NRF_INTERNAL_GPIO_PORT_EXTRACT_1(periph_name, prefix, i, port) \
    case i:                                                            \
        port = NRFX_CONCAT(NRF_, periph_name, prefix, i);              \
        break;

/*
 * Macro for generating case code blocks for switch statement used in function nrf_gpio_pin_port_decode.
 * It allows extracting the port number relative to the decoded pin.
 */
#define NRF_INTERNAL_GPIO_PORT_EXTRACT(port) \
    NRFX_FOREACH_PRESENT(P, NRF_INTERNAL_GPIO_PORT_EXTRACT_1, (), (), port)

/*
 * Macro for generating case code blocks that set mask to <periph_name><prefix><i>_FEATURE_PINS_PRESENT
 * for case value equal to <i>.
 *
 * Used by NRF_INTERNAL_GPIO_PORT_MASK_SET.
 */
#define NRF_INTERNAL_GPIO_PORT_MASK_SET_1(periph_name, prefix, i, mask)    \
    case i:                                                                \
        mask = NRFX_CONCAT(periph_name, prefix, i, _FEATURE_PINS_PRESENT); \
        break;

/*
 * Macro for generating case code blocks for switch statement used in function nrf_gpio_pin_present_check.
 * It allows setting the mask to a value associated with the specific port.
 */
#define NRF_INTERNAL_GPIO_PORT_MASK_SET(mask) \
    NRFX_FOREACH_PRESENT(P, NRF_INTERNAL_GPIO_PORT_MASK_SET_1, (), (), mask)

/**
 * @defgroup nrf_gpio_hal GPIO HAL
 * @{
 * @ingroup nrf_gpio
 * @brief   Hardware access layer for managing the GPIO peripheral.
 */

#if defined(GPIO_LATCH_PIN0_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the functionality of latching GPIO state change is present. */
#define NRF_GPIO_LATCH_PRESENT
#endif

#if defined(GPIO_PIN_CNF_MCUSEL_Msk) || defined(GPIO_PIN_CNF_CTRLSEL_Msk) \
    || defined(__NRFX_DOXYGEN__)
/** @brief Presence of MCU/Subsystem control selection. */
#define NRF_GPIO_HAS_SEL 1
#else
#define NRF_GPIO_HAS_SEL 0
#endif

#if defined(GPIO_PIN_CNF_CTRLSEL_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of MCU/Subsystem control selection for multiple peripherals. */
#define NRF_GPIO_HAS_MULTIPERIPH_SEL 1
#else
#define NRF_GPIO_HAS_MULTIPERIPH_SEL 0
#endif

#if defined(GPIO_PIN_CNF_CLOCKPIN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of clock pin enable. */
#define NRF_GPIO_HAS_CLOCKPIN 1
#else
#define NRF_GPIO_HAS_CLOCKPIN 0
#endif

#if defined(GPIO_PORTCNF_DRIVECTRL_IMPEDANCE50_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of drive control for impedance. */
#define NRF_GPIO_HAS_PORT_IMPEDANCE 1
#else
#define NRF_GPIO_HAS_PORT_IMPEDANCE 0
#endif

#if defined(GPIO_RETAIN_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of register retention. */
#define NRF_GPIO_HAS_RETENTION 1
#else
#define NRF_GPIO_HAS_RETENTION 0
#endif

#if (defined(GPIO_RETAINSET_ResetValue) && defined(GPIO_RETAINCLR_ResetValue)) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Presence of register retention set/clear. */
#define NRF_GPIO_HAS_RETENTION_SETCLEAR 1
#else
#define NRF_GPIO_HAS_RETENTION_SETCLEAR 0
#endif

#if defined(GPIO_DETECTMODE_DETECTMODE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of detect mode. */
#define NRF_GPIO_HAS_DETECT_MODE 1
#else
#define NRF_GPIO_HAS_DETECT_MODE 0
#endif

#if defined(GPIO_PIN_CNF_DRIVE_E0E1) || defined(GPIO_PIN_CNF_DRIVE0_E0) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of extra high pin drive mode. */
#define NRF_GPIO_HAS_DRIVE_EXTRA 1
#else
#define NRF_GPIO_HAS_DRIVE_EXTRA 0
#endif

#if defined(GPIO_PIN_CNF_CTRLSEL_GRTC) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of control selection for GRTC. */
#define NRF_GPIO_HAS_CTRLSEL_GRTC 1
#else
#define NRF_GPIO_HAS_CTRLSEL_GRTC 0
#endif

/** @brief Macro for getting number of pins in the port */
#define NRF_GPIO_PIN_NUM(periph, prefix, i, _) NRFX_CONCAT(periph, prefix, i, _PIN_NUM)

#if defined(GPIOTE_SPARSE_PINS) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying total number of GPIO pins across all ports. */
#define NRF_GPIO_MAX_PIN_NUMBER 32
#else
#define NRF_GPIO_MAX_PIN_NUMBER NRFX_FOREACH_PRESENT(P, NRF_GPIO_PIN_NUM, (+), (0), _)
#endif

/** @brief Macro for mapping port and pin numbers to values understandable for nrf_gpio functions. */
#define NRF_GPIO_PIN_MAP(port, pin) NRF_PIN_PORT_TO_PIN_NUMBER(pin, port)

#if NRF_GPIO_HAS_PORT_IMPEDANCE
/** @brief Mask of all impedances. */
#define NRF_GPIO_PORT_IMPEDANCE_ALL_MASK (GPIO_PORTCNF_DRIVECTRL_IMPEDANCE50_Msk  | \
                                          GPIO_PORTCNF_DRIVECTRL_IMPEDANCE100_Msk | \
                                          GPIO_PORTCNF_DRIVECTRL_IMPEDANCE200_Msk | \
                                          GPIO_PORTCNF_DRIVECTRL_IMPEDANCE400_Msk | \
                                          GPIO_PORTCNF_DRIVECTRL_IMPEDANCE800_Msk | \
                                          GPIO_PORTCNF_DRIVECTRL_IMPEDANCE1600_Msk)
#endif

/** @brief Pin direction definitions. */
typedef enum
{
    NRF_GPIO_PIN_DIR_INPUT  = GPIO_PIN_CNF_DIR_Input, ///< Input.
    NRF_GPIO_PIN_DIR_OUTPUT = GPIO_PIN_CNF_DIR_Output ///< Output.
} nrf_gpio_pin_dir_t;

/** @brief Connection of input buffer. */
typedef enum
{
    NRF_GPIO_PIN_INPUT_CONNECT    = GPIO_PIN_CNF_INPUT_Connect,   ///< Connect input buffer.
    NRF_GPIO_PIN_INPUT_DISCONNECT = GPIO_PIN_CNF_INPUT_Disconnect ///< Disconnect input buffer.
} nrf_gpio_pin_input_t;

/**
 * @brief Enumerator used for selecting the pin to be pulled down or up at the time of pin
 * configuration.
 */
typedef enum
{
    NRF_GPIO_PIN_NOPULL   = GPIO_PIN_CNF_PULL_Disabled, ///<  Pin pull-up resistor disabled.
    NRF_GPIO_PIN_PULLDOWN = GPIO_PIN_CNF_PULL_Pulldown, ///<  Pin pull-down resistor enabled.
    NRF_GPIO_PIN_PULLUP   = GPIO_PIN_CNF_PULL_Pullup,   ///<  Pin pull-up resistor enabled.
} nrf_gpio_pin_pull_t;

/** @brief Enumerator used for selecting output drive mode. */
typedef enum
{
#if defined(GPIO_PIN_CNF_DRIVE_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_GPIO_PIN_S0S1 = GPIO_PIN_CNF_DRIVE_S0S1, ///< Standard '0', standard '1'.
    NRF_GPIO_PIN_H0S1 = GPIO_PIN_CNF_DRIVE_H0S1, ///< High drive '0', standard '1'.
    NRF_GPIO_PIN_S0H1 = GPIO_PIN_CNF_DRIVE_S0H1, ///< Standard '0', high drive '1'.
    NRF_GPIO_PIN_H0H1 = GPIO_PIN_CNF_DRIVE_H0H1, ///< High drive '0', high drive '1'.
    NRF_GPIO_PIN_D0S1 = GPIO_PIN_CNF_DRIVE_D0S1, ///< Disconnect '0' standard '1'.
    NRF_GPIO_PIN_D0H1 = GPIO_PIN_CNF_DRIVE_D0H1, ///< Disconnect '0', high drive '1'.
    NRF_GPIO_PIN_S0D1 = GPIO_PIN_CNF_DRIVE_S0D1, ///< Standard '0', disconnect '1'.
    NRF_GPIO_PIN_H0D1 = GPIO_PIN_CNF_DRIVE_H0D1, ///< High drive '0', disconnect '1'.
#if defined(GPIO_PIN_CNF_DRIVE_E0E1) || defined(__NRFX_DOXYGEN__)
    NRF_GPIO_PIN_E0E1 = GPIO_PIN_CNF_DRIVE_E0E1, ///< Extra high drive '0', extra high drive '1'.
#endif
#else
    NRF_GPIO_PIN_S0S1 = GPIO_PIN_CNF_DRIVE0_S0 |
                        (GPIO_PIN_CNF_DRIVE1_S1 << GPIO_PIN_CNF_DRIVE1_OFFSET),
    NRF_GPIO_PIN_H0S1 = GPIO_PIN_CNF_DRIVE0_H0 |
                        (GPIO_PIN_CNF_DRIVE1_S1 << GPIO_PIN_CNF_DRIVE1_OFFSET),
    NRF_GPIO_PIN_S0H1 = GPIO_PIN_CNF_DRIVE0_S0 |
                        (GPIO_PIN_CNF_DRIVE1_H1 << GPIO_PIN_CNF_DRIVE1_OFFSET),
    NRF_GPIO_PIN_H0H1 = GPIO_PIN_CNF_DRIVE0_H0 |
                        (GPIO_PIN_CNF_DRIVE1_H1 << GPIO_PIN_CNF_DRIVE1_OFFSET),
    NRF_GPIO_PIN_D0S1 = GPIO_PIN_CNF_DRIVE0_D0 |
                        (GPIO_PIN_CNF_DRIVE1_S1 << GPIO_PIN_CNF_DRIVE1_OFFSET),
    NRF_GPIO_PIN_D0H1 = GPIO_PIN_CNF_DRIVE0_D0 |
                        (GPIO_PIN_CNF_DRIVE1_H1 << GPIO_PIN_CNF_DRIVE1_OFFSET),
    NRF_GPIO_PIN_S0D1 = GPIO_PIN_CNF_DRIVE0_S0 |
                        (GPIO_PIN_CNF_DRIVE1_D1 << GPIO_PIN_CNF_DRIVE1_OFFSET),
    NRF_GPIO_PIN_H0D1 = GPIO_PIN_CNF_DRIVE0_H0 |
                        (GPIO_PIN_CNF_DRIVE1_D1 << GPIO_PIN_CNF_DRIVE1_OFFSET),
    NRF_GPIO_PIN_E0E1 = GPIO_PIN_CNF_DRIVE0_E0 |
                        (GPIO_PIN_CNF_DRIVE1_E1 << GPIO_PIN_CNF_DRIVE1_OFFSET),
#endif // defined(GPIO_PIN_CNF_DRIVE_Msk) || defined(__NRFX_DOXYGEN__)
} nrf_gpio_pin_drive_t;

/** @brief Enumerator used for selecting the pin to sense high or low level on the pin input. */
typedef enum
{
    NRF_GPIO_PIN_NOSENSE    = GPIO_PIN_CNF_SENSE_Disabled, ///<  Pin sense level disabled.
    NRF_GPIO_PIN_SENSE_LOW  = GPIO_PIN_CNF_SENSE_Low,      ///<  Pin sense low level.
    NRF_GPIO_PIN_SENSE_HIGH = GPIO_PIN_CNF_SENSE_High,     ///<  Pin sense high level.
} nrf_gpio_pin_sense_t;

#if NRF_GPIO_HAS_SEL
/** @brief Enumerator used for selecting the MCU/Subsystem to control the specified pin. */
typedef enum
{
#if defined(GPIO_PIN_CNF_MCUSEL_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_GPIO_PIN_SEL_APP        = GPIO_PIN_CNF_MCUSEL_AppMCU,     ///< Pin controlled by Application MCU.
    NRF_GPIO_PIN_SEL_NETWORK    = GPIO_PIN_CNF_MCUSEL_NetworkMCU, ///< Pin controlled by Network MCU.
    NRF_GPIO_PIN_SEL_PERIPHERAL = GPIO_PIN_CNF_MCUSEL_Peripheral, ///< Pin controlled by dedicated peripheral.
#endif
#if defined(GPIO_PIN_CNF_MCUSEL_TND) || defined(__NRFX_DOXYGEN__)
    NRF_GPIO_PIN_SEL_TND        = GPIO_PIN_CNF_MCUSEL_TND,        ///< Pin controlled by Trace and Debug Subsystem.
#elif defined(GPIO_PIN_CNF_CTRLSEL_TND)
    NRF_GPIO_PIN_SEL_TND        = GPIO_PIN_CNF_CTRLSEL_TND,       ///< Pin controlled by Trace and Debug Subsystem.
#endif
#if defined(GPIO_PIN_CNF_CTRLSEL_GPIO) || defined(__NRFX_DOXYGEN__)
    NRF_GPIO_PIN_SEL_GPIO       = GPIO_PIN_CNF_CTRLSEL_GPIO,      ///< Pin controlled by GPIO or peripherals with configurable pins.
#endif
#if defined(GPIO_PIN_CNF_CTRLSEL_VPR) || defined(__NRFX_DOXYGEN__)
    NRF_GPIO_PIN_SEL_VPR        = GPIO_PIN_CNF_CTRLSEL_VPR,       ///< Pin controlled by VPR.
#endif
#if defined(GPIO_PIN_CNF_CTRLSEL_QSPI) || defined(__NRFX_DOXYGEN__)
    NRF_GPIO_PIN_SEL_QSPI       = GPIO_PIN_CNF_CTRLSEL_QSPI,      ///< Pin controlled by QSPI peripheral.
#endif
#if defined(GPIO_PIN_CNF_CTRLSEL_GRTC) || defined(__NRFX_DOXYGEN__)
    NRF_GPIO_PIN_SEL_GRTC       = GPIO_PIN_CNF_CTRLSEL_GRTC,      ///< Pin controlled by GRTC peripheral.
#endif
#if defined(NRF_GPIO_PIN_SEL_EXT)
    NRF_GPIO_PIN_SEL_EXT
#endif
} nrf_gpio_pin_sel_t;
#endif // NRF_GPIO_HAS_SEL

#if NRF_GPIO_HAS_PORT_IMPEDANCE
/** @brief Port impedance enable mask. */
typedef enum
{
    NRF_GPIO_PORT_IMPEDANCE_50_MASK   = GPIO_PORTCNF_DRIVECTRL_IMPEDANCE50_Msk,   ///< Enable 50 Ohm impedance.
    NRF_GPIO_PORT_IMPEDANCE_100_MASK  = GPIO_PORTCNF_DRIVECTRL_IMPEDANCE100_Msk,  ///< Enable 100 Ohm impedance.
    NRF_GPIO_PORT_IMPEDANCE_200_MASK  = GPIO_PORTCNF_DRIVECTRL_IMPEDANCE200_Msk,  ///< Enable 200 Ohm impedance.
    NRF_GPIO_PORT_IMPEDANCE_400_MASK  = GPIO_PORTCNF_DRIVECTRL_IMPEDANCE400_Msk,  ///< Enable 400 Ohm impedance.
    NRF_GPIO_PORT_IMPEDANCE_800_MASK  = GPIO_PORTCNF_DRIVECTRL_IMPEDANCE800_Msk,  ///< Enable 800 Ohm impedance.
    NRF_GPIO_PORT_IMPEDANCE_1600_MASK = GPIO_PORTCNF_DRIVECTRL_IMPEDANCE1600_Msk, ///< Enable 1600 Ohm impedance.
} nrf_gpio_port_impedance_mask_t;
#endif

#if NRF_GPIO_HAS_RETENTION
/** @brief Retention enable mask. */
typedef enum
{
#if defined(GPIO_RETAIN_APPLICAION_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_GPIO_RETAIN_APPLICATION_MASK = GPIO_RETAIN_APPLICAION_Msk, ///< Enable retention for GPIO registers for Application domain
#endif
#if defined(GPIO_RETAIN_RADIOCORE_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_GPIO_RETAIN_NETWORK_MASK     = GPIO_RETAIN_RADIOCORE_Msk,  ///< Enable retention for GPIO registers for Radio core
#endif
#if defined(GPIO_RETAIN_PIN0_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_GPIO_RETAIN_PIN0_MASK        = GPIO_RETAIN_PIN0_Msk,       ///< Enable retention for pin 0.
    NRF_GPIO_RETAIN_PIN1_MASK        = GPIO_RETAIN_PIN1_Msk,       ///< Enable retention for pin 1.
    NRF_GPIO_RETAIN_PIN2_MASK        = GPIO_RETAIN_PIN2_Msk,       ///< Enable retention for pin 2.
    NRF_GPIO_RETAIN_PIN3_MASK        = GPIO_RETAIN_PIN3_Msk,       ///< Enable retention for pin 3.
    NRF_GPIO_RETAIN_PIN4_MASK        = GPIO_RETAIN_PIN4_Msk,       ///< Enable retention for pin 4.
    NRF_GPIO_RETAIN_PIN5_MASK        = GPIO_RETAIN_PIN5_Msk,       ///< Enable retention for pin 5.
    NRF_GPIO_RETAIN_PIN6_MASK        = GPIO_RETAIN_PIN6_Msk,       ///< Enable retention for pin 6.
    NRF_GPIO_RETAIN_PIN7_MASK        = GPIO_RETAIN_PIN7_Msk,       ///< Enable retention for pin 7.
    NRF_GPIO_RETAIN_PIN8_MASK        = GPIO_RETAIN_PIN8_Msk,       ///< Enable retention for pin 8.
    NRF_GPIO_RETAIN_PIN9_MASK        = GPIO_RETAIN_PIN9_Msk,       ///< Enable retention for pin 9.
    NRF_GPIO_RETAIN_PIN10_MASK       = GPIO_RETAIN_PIN10_Msk,      ///< Enable retention for pin 10.
    NRF_GPIO_RETAIN_PIN11_MASK       = GPIO_RETAIN_PIN11_Msk,      ///< Enable retention for pin 11.
    NRF_GPIO_RETAIN_PIN12_MASK       = GPIO_RETAIN_PIN12_Msk,      ///< Enable retention for pin 12.
    NRF_GPIO_RETAIN_PIN13_MASK       = GPIO_RETAIN_PIN13_Msk,      ///< Enable retention for pin 13.
    NRF_GPIO_RETAIN_PIN14_MASK       = GPIO_RETAIN_PIN14_Msk,      ///< Enable retention for pin 14.
    NRF_GPIO_RETAIN_PIN15_MASK       = GPIO_RETAIN_PIN15_Msk,      ///< Enable retention for pin 15.
    NRF_GPIO_RETAIN_PIN16_MASK       = GPIO_RETAIN_PIN16_Msk,      ///< Enable retention for pin 16.
    NRF_GPIO_RETAIN_PIN17_MASK       = GPIO_RETAIN_PIN17_Msk,      ///< Enable retention for pin 17.
    NRF_GPIO_RETAIN_PIN18_MASK       = GPIO_RETAIN_PIN18_Msk,      ///< Enable retention for pin 18.
    NRF_GPIO_RETAIN_PIN19_MASK       = GPIO_RETAIN_PIN19_Msk,      ///< Enable retention for pin 19.
    NRF_GPIO_RETAIN_PIN20_MASK       = GPIO_RETAIN_PIN20_Msk,      ///< Enable retention for pin 20.
    NRF_GPIO_RETAIN_PIN21_MASK       = GPIO_RETAIN_PIN21_Msk,      ///< Enable retention for pin 21.
    NRF_GPIO_RETAIN_PIN22_MASK       = GPIO_RETAIN_PIN22_Msk,      ///< Enable retention for pin 22.
    NRF_GPIO_RETAIN_PIN23_MASK       = GPIO_RETAIN_PIN23_Msk,      ///< Enable retention for pin 23.
    NRF_GPIO_RETAIN_PIN24_MASK       = GPIO_RETAIN_PIN24_Msk,      ///< Enable retention for pin 24.
    NRF_GPIO_RETAIN_PIN25_MASK       = GPIO_RETAIN_PIN25_Msk,      ///< Enable retention for pin 25.
    NRF_GPIO_RETAIN_PIN26_MASK       = GPIO_RETAIN_PIN26_Msk,      ///< Enable retention for pin 26.
    NRF_GPIO_RETAIN_PIN27_MASK       = GPIO_RETAIN_PIN27_Msk,      ///< Enable retention for pin 27.
    NRF_GPIO_RETAIN_PIN28_MASK       = GPIO_RETAIN_PIN28_Msk,      ///< Enable retention for pin 28.
    NRF_GPIO_RETAIN_PIN29_MASK       = GPIO_RETAIN_PIN29_Msk,      ///< Enable retention for pin 29.
    NRF_GPIO_RETAIN_PIN30_MASK       = GPIO_RETAIN_PIN30_Msk,      ///< Enable retention for pin 30.
    NRF_GPIO_RETAIN_PIN31_MASK       = GPIO_RETAIN_PIN31_Msk,      ///< Enable retention for pin 31.
#endif
#if defined(NRF_GPIO_RETAIN_EXT)
    NRF_GPIO_RETAIN_EXT
#endif
} nrf_gpio_retain_mask_t;
#endif

/**
 * @brief Function for configuring the GPIO pin range as output pins with normal drive strength.
 *        This function can be used to configure pin range as simple output with gate driving GPIO_PIN_CNF_DRIVE_S0S1 (normal cases).
 *
 * @note For configuring only one pin as output, use @ref nrf_gpio_cfg_output.
 *       Sense capability on the pin is disabled and input is disconnected from the buffer as the pins are configured as output.
 *
 * @param pin_range_start  Specifies the start number (inclusive) in the range of pin numbers to be configured.
 * @param pin_range_end    Specifies the end number (inclusive) in the range of pin numbers to be configured.
 */
NRF_STATIC_INLINE void nrf_gpio_range_cfg_output(uint32_t pin_range_start, uint32_t pin_range_end);

/**
 * @brief Function for configuring the GPIO pin range as input pins with given initial value set, hiding inner details.
 *        This function can be used to configure pin range as simple input.
 *
 * @note  For configuring only one pin as input, use @ref nrf_gpio_cfg_input.
 *        Sense capability on the pin is disabled and input is connected to buffer so that the GPIO->IN register is readable.
 *
 * @param pin_range_start  Specifies the start number (inclusive) in the range of pin numbers to be configured.
 * @param pin_range_end    Specifies the end number (inclusive) in the range of pin numbers to be configured.
 * @param pull_config      State of the pin range pull resistor (no pull, pulled down, or pulled high).
 */
NRF_STATIC_INLINE void nrf_gpio_range_cfg_input(uint32_t            pin_range_start,
                                                uint32_t            pin_range_end,
                                                nrf_gpio_pin_pull_t pull_config);

/**
 * @brief Pin configuration function.
 *
 * The main pin configuration function.
 * This function allows to set any aspect in PIN_CNF register.
 *
 * @param pin_number Specifies the pin number.
 * @param dir        Pin direction.
 * @param input      Connect or disconnect the input buffer.
 * @param pull       Pull configuration.
 * @param drive      Drive configuration.
 * @param sense      Pin sensing mechanism.
 */
NRF_STATIC_INLINE void nrf_gpio_cfg(
    uint32_t             pin_number,
    nrf_gpio_pin_dir_t   dir,
    nrf_gpio_pin_input_t input,
    nrf_gpio_pin_pull_t  pull,
    nrf_gpio_pin_drive_t drive,
    nrf_gpio_pin_sense_t sense);

/**
 * @brief Function for reconfiguring pin.
 *
 * @note This function selectively updates fields in PIN_CNF register. Reconfiguration
 *       is performed in single register write. Fields for which new configuration is
 *       not provided remain unchanged.
 *
 * @param pin_number Specifies the pin number.
 * @param p_dir      Pin direction. If NULL, previous setting remains.
 * @param p_input    Connect or disconnect the input buffer. If NULL, previous setting remains.
 * @param p_pull     Pull configuration. If NULL, previous setting remains.
 * @param p_drive    Drive configuration. If NULL, previous setting remains.
 * @param p_sense    Pin sensing mechanism. If NULL, previous setting remains.
 */
NRF_STATIC_INLINE void nrf_gpio_reconfigure(uint32_t                     pin_number,
                                            const nrf_gpio_pin_dir_t *   p_dir,
                                            const nrf_gpio_pin_input_t * p_input,
                                            const nrf_gpio_pin_pull_t *  p_pull,
                                            const nrf_gpio_pin_drive_t * p_drive,
                                            const nrf_gpio_pin_sense_t * p_sense);

/**
 * @brief Function for configuring the given GPIO pin number as output, hiding inner details.
 *        This function can be used to configure a pin as simple output with gate driving GPIO_PIN_CNF_DRIVE_S0S1 (normal cases).
 *
 * @note  Sense capability on the pin is disabled and input is disconnected from the buffer as the pins are configured as output.
 *
 * @param pin_number Specifies the pin number.
 */
NRF_STATIC_INLINE void nrf_gpio_cfg_output(uint32_t pin_number);

/**
 * @brief Function for configuring the given GPIO pin number from a given port as output, hiding inner details.
 *        This function can be used to configure a pin as simple output with gate driving GPIO_PIN_CNF_DRIVE_S0S1 (normal cases).
 *
 * @note  Sense capability on the pin is disabled and input is disconnected from the buffer as the pins are configured as output.
 *
 * @param p_reg      Pointer to the structure of registers of the peripheral.
 * @param pin_number Specifies the relative pin number.
 */
NRF_STATIC_INLINE void nrf_gpio_port_pin_output_set(NRF_GPIO_Type * p_reg, uint32_t pin_number);

/**
 * @brief Function for configuring the given GPIO pin number as input, hiding inner details.
 *        This function can be used to configure a pin as simple input.
 *
 * @note  Sense capability on the pin is disabled and input is connected to buffer so that the GPIO->IN register is readable.
 *
 * @param pin_number  Specifies the pin number.
 * @param pull_config State of the pin range pull resistor (no pull, pulled down, or pulled high).
 */
NRF_STATIC_INLINE void nrf_gpio_cfg_input(uint32_t pin_number, nrf_gpio_pin_pull_t pull_config);

/**
 * @brief Function for configuring the given GPIO pin number from a given port as input, hiding inner details.
 *        This function can be used to configure a pin as simple input.
 *
 * @note  Sense capability on the pin is disabled and input is connected to buffer so that the GPIO->IN register is readable.
 *
 * @param p_reg       Pointer to the structure of registers of the peripheral.
 * @param pin_number  Specifies the relative pin number.
 * @param pull_config State of the pin range pull resistor (no pull, pulled down, or pulled high).
 */
NRF_STATIC_INLINE void nrf_gpio_port_pin_input_set(NRF_GPIO_Type *     p_reg,
                                                   uint32_t            pin_number,
                                                   nrf_gpio_pin_pull_t pull_config);

/**
 * @brief Function for resetting pin configuration to its default state.
 *
 * @param pin_number Specifies the pin number.
 */
NRF_STATIC_INLINE void nrf_gpio_cfg_default(uint32_t pin_number);

/**
 * @brief Function for configuring the given GPIO pin number as a watcher. Only input is connected.
 *
 * @param pin_number Specifies the pin number.
 *
 */
NRF_STATIC_INLINE void nrf_gpio_cfg_watcher(uint32_t pin_number);

/**
 * @brief Function for disconnecting input for the given GPIO.
 *
 * @param pin_number Specifies the pin number.
 */
NRF_STATIC_INLINE void nrf_gpio_input_disconnect(uint32_t pin_number);

/**
 * @brief Function for configuring the given GPIO pin number as input, hiding inner details.
 *        This function can be used to configure pin range as simple input.
 *        Sense capability on the pin is configurable and input is connected to buffer so that the GPIO->IN register is readable.
 *
 * @param pin_number   Specifies the pin number.
 * @param pull_config  State of the pin pull resistor (no pull, pulled down, or pulled high).
 * @param sense_config Sense level of the pin (no sense, sense low, or sense high).
 */
NRF_STATIC_INLINE void nrf_gpio_cfg_sense_input(uint32_t             pin_number,
                                                nrf_gpio_pin_pull_t  pull_config,
                                                nrf_gpio_pin_sense_t sense_config);

/**
 * @brief Function for configuring sense level for the given GPIO.
 *
 * @param pin_number   Specifies the pin number.
 * @param sense_config Sense configuration.
 */
NRF_STATIC_INLINE void nrf_gpio_cfg_sense_set(uint32_t             pin_number,
                                              nrf_gpio_pin_sense_t sense_config);

/**
 * @brief Function for setting the direction for a GPIO pin.
 *
 * @param pin_number Specifies the pin number for which to set the direction.
 * @param direction  Specifies the direction.
 */
NRF_STATIC_INLINE void nrf_gpio_pin_dir_set(uint32_t pin_number, nrf_gpio_pin_dir_t direction);

/**
 * @brief Function for setting a GPIO pin.
 *
 * @param pin_number Specifies the pin number to be set.
 */
NRF_STATIC_INLINE void nrf_gpio_pin_set(uint32_t pin_number);

/**
 * @brief Function for clearing a GPIO pin.
 *
 * @param pin_number Specifies the pin number to clear.
 */
NRF_STATIC_INLINE void nrf_gpio_pin_clear(uint32_t pin_number);

/**
 * @brief Function for toggling a GPIO pin.
 *
 * @param pin_number Specifies the pin number to toggle.
 */
NRF_STATIC_INLINE void nrf_gpio_pin_toggle(uint32_t pin_number);

/**
 * @brief Function for writing a value to a GPIO pin.
 *
 * @param pin_number Specifies the pin number to write.
 * @param value      Specifies the value to be written to the pin.
 * @arg 0 Clears the pin.
 * @arg >=1 Sets the pin.
 */
NRF_STATIC_INLINE void nrf_gpio_pin_write(uint32_t pin_number, uint32_t value);

/**
 * @brief Function for writing a value to a GPIO pin of a given port.
 *
 * @param p_reg      Pointer to the structure of registers of the peripheral.
 * @param pin_number Specifies the relative pin number to write.
 * @param value      Specifies the value to be written to the pin.
 * @arg 0 Clears the pin.
 * @arg >=1 Sets the pin.
 */
NRF_STATIC_INLINE void nrf_gpio_port_pin_write(NRF_GPIO_Type * p_reg,
                                               uint32_t        pin_number,
                                               uint32_t        value);

/**
 * @brief Function for reading the input level of a GPIO pin.
 *
 * If the value returned by this function is to be valid, the pin's input buffer must be connected.
 *
 * @param pin_number Specifies the pin number to read.
 *
 * @return 0 if the pin input level is low. Positive value if the pin is high.
 */
NRF_STATIC_INLINE uint32_t nrf_gpio_pin_read(uint32_t pin_number);

/**
 * @brief Function for reading the input level of a GPIO pin of a given port.
 *
 * If the value returned by this function is to be valid, the pin's input buffer must be connected.
 *
 * @param p_reg      Pointer to the structure of registers of the peripheral.
 * @param pin_number Specifies the relative pin number to read.
 *
 * @return False if the pin input level is low. True if the pin is high.
 */
NRF_STATIC_INLINE bool nrf_gpio_port_pin_read(NRF_GPIO_Type const * p_reg, uint32_t pin_number);

/**
 * @brief Function for reading the output level of a GPIO pin.
 *
 * @param pin_number Specifies the pin number to read.
 *
 * @return 0 if the pin output level is low. Positive value if pin output is high.
 */
NRF_STATIC_INLINE uint32_t nrf_gpio_pin_out_read(uint32_t pin_number);

/**
 * @brief Function for reading the sense configuration of a GPIO pin.
 *
 * @param pin_number Specifies the pin number to read.
 *
 * @return Sense configuration.
 */
NRF_STATIC_INLINE nrf_gpio_pin_sense_t nrf_gpio_pin_sense_get(uint32_t pin_number);

/**
 * @brief Function for reading the direction configuration of a GPIO pin.
 *
 * @param pin_number Specifies the pin number to read.
 *
 * @return Direction configuration.
 */
NRF_STATIC_INLINE nrf_gpio_pin_dir_t nrf_gpio_pin_dir_get(uint32_t pin_number);

/**
 * @brief Function for reading the status of GPIO pin input buffer.
 *
 * @param pin_number Pin number to be read.
 *
 * @retval Input buffer configuration.
 */
NRF_STATIC_INLINE nrf_gpio_pin_input_t nrf_gpio_pin_input_get(uint32_t pin_number);

/**
 * @brief Function for reading the pull configuration of a GPIO pin.
 *
 * @param pin_number Specifies the pin number to read.
 *
 * @retval Pull configuration.
 */
NRF_STATIC_INLINE nrf_gpio_pin_pull_t nrf_gpio_pin_pull_get(uint32_t pin_number);

/**
 * @brief Function for reading the drive configuration of a GPIO pin.
 *
 * @param pin_number Specifies the pin number to read.
 *
 * @retval Drive configuration.
 */
NRF_STATIC_INLINE nrf_gpio_pin_drive_t nrf_gpio_pin_drive_get(uint32_t pin_number);

/**
 * @brief Function for setting output direction on the selected pins on the given port.
 *
 * @param p_reg    Pointer to the structure of registers of the peripheral.
 * @param out_mask Mask specifying the pins to set as output.
 */
NRF_STATIC_INLINE void nrf_gpio_port_dir_output_set(NRF_GPIO_Type * p_reg, uint32_t out_mask);

/**
 * @brief Function for setting input direction on selected pins on a given port.
 *
 * @param p_reg   Pointer to the structure of registers of the peripheral.
 * @param in_mask Mask that specifies the pins to be set as input.
 */
NRF_STATIC_INLINE void nrf_gpio_port_dir_input_set(NRF_GPIO_Type * p_reg, uint32_t in_mask);

/**
 * @brief Function for writing the direction configuration of the GPIO pins in the given port.
 *
 * @warning This register is retained when retention is enabled.
 *
 * @param p_reg    Pointer to the structure of registers of the peripheral.
 * @param dir_mask Mask that specifies the direction of pins. Bit set means that the given pin is configured as output.
 */
NRF_STATIC_INLINE void nrf_gpio_port_dir_write(NRF_GPIO_Type * p_reg, uint32_t dir_mask);

/**
 * @brief Function for reading the direction configuration of a GPIO port.
 *
 * @warning This register is retained when retention is enabled.
 *
 * @param p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pin configuration of the current direction settings. Bit set means that the given pin is configured as output.
 */
NRF_STATIC_INLINE uint32_t nrf_gpio_port_dir_read(NRF_GPIO_Type const * p_reg);

/**
 * @brief Function for reading the input signals of the GPIO pins on the given port.
 *
 * @param p_reg Pointer to the peripheral registers structure.
 *
 * @return Port input values.
 */
NRF_STATIC_INLINE uint32_t nrf_gpio_port_in_read(NRF_GPIO_Type const * p_reg);

/**
 * @brief Function for reading the output signals of the GPIO pins on the given port.
 *
 * @warning This register is retained when retention is enabled.
 *
 * @param p_reg Pointer to the peripheral registers structure.
 *
 * @return Port output values.
 */
NRF_STATIC_INLINE uint32_t nrf_gpio_port_out_read(NRF_GPIO_Type const * p_reg);

/**
 * @brief Function for writing the GPIO pins output on a given port.
 *
 * @warning This register is retained when retention is enabled.
 *
 * @param p_reg Pointer to the structure of registers of the peripheral.
 * @param value Output port mask.
 */
NRF_STATIC_INLINE void nrf_gpio_port_out_write(NRF_GPIO_Type * p_reg, uint32_t value);

/**
 * @brief Function for setting high level on selected the GPIO pins on the given port.
 *
 * @param p_reg    Pointer to the structure of registers of the peripheral.
 * @param set_mask Mask with pins to be set as logical high level.
 */
NRF_STATIC_INLINE void nrf_gpio_port_out_set(NRF_GPIO_Type * p_reg, uint32_t set_mask);

/**
 * @brief Function for setting low level on selected the GPIO pins on the given port.
 *
 * @param p_reg    Pointer to the structure of registers of the peripheral.
 * @param clr_mask Mask with pins to be set as logical low level.
 */
NRF_STATIC_INLINE void nrf_gpio_port_out_clear(NRF_GPIO_Type * p_reg, uint32_t clr_mask);

/**
 * @brief Function for reading pin state of multiple consecutive ports.
 *
 * @param start_port Index of the first port to read.
 * @param length     Number of ports to read.
 * @param p_masks    Pointer to output array where port states will be stored.
 */
NRF_STATIC_INLINE void nrf_gpio_ports_read(uint32_t   start_port,
                                           uint32_t   length,
                                           uint32_t * p_masks);

#if NRF_GPIO_HAS_PORT_IMPEDANCE
/**
 * @brief Function for setting the impedance matching of the pins on the given port.
 *
 * @note Each bit sets certain impedance and have them in parallel when more than one bit is set.
 *       High impedance is set for the pin when all bits are disabled.
 *       When all bits are enabled, the resulting impedance is about 25 Ohm.
 *
 * @warning This register is retained when retention is enabled.
 *
 * @param p_reg Pointer to the structure of registers of the peripheral.
 * @param mask  Mask of impedances to be set, created using @ref nrf_gpio_port_impedance_mask_t.
 */
NRF_STATIC_INLINE void nrf_gpio_port_impedance_set(NRF_GPIO_Type * p_reg, uint32_t mask);

/**
 * @brief Function for geting the impedance matching of the pins on the given port.
 *
 * @warning This register is retained when retention is enabled.
 *
 * @param p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Mask of impedances set, created using @ref nrf_gpio_port_impedance_mask_t.
 */
NRF_STATIC_INLINE uint32_t nrf_gpio_port_impedance_get(NRF_GPIO_Type const * p_reg);
#endif

#if NRF_GPIO_HAS_RETENTION
/**
 * @brief Function for setting the retention of the registers.
 *
 * @param p_reg Pointer to the structure of registers of the peripheral.
 * @param mask  Mask of retention domains to be enabled, created using @ref nrf_gpio_retain_mask_t.
 */
NRF_STATIC_INLINE void nrf_gpio_port_retain_set(NRF_GPIO_Type * p_reg, uint32_t mask);

/**
 * @brief Function for geting the retention setting of the registers.
 *
 * @param p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Mask of retention domains set, created using @ref nrf_gpio_retain_mask_t.
 */
NRF_STATIC_INLINE uint32_t nrf_gpio_port_retain_get(NRF_GPIO_Type const * p_reg);

/**
 * @brief Function for checking the retention setting of a pin.
 *
 * @param pin_number Pin number.
 *
 * @retval true  If pin is retained.
 * @retval false If pin is not retained.
 */
NRF_STATIC_INLINE bool nrf_gpio_pin_retain_check(uint32_t pin_number);
#endif

#if NRF_GPIO_HAS_RETENTION_SETCLEAR
/**
 * @brief Function for enabling the retention of the registers.
 *
 * @param p_reg Pointer to the structure of registers of the peripheral.
 * @param mask  Mask of pins to have retention enabled, created using @ref nrf_gpio_retain_mask_t.
 */
NRF_STATIC_INLINE void nrf_gpio_port_retain_enable(NRF_GPIO_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the retention of the registers.
 *
 * @param p_reg Pointer to the structure of registers of the peripheral.
 * @param mask  Mask of pins to have retention be disabled, created using @ref nrf_gpio_retain_mask_t.
 */
NRF_STATIC_INLINE void nrf_gpio_port_retain_disable(NRF_GPIO_Type * p_reg, uint32_t mask);

/**
 * @brief Function for enabling the retention of a pin.
 *
 * @param pin_number Pin number.
 */
NRF_STATIC_INLINE void nrf_gpio_pin_retain_enable(uint32_t pin_number);

/**
 * @brief Function for disabling the retention of a pin.
 *
 * @param pin_number Pin number.
 */
NRF_STATIC_INLINE void nrf_gpio_pin_retain_disable(uint32_t pin_number);
#endif


#if NRF_GPIO_HAS_DETECT_MODE
/**
 * @brief Function for setting the latched detect behaviour.
 *
 * @param p_reg  Pointer to the structure of registers of the peripheral.
 * @param enable True if the latched LDETECT behaviour is to be used, false if DETECT is to be
 *               directly connected to PIN DETECT signals.
 */
NRF_STATIC_INLINE void nrf_gpio_port_detect_latch_set(NRF_GPIO_Type * p_reg, bool enable);

/**
 * @brief Function for checking the latched detect behaviour.
 *
 * @param p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Latched LDETECT behaviour is used.
 * @retval false DETECT is directly connected to PIN DETECT signals.
 */
NRF_STATIC_INLINE bool nrf_gpio_port_detect_latch_check(NRF_GPIO_Type const * p_reg);
#endif

#if defined(NRF_GPIO_LATCH_PRESENT)
/**
 * @brief Function for reading latch state of multiple consecutive ports.
 *
 * @warning This register is retained when retention is enabled.
 *
 * @param start_port Index of the first port to read.
 * @param length     Number of ports to read.
 * @param p_masks    Pointer to output array where latch states will be stored.
 */
NRF_STATIC_INLINE void nrf_gpio_latches_read(uint32_t   start_port,
                                             uint32_t   length,
                                             uint32_t * p_masks);

/**
 * @brief Function for reading and immediate clearing latch state of multiple consecutive ports.
 *
 * @warning This register is retained when retention is enabled.
 *
 * @param start_port Index of the first port to read and clear.
 * @param length     Number of ports to read and clear.
 * @param p_masks    Pointer to output array where latch states will be stored.
 */
NRF_STATIC_INLINE void nrf_gpio_latches_read_and_clear(uint32_t   start_port,
                                                       uint32_t   length,
                                                       uint32_t * p_masks);

/**
 * @brief Function for reading latch state of single pin.
 *
 * @warning This register is retained when retention is enabled.
 *
 * @param pin_number Pin number.
 *
 * @return 0 if latch is not set. Positive value otherwise.
 */
NRF_STATIC_INLINE uint32_t nrf_gpio_pin_latch_get(uint32_t pin_number);

/**
 * @brief Function for clearing latch state of a single pin.
 *
 * @warning This register is retained when retention is enabled.
 *
 * @param pin_number Pin number.
 */
NRF_STATIC_INLINE void nrf_gpio_pin_latch_clear(uint32_t pin_number);
#endif // defined(NRF_GPIO_LATCH_PRESENT)

#if NRF_GPIO_HAS_SEL
/**
 * @brief Function for selecting the MCU or Subsystem to control a GPIO pin.
 *
 * @warning This register is retained when retention is enabled.
 *
 * @param pin_number Pin_number.
 * @param ctrl       MCU/Subsystem to control the pin.
 */
NRF_STATIC_INLINE void nrf_gpio_pin_control_select(uint32_t pin_number, nrf_gpio_pin_sel_t ctrl);
#endif

#if NRF_GPIO_HAS_CLOCKPIN
/**
 * @brief Function for setting whether the clock should be enabled for the specified GPIO pin.
 *
 * @warning This register is retained when retention is enabled.
 *
 * @param[in] pin_number Pin number.
 * @param[in] enable     True if clock is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_gpio_pin_clock_set(uint32_t pin_number, bool enable);

/**
 * @brief Function for getting the clock enable setting for the specified GPIO pin.
 *
 * @warning This register is retained when retention is enabled.
 *
 * @param[in] pin_number Pin number.
 *
 * @retval true  Clock is enabled.
 * @retval false Clock is disabled.
 */
NRF_STATIC_INLINE bool nrf_gpio_pin_clock_check(uint32_t pin_number);
#endif

/**
 * @brief Function for checking if provided pin is present on the MCU.
 *
 * @param[in] pin_number Number of the pin to be checked.
 *
 * @retval true  Pin is present.
 * @retval false Pin is not present.
 */
NRF_STATIC_INLINE bool nrf_gpio_pin_present_check(uint32_t pin_number);

/**
 * @brief Function for extracting port number and the relative pin number
 *        from the absolute pin number.
 *
 * @param[in,out] p_pin Pointer to the absolute pin number overridden by the pin number
 *                      that is relative to the port.
 *
 * @return Port number.
*/
NRF_STATIC_INLINE uint32_t nrf_gpio_pin_port_number_extract(uint32_t * p_pin);

/**
 * @brief Function for extracting port and the relative pin number from the absolute pin number.
 *
 * @param[in,out] p_pin Pointer to the absolute pin number overridden by the pin number
 *                      that is relative to the port.
 *
 * @return Pointer to port register set.
 */
NRF_STATIC_INLINE NRF_GPIO_Type * nrf_gpio_pin_port_decode(uint32_t * p_pin);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE NRF_GPIO_Type * nrf_gpio_pin_port_decode(uint32_t * p_pin)
{
    NRFX_ASSERT(nrf_gpio_pin_present_check(*p_pin));

    NRF_GPIO_Type * p_port = NULL;

    switch (nrf_gpio_pin_port_number_extract(p_pin))
    {
        NRF_INTERNAL_GPIO_PORT_EXTRACT(p_port);

        default:
            NRFX_ASSERT(0);
    }
    return p_port;
}


NRF_STATIC_INLINE void nrf_gpio_range_cfg_output(uint32_t pin_range_start, uint32_t pin_range_end)
{
    for (; pin_range_start <= pin_range_end; pin_range_start++)
    {
        nrf_gpio_cfg_output(pin_range_start);
    }
}


NRF_STATIC_INLINE void nrf_gpio_range_cfg_input(uint32_t            pin_range_start,
                                                uint32_t            pin_range_end,
                                                nrf_gpio_pin_pull_t pull_config)
{
    for (; pin_range_start <= pin_range_end; pin_range_start++)
    {
        nrf_gpio_cfg_input(pin_range_start, pull_config);
    }
}


NRF_STATIC_INLINE void nrf_gpio_cfg(
    uint32_t             pin_number,
    nrf_gpio_pin_dir_t   dir,
    nrf_gpio_pin_input_t input,
    nrf_gpio_pin_pull_t  pull,
    nrf_gpio_pin_drive_t drive,
    nrf_gpio_pin_sense_t sense)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);
    uint32_t cnf = reg->PIN_CNF[pin_number];

    uint32_t to_update = GPIO_PIN_CNF_DIR_Msk    |
                         GPIO_PIN_CNF_INPUT_Msk  |
                         GPIO_PIN_CNF_PULL_Msk   |
#if defined(GPIO_PIN_CNF_DRIVE_Msk)
                         GPIO_PIN_CNF_DRIVE_Msk  |
#else
                         GPIO_PIN_CNF_DRIVE0_Msk |
                         GPIO_PIN_CNF_DRIVE1_Msk |
#endif
                         GPIO_PIN_CNF_SENSE_Msk;

    /* Clear fields that will be updated. */
    cnf &= ~to_update;
    cnf |= ((uint32_t)dir << GPIO_PIN_CNF_DIR_Pos)      |
           ((uint32_t)input << GPIO_PIN_CNF_INPUT_Pos)  |
           ((uint32_t)pull << GPIO_PIN_CNF_PULL_Pos)    |
#if defined(GPIO_PIN_CNF_DRIVE_Pos)
           ((uint32_t)drive << GPIO_PIN_CNF_DRIVE_Pos)  |
#else
           ((uint32_t)drive << GPIO_PIN_CNF_DRIVE0_Pos) |
#endif
           ((uint32_t)sense << GPIO_PIN_CNF_SENSE_Pos);

    reg->PIN_CNF[pin_number] = cnf;
}

NRF_STATIC_INLINE void nrf_gpio_reconfigure(uint32_t                     pin_number,
                                            const nrf_gpio_pin_dir_t *   p_dir,
                                            const nrf_gpio_pin_input_t * p_input,
                                            const nrf_gpio_pin_pull_t *  p_pull,
                                            const nrf_gpio_pin_drive_t * p_drive,
                                            const nrf_gpio_pin_sense_t * p_sense)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);
    uint32_t cnf = reg->PIN_CNF[pin_number];
    uint32_t to_update = (p_dir   ? GPIO_PIN_CNF_DIR_Msk                                : 0) |
                         (p_input ? GPIO_PIN_CNF_INPUT_Msk                              : 0) |
                         (p_pull  ? GPIO_PIN_CNF_PULL_Msk                               : 0) |
#if defined(GPIO_PIN_CNF_DRIVE_Msk)
                         (p_drive ? GPIO_PIN_CNF_DRIVE_Msk                              : 0) |
#else
                         (p_drive ? (GPIO_PIN_CNF_DRIVE0_Msk | GPIO_PIN_CNF_DRIVE1_Msk) : 0) |
#endif
                         (p_sense ? GPIO_PIN_CNF_SENSE_Msk                              : 0);

    /* Clear fields that will be updated. */
    cnf &= ~to_update;
    cnf |= ((uint32_t)(p_dir   ? *p_dir   : 0) << GPIO_PIN_CNF_DIR_Pos)    |
           ((uint32_t)(p_input ? *p_input : 0) << GPIO_PIN_CNF_INPUT_Pos)  |
           ((uint32_t)(p_pull  ? *p_pull  : 0) << GPIO_PIN_CNF_PULL_Pos)   |
#if defined(GPIO_PIN_CNF_DRIVE_Pos)
           ((uint32_t)(p_drive ? *p_drive : 0) << GPIO_PIN_CNF_DRIVE_Pos)  |
#else
           ((uint32_t)(p_drive ? *p_drive : 0) << GPIO_PIN_CNF_DRIVE0_Pos) |
#endif
           ((uint32_t)(p_sense ? *p_sense : 0)<< GPIO_PIN_CNF_SENSE_Pos);

    reg->PIN_CNF[pin_number] = cnf;
}

NRF_STATIC_INLINE void nrf_gpio_cfg_output(uint32_t pin_number)
{
    nrf_gpio_cfg(
        pin_number,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_S0S1,
        NRF_GPIO_PIN_NOSENSE);
}

NRF_STATIC_INLINE void nrf_gpio_port_pin_output_set(NRF_GPIO_Type * p_reg, uint32_t pin_number)
{
    uint32_t cnf = ((uint32_t)NRF_GPIO_PIN_DIR_OUTPUT << GPIO_PIN_CNF_DIR_Pos) |
           ((uint32_t)NRF_GPIO_PIN_INPUT_DISCONNECT << GPIO_PIN_CNF_INPUT_Pos) |
           ((uint32_t)NRF_GPIO_PIN_NOPULL << GPIO_PIN_CNF_PULL_Pos)            |
#if defined(GPIO_PIN_CNF_DRIVE_Pos)
           ((uint32_t)NRF_GPIO_PIN_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)             |
#else
           ((uint32_t)NRF_GPIO_PIN_S0S1 << GPIO_PIN_CNF_DRIVE0_Pos)            |
#endif
           ((uint32_t)NRF_GPIO_PIN_NOSENSE << GPIO_PIN_CNF_SENSE_Pos);
    p_reg->PIN_CNF[pin_number] = cnf;
}

NRF_STATIC_INLINE void nrf_gpio_cfg_input(uint32_t pin_number, nrf_gpio_pin_pull_t pull_config)
{
    nrf_gpio_cfg(
        pin_number,
        NRF_GPIO_PIN_DIR_INPUT,
        NRF_GPIO_PIN_INPUT_CONNECT,
        pull_config,
        NRF_GPIO_PIN_S0S1,
        NRF_GPIO_PIN_NOSENSE);
}

NRF_STATIC_INLINE void nrf_gpio_port_pin_input_set(NRF_GPIO_Type *     p_reg,
                                                   uint32_t            pin_number,
                                                   nrf_gpio_pin_pull_t pull_config)
{
    uint32_t cnf = ((uint32_t)NRF_GPIO_PIN_DIR_INPUT << GPIO_PIN_CNF_DIR_Pos) |
           ((uint32_t)NRF_GPIO_PIN_INPUT_CONNECT << GPIO_PIN_CNF_INPUT_Pos)   |
           ((uint32_t)pull_config << GPIO_PIN_CNF_PULL_Pos)                   |
#if defined(GPIO_PIN_CNF_DRIVE_Pos)
           ((uint32_t)NRF_GPIO_PIN_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)            |
#else
           ((uint32_t)NRF_GPIO_PIN_S0S1 << GPIO_PIN_CNF_DRIVE0_Pos)           |
#endif
           ((uint32_t)NRF_GPIO_PIN_NOSENSE << GPIO_PIN_CNF_SENSE_Pos);
    p_reg->PIN_CNF[pin_number] = cnf;
}


NRF_STATIC_INLINE void nrf_gpio_cfg_default(uint32_t pin_number)
{
    nrf_gpio_cfg(
        pin_number,
        NRF_GPIO_PIN_DIR_INPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_S0S1,
        NRF_GPIO_PIN_NOSENSE);
}


NRF_STATIC_INLINE void nrf_gpio_cfg_watcher(uint32_t pin_number)
{
    nrf_gpio_pin_input_t input = NRF_GPIO_PIN_INPUT_CONNECT;

    nrf_gpio_reconfigure(pin_number, NULL, &input, NULL, NULL, NULL);
}


NRF_STATIC_INLINE void nrf_gpio_input_disconnect(uint32_t pin_number)
{
    nrf_gpio_pin_input_t input = NRF_GPIO_PIN_INPUT_DISCONNECT;

    nrf_gpio_reconfigure(pin_number, NULL, &input, NULL, NULL, NULL);
}


NRF_STATIC_INLINE void nrf_gpio_cfg_sense_input(uint32_t             pin_number,
                                                nrf_gpio_pin_pull_t  pull_config,
                                                nrf_gpio_pin_sense_t sense_config)
{
    nrf_gpio_cfg(
        pin_number,
        NRF_GPIO_PIN_DIR_INPUT,
        NRF_GPIO_PIN_INPUT_CONNECT,
        pull_config,
        NRF_GPIO_PIN_S0S1,
        sense_config);
}


NRF_STATIC_INLINE void nrf_gpio_cfg_sense_set(uint32_t             pin_number,
                                              nrf_gpio_pin_sense_t sense_config)
{
    nrf_gpio_reconfigure(pin_number, NULL, NULL, NULL, NULL, &sense_config);
}

NRF_STATIC_INLINE void nrf_gpio_pin_dir_set(uint32_t pin_number, nrf_gpio_pin_dir_t direction)
{
    if (direction == NRF_GPIO_PIN_DIR_INPUT)
    {
        nrf_gpio_cfg(
            pin_number,
            NRF_GPIO_PIN_DIR_INPUT,
            NRF_GPIO_PIN_INPUT_CONNECT,
            NRF_GPIO_PIN_NOPULL,
            NRF_GPIO_PIN_S0S1,
            NRF_GPIO_PIN_NOSENSE);
    }
    else
    {
        NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);
        reg->DIRSET = (1UL << pin_number);
    }
}


NRF_STATIC_INLINE void nrf_gpio_pin_set(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    nrf_gpio_port_out_set(reg, 1UL << pin_number);
}


NRF_STATIC_INLINE void nrf_gpio_pin_clear(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    nrf_gpio_port_out_clear(reg, 1UL << pin_number);
}


NRF_STATIC_INLINE void nrf_gpio_pin_toggle(uint32_t pin_number)
{
    NRF_GPIO_Type * reg        = nrf_gpio_pin_port_decode(&pin_number);
    uint32_t        pins_state = reg->OUT;

    reg->OUTSET = (~pins_state & (1UL << pin_number));
    reg->OUTCLR = (pins_state & (1UL << pin_number));
}


NRF_STATIC_INLINE void nrf_gpio_pin_write(uint32_t pin_number, uint32_t value)
{
    if (value == 0)
    {
        nrf_gpio_pin_clear(pin_number);
    }
    else
    {
        nrf_gpio_pin_set(pin_number);
    }
}

NRF_STATIC_INLINE void nrf_gpio_port_pin_write(NRF_GPIO_Type * p_reg,
                                               uint32_t        pin_number,
                                               uint32_t        value)
{
    if (value == 0)
    {
        nrf_gpio_port_out_clear(p_reg, 1UL << pin_number);
    }
    else
    {
        nrf_gpio_port_out_set(p_reg, 1UL << pin_number);
    }
}


NRF_STATIC_INLINE uint32_t nrf_gpio_pin_read(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    return ((nrf_gpio_port_in_read(reg) >> pin_number) & 1UL);
}

NRF_STATIC_INLINE bool nrf_gpio_port_pin_read(NRF_GPIO_Type const * p_reg, uint32_t pin_number)
{
    return ((nrf_gpio_port_in_read(p_reg) >> pin_number) & 1UL);
}

NRF_STATIC_INLINE uint32_t nrf_gpio_pin_out_read(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    return ((nrf_gpio_port_out_read(reg) >> pin_number) & 1UL);
}


NRF_STATIC_INLINE nrf_gpio_pin_sense_t nrf_gpio_pin_sense_get(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    return (nrf_gpio_pin_sense_t)((reg->PIN_CNF[pin_number] &
                                   GPIO_PIN_CNF_SENSE_Msk) >> GPIO_PIN_CNF_SENSE_Pos);
}


NRF_STATIC_INLINE nrf_gpio_pin_dir_t nrf_gpio_pin_dir_get(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    return (nrf_gpio_pin_dir_t)((reg->PIN_CNF[pin_number] &
                                 GPIO_PIN_CNF_DIR_Msk) >> GPIO_PIN_CNF_DIR_Pos);
}

NRF_STATIC_INLINE nrf_gpio_pin_input_t nrf_gpio_pin_input_get(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    return (nrf_gpio_pin_input_t)((reg->PIN_CNF[pin_number] &
                                   GPIO_PIN_CNF_INPUT_Msk) >> GPIO_PIN_CNF_INPUT_Pos);
}

NRF_STATIC_INLINE nrf_gpio_pin_pull_t nrf_gpio_pin_pull_get(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    return (nrf_gpio_pin_pull_t)((reg->PIN_CNF[pin_number] &
                                  GPIO_PIN_CNF_PULL_Msk) >> GPIO_PIN_CNF_PULL_Pos);
}

NRF_STATIC_INLINE nrf_gpio_pin_drive_t nrf_gpio_pin_drive_get(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

#if defined(GPIO_PIN_CNF_DRIVE_Pos)
    return (nrf_gpio_pin_drive_t)((reg->PIN_CNF[pin_number] &
                                  GPIO_PIN_CNF_DRIVE_Msk) >> GPIO_PIN_CNF_DRIVE_Pos);
#else
    return (nrf_gpio_pin_drive_t)((reg->PIN_CNF[pin_number] &
                                  (GPIO_PIN_CNF_DRIVE0_Msk | GPIO_PIN_CNF_DRIVE1_Msk))
                                  >> GPIO_PIN_CNF_DRIVE0_Pos);
#endif
}

NRF_STATIC_INLINE void nrf_gpio_port_dir_output_set(NRF_GPIO_Type * p_reg, uint32_t out_mask)
{
    p_reg->DIRSET = out_mask;
}


NRF_STATIC_INLINE void nrf_gpio_port_dir_input_set(NRF_GPIO_Type * p_reg, uint32_t in_mask)
{
    p_reg->DIRCLR = in_mask;
}


NRF_STATIC_INLINE void nrf_gpio_port_dir_write(NRF_GPIO_Type * p_reg, uint32_t value)
{
    p_reg->DIR = value;
}


NRF_STATIC_INLINE uint32_t nrf_gpio_port_dir_read(NRF_GPIO_Type const * p_reg)
{
    return p_reg->DIR;
}


NRF_STATIC_INLINE uint32_t nrf_gpio_port_in_read(NRF_GPIO_Type const * p_reg)
{
    return p_reg->IN;
}


NRF_STATIC_INLINE uint32_t nrf_gpio_port_out_read(NRF_GPIO_Type const * p_reg)
{
    return p_reg->OUT;
}


NRF_STATIC_INLINE void nrf_gpio_port_out_write(NRF_GPIO_Type * p_reg, uint32_t value)
{
    p_reg->OUT = value;
}


NRF_STATIC_INLINE void nrf_gpio_port_out_set(NRF_GPIO_Type * p_reg, uint32_t set_mask)
{
    p_reg->OUTSET = set_mask;
}


NRF_STATIC_INLINE void nrf_gpio_port_out_clear(NRF_GPIO_Type * p_reg, uint32_t clr_mask)
{
    p_reg->OUTCLR = clr_mask;
}


NRF_STATIC_INLINE void nrf_gpio_ports_read(uint32_t   start_port,
                                           uint32_t   length,
                                           uint32_t * p_masks)
{
    NRF_GPIO_Type * gpio_regs[GPIO_COUNT] = GPIO_REG_LIST;

    NRFX_ASSERT(start_port + length <= GPIO_COUNT);
    uint32_t i;

    for (i = start_port; i < (start_port + length); i++)
    {
        *p_masks = nrf_gpio_port_in_read(gpio_regs[i]);
        p_masks++;
    }
}

#if NRF_GPIO_HAS_PORT_IMPEDANCE
NRF_STATIC_INLINE void nrf_gpio_port_impedance_set(NRF_GPIO_Type * p_reg, uint32_t mask)
{
    p_reg->PORTCNF.DRIVECTRL = ((p_reg->PORTCNF.DRIVECTRL & ~NRF_GPIO_PORT_IMPEDANCE_ALL_MASK) |
                                (mask & NRF_GPIO_PORT_IMPEDANCE_ALL_MASK));
}

NRF_STATIC_INLINE uint32_t nrf_gpio_port_impedance_get(NRF_GPIO_Type const * p_reg)
{
    return p_reg->PORTCNF.DRIVECTRL & NRF_GPIO_PORT_IMPEDANCE_ALL_MASK;
}
#endif

#if NRF_GPIO_HAS_RETENTION
NRF_STATIC_INLINE void nrf_gpio_port_retain_set(NRF_GPIO_Type * p_reg, uint32_t mask)
{
    p_reg->RETAIN = mask;
}

NRF_STATIC_INLINE uint32_t nrf_gpio_port_retain_get(NRF_GPIO_Type const * p_reg)
{
    return p_reg->RETAIN;
}

NRF_STATIC_INLINE bool nrf_gpio_pin_retain_check(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    return (nrf_gpio_port_retain_get(reg) & (1UL << pin_number)) != 0U;
}
#endif

#if NRF_GPIO_HAS_RETENTION_SETCLEAR
NRF_STATIC_INLINE void nrf_gpio_port_retain_enable(NRF_GPIO_Type * p_reg, uint32_t mask)
{
    p_reg->RETAINSET = mask;
}

NRF_STATIC_INLINE void nrf_gpio_port_retain_disable(NRF_GPIO_Type * p_reg, uint32_t mask)
{
    p_reg->RETAINCLR = mask;
}

NRF_STATIC_INLINE void nrf_gpio_pin_retain_enable(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    nrf_gpio_port_retain_enable(reg, 1UL << pin_number);
}

NRF_STATIC_INLINE void nrf_gpio_pin_retain_disable(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    nrf_gpio_port_retain_disable(reg, 1UL << pin_number);
}
#endif

#if NRF_GPIO_HAS_DETECT_MODE
NRF_STATIC_INLINE void nrf_gpio_port_detect_latch_set(NRF_GPIO_Type * p_reg, bool enable)
{
    p_reg->DETECTMODE = (enable ? GPIO_DETECTMODE_DETECTMODE_LDETECT :
                                  GPIO_DETECTMODE_DETECTMODE_Default);
}

NRF_STATIC_INLINE bool nrf_gpio_port_detect_latch_check(NRF_GPIO_Type const * p_reg)
{
    return (p_reg->DETECTMODE == GPIO_DETECTMODE_DETECTMODE_LDETECT);
}
#endif

#if defined(NRF_GPIO_LATCH_PRESENT)
NRF_STATIC_INLINE void nrf_gpio_latches_read(uint32_t   start_port,
                                             uint32_t   length,
                                             uint32_t * p_masks)
{
    NRF_GPIO_Type * gpio_regs[GPIO_COUNT] = GPIO_REG_LIST;
    uint32_t        i;

    for (i = start_port; i < (start_port + length); i++)
    {
        *p_masks = gpio_regs[i]->LATCH;
        p_masks++;
    }
}

NRF_STATIC_INLINE void nrf_gpio_latches_read_and_clear(uint32_t   start_port,
                                                       uint32_t   length,
                                                       uint32_t * p_masks)
{
    NRF_GPIO_Type * gpio_regs[GPIO_COUNT] = GPIO_REG_LIST;
    uint32_t        i;

    for (i = start_port; i < (start_port + length); i++)
    {
        *p_masks = gpio_regs[i]->LATCH;

        // The LATCH register is cleared by writing a '1' to the bit that shall be cleared.
        gpio_regs[i]->LATCH = *p_masks;

        p_masks++;
    }

    if (NRF_ERRATA_DYNAMIC_CHECK(52, 173) || NRF_ERRATA_DYNAMIC_CHECK(53, 119))
    {
        for (i = start_port; i < (start_port + length); i++)
        {
            (void)gpio_regs[i]->LATCH;
        }
    }
}

NRF_STATIC_INLINE uint32_t nrf_gpio_pin_latch_get(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    return (reg->LATCH & (1 << pin_number)) ? 1 : 0;
}


NRF_STATIC_INLINE void nrf_gpio_pin_latch_clear(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    reg->LATCH = (1 << pin_number);

    if (NRF_ERRATA_DYNAMIC_CHECK(52, 173) || NRF_ERRATA_DYNAMIC_CHECK(53, 119))
    {
        for (uint8_t i = 0; i < 4; i++)
        {
            (void)reg->LATCH;
        }
    }
}
#endif // defined(NRF_GPIO_LATCH_PRESENT)

#if NRF_GPIO_HAS_SEL
NRF_STATIC_INLINE void nrf_gpio_pin_control_select(uint32_t pin_number, nrf_gpio_pin_sel_t ctrl)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);
#if defined(GPIO_PIN_CNF_MCUSEL_Msk)
    uint32_t cnf = reg->PIN_CNF[pin_number] & ~GPIO_PIN_CNF_MCUSEL_Msk;
    reg->PIN_CNF[pin_number] = cnf | (ctrl << GPIO_PIN_CNF_MCUSEL_Pos);
#else
    uint32_t cnf = reg->PIN_CNF[pin_number] & ~GPIO_PIN_CNF_CTRLSEL_Msk;
    reg->PIN_CNF[pin_number] = cnf | (ctrl << GPIO_PIN_CNF_CTRLSEL_Pos);
#endif
}
#endif // NRF_GPIO_HAS_SEL

#if NRF_GPIO_HAS_CLOCKPIN
NRF_STATIC_INLINE void nrf_gpio_pin_clock_set(uint32_t pin_number, bool enable)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    reg->PIN_CNF[pin_number] = ((reg->PIN_CNF[pin_number] & ~GPIO_PIN_CNF_CLOCKPIN_Msk) |
                                ((enable ? GPIO_PIN_CNF_CLOCKPIN_Enabled :
                                  GPIO_PIN_CNF_CLOCKPIN_Disabled) << GPIO_PIN_CNF_CLOCKPIN_Pos));
}

NRF_STATIC_INLINE bool nrf_gpio_pin_clock_check(uint32_t pin_number)
{
    NRF_GPIO_Type * reg = nrf_gpio_pin_port_decode(&pin_number);

    return (((reg->PIN_CNF[pin_number] & GPIO_PIN_CNF_CLOCKPIN_Msk) >> GPIO_PIN_CNF_CLOCKPIN_Pos)
            == GPIO_PIN_CNF_CLOCKPIN_Enabled);
}
#endif

NRF_STATIC_INLINE bool nrf_gpio_pin_present_check(uint32_t pin_number)
{
    uint32_t port = pin_number >> 5;
    uint32_t mask = 0;

    switch (port)
    {
        NRF_INTERNAL_GPIO_PORT_MASK_SET(mask);

        default:
            return false;
    }

#if defined(P0_FEATURE_PINS_PRESENT) && defined(GPIO_PINS_OVERRIDE)
    mask |= GPIO_PINS_OVERRIDE;
#endif

    pin_number &= 0x1F;

    return (mask & (1UL << pin_number)) ? true : false;
}

NRF_STATIC_INLINE uint32_t nrf_gpio_pin_port_number_extract(uint32_t * p_pin)
{
    uint32_t pin_number = *p_pin;
    *p_pin = NRF_PIN_NUMBER_TO_PIN(pin_number);

    return NRF_PIN_NUMBER_TO_PORT(pin_number);
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_GPIO_H__
