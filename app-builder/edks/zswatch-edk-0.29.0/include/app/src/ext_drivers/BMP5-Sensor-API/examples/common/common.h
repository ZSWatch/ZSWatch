/**\
 * Copyright (c) 2022 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef _BMP5_COMMON_H
#define _BMP5_COMMON_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "bmp5.h"

/***************************************************************************/

/*!                 User function prototypes
 ****************************************************************************/

/*!
 *  @brief Function for reading the sensor's registers through SPI bus.
 *
 *  @param[in] reg_addr      : Register address.
 *  @param[out] reg_data     : Pointer to the data buffer to store the read data.
 *  @param[in] length        : No of bytes to read.
 *  @param[in, out] intf_ptr : Void pointer that can enable the linking of descriptors
 *                             for interface related call backs.
 *
 *  @return Status of execution
 *
 *  @retval BMP5_INTF_RET_SUCCESS -> Success.
 *  @retval != BMP5_INTF_RET_SUCCESS -> Fail.
 *
 */
BMP5_INTF_RET_TYPE bmp5_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);

/*!
 *  @brief Function for reading the sensor's registers through I2C bus.
 *
 *  @param[in] reg_addr      : Register address.
 *  @param[out] reg_data     : Pointer to the data buffer to store the read data.
 *  @param[in] length        : No of bytes to read.
 *  @param[in, out] intf_ptr : Void pointer that can enable the linking of descriptors
 *                             for interface related call backs.
 *
 *  @return Status of execution
 *
 *  @retval BMP5_INTF_RET_SUCCESS -> Success.
 *  @retval != BMP5_INTF_RET_SUCCESS -> Fail.
 *
 */
BMP5_INTF_RET_TYPE bmp5_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);

/*!
 *  @brief Function for writing the sensor's registers through SPI bus.
 *
 *  @param[in] reg_addr      : Register address.
 *  @param[in] reg_data      : Pointer to the data buffer whose data has to be written.
 *  @param[in] length        : No of bytes to write.
 *  @param[in, out] intf_ptr : Void pointer that can enable the linking of descriptors
 *                             for interface related call backs.
 *
 *  @return Status of execution
 *
 *  @retval BMP5_INTF_RET_SUCCESS -> Success.
 *  @retval  != BMP5_INTF_RET_SUCCESS -> Fail.
 *
 */
BMP5_INTF_RET_TYPE bmp5_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);

/*!
 *  @brief Function for writing the sensor's registers through I2C bus.
 *
 *  @param[in] reg_addr      : Register address.
 *  @param[in] reg_data      : Pointer to the data buffer whose value is to be written.
 *  @param[in] length        : No of bytes to write.
 *  @param[in, out] intf_ptr : Void pointer that can enable the linking of descriptors
 *                             for interface related call backs.
 *
 *  @return Status of execution
 *
 *  @retval BMP5_INTF_RET_SUCCESS -> Success.
 *  @retval != BMP5_INTF_RET_SUCCESS -> Failure.
 *
 */
BMP5_INTF_RET_TYPE bmp5_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);

/*!
 * @brief This function provides the delay for required time (Microsecond) as per the input provided in some of the
 * APIs.
 *
 *  @param[in] period_us      : The required wait time in microsecond.
 *  @param[in, out] intf_ptr  : Void pointer that can enable the linking of descriptors
 *                             for interface related call backs.
 *  @return void.
 *
 */
void bmp5_delay_us(uint32_t period_us, void *intf_ptr);

/*!
 *  @brief Function to select the interface between SPI and I2C.
 *         Also to initialize coines platform.
 *
 *  @param[in] bmp5_dev : Structure instance of bmp5_dev
 *  @param[in] intf     : Interface selection parameter
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval < 0 -> Failure Info
 */
int8_t bmp5_interface_init(struct bmp5_dev *bmp5_dev, uint8_t intf);

/*!
 *  @brief Prints the execution status of the APIs.
 *
 *  @param[in] api_name : Name of the API whose execution status has to be printed.
 *  @param[in] rslt     : Error code returned by the API whose execution status has to be printed.
 *
 *  @return void.
 */
void bmp5_error_codes_print_result(const char api_name[], int8_t rslt);

/*!
 * @brief This function deinitializes coines platform
 *
 *  @return void.
 *
 */
void bmp5_coines_deinit(void);

#ifdef __cplusplus
}
#endif /* End of CPP guard */

#endif /* _BMP5_COMMON_H */
