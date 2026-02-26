/*
 * Copyright (c) Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file sdc_hci_cmd_status_params.h
 *
 * @defgroup sdc_hci_sp SoftDevice Controller HCI Interface for Status parameters
 * @ingroup sdc_hci
 *
 * All APIs in this header file are expected to be called from the
 * same execution priority as mpsl_low_priority_process.
 * Not doing so will lead to undefined behavior.
 *
 * @{
 */

#ifndef SDC_HCI_CMD_STATUS_PARAMS_H__
#define SDC_HCI_CMD_STATUS_PARAMS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <nrfx.h>

/**
 * @defgroup HCI_TYPES Types
 * @{
 */

/** @brief HCI SP OpCode Field values. */
enum sdc_hci_opcode_sp
{
    /** @brief See @ref sdc_hci_cmd_sp_read_rssi(). */
    SDC_HCI_OPCODE_CMD_SP_READ_RSSI = 0x1405,
};

/** @} end of HCI_TYPES */

/**
 * @defgroup HCI_COMMAND_PARAMETERS Parameters
 * @{
 */

/** @brief Read RSSI command parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t handle;
} sdc_hci_cmd_sp_read_rssi_t;

/** @brief Read RSSI return parameter(s). */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t handle;
    int8_t rssi;
} sdc_hci_cmd_sp_read_rssi_return_t;

/** @} end of HCI_COMMAND_PARAMETERS */

/**
 * @defgroup HCI_VS_API API
 * @{
 */
/** @brief Read RSSI.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.5.4
 *
 * This command reads the Received Signal Strength Indication (RSSI) value from a
 * Controller.
 *
 * For a BR/EDR Controller, the RSSI parameter returns the difference between the
 * measured Received Signal Strength Indication (RSSI) and the limits of a range selected
 * by the Controller. The lower limit shall correspond to a received power not less than -56
 * dBm and not greater than 6 dB above the actual sensitivity of the receiver. The upper
 * limit shall be 20±6 dB above the lower limit. A positive RSSI value shall indicate how
 * many dB the RSSI is above the upper limit, a negative value shall indicate how many
 * dB the RSSI is below the lower limit, and zero shall indicate that the RSSI is inside the
 * range.
 *
 * The returned RSSI value is not required to have any specific accuracy provided that
 * it correctly indicates whether the received signal strength was above the upper limit,
 * below the lower limit, or between the limits.
 *
 * For an LE transport, the RSSI parameter returns the absolute received signal strength
 * value in dBm to ±6 dB accuracy. If the RSSI cannot be read, the RSSI parameter shall
 * be set to 127.
 *
 * Event(s) generated (unless masked away):
 *
 * When the HCI_Read_RSSI command has completed, an HCI_Command_Complete
 * event shall be generated.
 *
 * @param[in]  p_params Input parameters.
 * @param[out] p_return Extra return parameters.
 *
 * @retval 0 if success.
 * @return Returns value between 0x01-0xFF in case of error.
 *         See Vol 2, Part D, Error for a list of error codes and descriptions.
 */
uint8_t sdc_hci_cmd_sp_read_rssi(const sdc_hci_cmd_sp_read_rssi_t * p_params,
                                 sdc_hci_cmd_sp_read_rssi_return_t * p_return);

/** @} end of HCI_VS_API */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SDC_HCI_CMD_STATUS_PARAMS_H__ */
