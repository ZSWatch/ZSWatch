/*
 * Copyright (c) 2023 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_RRAMC_H__
#define NRF_RRAMC_H__

#include <nrfx.h>
#include <nrf_bitmask.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_rramc_hal RRAMC HAL
 * @{
 * @ingroup nrf_rramc
 * @brief   Hardware access layer for managing the the Resistive Random Access Memory Controller (RRAMC) peripheral.
 */

#if defined(RRAMC_REGION_CONFIG_SECURE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether RRAM controller has secure-only access configuration. */
#define NRF_RRAMC_HAS_SECURE_CONFIG 1
#else
#define NRF_RRAMC_HAS_SECURE_CONFIG 0
#endif

#if defined(RRAMC_POWER_LOWPOWERCONFIG_MODE_NAP) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether RRAM controller has nap mode. */
#define NRF_RRAMC_HAS_LP_MODE_NAP 1
#else
#define NRF_RRAMC_HAS_LP_MODE_NAP 0
#endif

#if defined(RRAMC_POWER_LOWPOWERCONFIG_MODE_PowerDown) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether RRAM controller has power down mode. */
#define NRF_RRAMC_HAS_LP_MODE_POWER_DOWN 1
#else
#define NRF_RRAMC_HAS_LP_MODE_POWER_DOWN 0
#endif

/** @brief Maximum size of a write-buffer in number of 128-bit words. */
#define NRF_RRAMC_CONFIG_WRITE_BUFF_SIZE_MAX RRAMC_CONFIG_WRITEBUFSIZE_Max

/** @brief Maximum preload timeout value for waiting for a next write. */
#define NRF_RRAMC_READYNEXTTIMEOUT_MAX RRAMC_READYNEXTTIMEOUT_VALUE_Max

/** @brief RRAMC region permissions bitmask. */
#define NRF_RRAMC_REGION_CONFIG_PERM_MASK (RRAMC_REGION_CONFIG_READ_Msk    |                        \
                                           RRAMC_REGION_CONFIG_WRITE_Msk   |                        \
                                           RRAMC_REGION_CONFIG_EXECUTE_Msk |                        \
                                           NRFX_COND_CODE_1(NRF_RRAMC_HAS_SECURE_CONFIG,            \
                                                            (RRAMC_REGION_CONFIG_SECURE_Msk), (0))) \

/** @brief RRAMC tasks. */
typedef enum
{
    NRF_RRAMC_TASK_WAKEUP          = offsetof(NRF_RRAMC_Type, TASKS_WAKEUP),         ///< Wakeup the RRAM from low power mode.
    NRF_RRAMC_TASK_COMMIT_WRITEBUF = offsetof(NRF_RRAMC_Type, TASKS_COMMITWRITEBUF), ///< Commit the data stored in internal write-buffer to RRAM.
} nrf_rramc_task_t;

/** @brief RRAMC events. */
typedef enum
{
    NRF_RRAMC_EVENT_WOKENUP      = offsetof(NRF_RRAMC_Type, EVENTS_WOKENUP),     ///< The RRAM is woken up from low power mode.
    NRF_RRAMC_EVENT_READY        = offsetof(NRF_RRAMC_Type, EVENTS_READY),       ///< RRAMC is ready.
    NRF_RRAMC_EVENT_READY_NEXT   = offsetof(NRF_RRAMC_Type, EVENTS_READYNEXT),   ///< Ready to accept a new write operation.
    NRF_RRAMC_EVENT_ERROR_ACCESS = offsetof(NRF_RRAMC_Type, EVENTS_ACCESSERROR), ///< RRAM access error.
} nrf_rramc_event_t;

/** @brief RRAMC interrupts. */
typedef enum
{
    NRF_RRAMC_INT_WOKENUP_MASK      = RRAMC_INTENSET_WOKENUP_Msk,     ///< Interrupt on WOKENUP event.
    NRF_RRAMC_INT_READY_MASK        = RRAMC_INTENSET_READY_Msk,       ///< Interrupt on READY event.
    NRF_RRAMC_INT_READY_NEXT_MASK   = RRAMC_INTENSET_READYNEXT_Msk,   ///< Interrupt on READYNEXT event.
    NRF_RRAMC_INT_ERROR_ACCESS_MASK = RRAMC_INTENSET_ACCESSERROR_Msk, ///< Interrupt on ACCESSERROR event.
    NRF_RRAMC_ALL_INTS_MASK         = NRF_RRAMC_INT_WOKENUP_MASK
                                    | NRF_RRAMC_INT_READY_MASK
                                    | NRF_RRAMC_INT_READY_NEXT_MASK
                                    | NRF_RRAMC_INT_ERROR_ACCESS_MASK ///< All RRAMC interrupts.
} nrf_rramc_int_mask_t;

/** @brief RRAMC low power modes. */
typedef enum
{
#if NRF_RRAMC_HAS_LP_MODE_POWER_DOWN
    NRF_RRAMC_LP_POWER_DOWN = RRAMC_POWER_LOWPOWERCONFIG_MODE_PowerDown, ///< The RRAM goes into power down mode.
#endif
    NRF_RRAMC_LP_STANDBY    = RRAMC_POWER_LOWPOWERCONFIG_MODE_Standby,   /**< The RRAM automatically goes into standby mode while the RRAM is
                                                                          *   not being accessed. */
#if NRF_RRAMC_HAS_LP_MODE_NAP
    NRF_RRAMC_LP_NAP        = RRAMC_POWER_LOWPOWERCONFIG_MODE_NAP,       ///< The RRAM goes into NAP mode.
#endif
    NRF_RRAMC_LP_POWER_OFF  = RRAMC_POWER_LOWPOWERCONFIG_MODE_PowerOff,  ///< The RRAM is powered off.
} nrf_rramc_lp_mode_t;

/** @brief RRAMC configuration structure. */
typedef struct
{
    bool    mode_write;      ///< True if write mode is to be enabled, false otherwise.
    uint8_t write_buff_size; ///< Write-buffer size in case set to 0 buffering is disabled.
} nrf_rramc_config_t;

/** @brief Preload timeout value for waiting for a next write. */
typedef struct
{
    uint16_t value;  ///< Preload value expressed in clock cycles.
    bool     enable; ///< True if write to the RRAM is to be triggered on the next timeout, false otherwise.
} nrf_rramc_ready_next_timeout_t;

/** @brief Power configuration. */
typedef struct
{
    uint16_t access_timeout; ///< Access timeout used for going into standby power mode or remain active on wake up, expressed in clock cycles.
    bool     abort_on_pof;   ///< True if the current RRAM write operation is to be aborted on the power failure, false otherwise.
} nrf_rramc_power_t;

/**
 * @brief RRAMC region permissions mask.
 *
 * @note When bit is set, the selected action is allowed.
 */
typedef enum
{
    NRF_RRAMC_REGION_PERM_READ_MASK    = RRAMC_REGION_CONFIG_READ_Msk,    ///< Read access.
    NRF_RRAMC_REGION_PERM_WRITE_MASK   = RRAMC_REGION_CONFIG_WRITE_Msk,   ///< Write access.
    NRF_RRAMC_REGION_PERM_EXECUTE_MASK = RRAMC_REGION_CONFIG_EXECUTE_Msk, ///< Software execute.
#if NRF_RRAMC_HAS_SECURE_CONFIG
    NRF_RRAMC_REGION_PERM_SECURE_MASK  = RRAMC_REGION_CONFIG_SECURE_Msk,  ///< Secure-only access.
#endif
} nrf_rramc_region_perm_mask_t;

/** @brief RRAMC region configuration. */
typedef struct
{
    uint32_t address;     ///< Start address of the region.
    uint32_t permissions; ///< Permissions created using @ref nrf_rramc_region_perm_mask_t.
    bool     writeonce;   ///< True if writes to the region are to be applied only when the current data is 0xFFFFFFFF.
    bool     lock;        ///< True if memory belonging to given region is to be read-only.
    uint16_t size_kb;     ///< Region size in kBs. */
} nrf_rramc_region_config_t;

/**
 * @brief Function for activating the specified RRAMC task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_rramc_task_trigger(NRF_RRAMC_Type * p_reg, nrf_rramc_task_t task);

/**
 * @brief Function for getting the address of the specified RRAMC task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  RRAMC task.
 *
 * @return Address of the specified task register.
 */
NRF_STATIC_INLINE uint32_t nrf_rramc_task_address_get(NRF_RRAMC_Type const * p_reg,
                                                      nrf_rramc_task_t       task);

/**
 * @brief Function for clearing the specified RRAMC event.
 *
 * @param[in] p_reg Pointer to the peripheral register structure.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_rramc_event_clear(NRF_RRAMC_Type * p_reg, nrf_rramc_event_t event);

/**
 * @brief Function for retrieving the state of the RRAMC event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_rramc_event_check(NRF_RRAMC_Type const * p_reg, nrf_rramc_event_t event);

/**
 * @brief Function for getting the address of the specified RRAMC event register.
 *
 * @param[in] p_reg Pointer to the peripheral register structure.
 * @param[in] event Requested event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_rramc_event_address_get(NRF_RRAMC_Type const * p_reg,
                                                       nrf_rramc_event_t      event);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the peripheral register structure.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_rramc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_rramc_int_enable(NRF_RRAMC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the peripheral register structure.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_rramc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_rramc_int_disable(NRF_RRAMC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_rramc_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_rramc_int_enable_check(NRF_RRAMC_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for getting the state of pending interrupts.
 *
 * @note States of pending interrupt are saved as a bitmask.
 *       One set at particular position means that interrupt for event is pending.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Bitmask with information about pending interrupts.
 *                  Use @ref nrf_rramc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE uint32_t nrf_rramc_int_pending_get(NRF_RRAMC_Type const * p_reg);

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a wakeup
 *        RRAMC task.
 *
 * @note Not every task has its corresponding subscribe register.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_rramc_subscribe_set(NRF_RRAMC_Type * p_reg,
                                               nrf_rramc_task_t task,
                                               uint8_t          channel);

/**
 * @brief Function for clearing the subscribe configuration for a wakeup
 *        RRAMC task.
 *
 * @note Not every task has its corresponding subscribe register.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_rramc_subscribe_clear(NRF_RRAMC_Type * p_reg, nrf_rramc_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        RRAMC task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return RRAMC subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_rramc_subscribe_get(NRF_RRAMC_Type const * p_reg,
                                                   nrf_rramc_task_t       task);

/**
 * @brief Function for setting the publish configuration for a wokenup
 *        RRAMC event.
 *
 * @note Not every event has its corresponding publish register.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_rramc_publish_set(NRF_RRAMC_Type *  p_reg,
                                             nrf_rramc_event_t event,
                                             uint8_t           channel);

/**
 * @brief Function for clearing the publish configuration for a wokenup
 *        RRAMC event.
 *
 * @note Not every event has its corresponding publish register.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_rramc_publish_clear(NRF_RRAMC_Type * p_reg, nrf_rramc_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        RRAMC event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return RRAMC publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_rramc_publish_get(NRF_RRAMC_Type const * p_reg,
                                                 nrf_rramc_event_t      event);

#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for checking current RRAMC operation status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Current operation is completed, and RRAMC is ready.
 * @retval false RRAMC is busy.
 */
NRF_STATIC_INLINE bool nrf_rramc_ready_check(NRF_RRAMC_Type const * p_reg);

/**
 * @brief Function for checking whether RRAMC is ready to accept a new write operation.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  RRAMC is ready to accept a new write operation.
 * @retval false RRAMC cannot accept any write operation now.
 */
NRF_STATIC_INLINE bool nrf_rramc_write_ready_check(NRF_RRAMC_Type const * p_reg);

/**
 * @brief Fuction for checking the address of the first access error.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Access error address.
 */
NRF_STATIC_INLINE uint32_t nrf_rramc_error_access_addr_get(NRF_RRAMC_Type const * p_reg);

/**
 * @brief Function for checking whether the internal write-buffer has been committed to RRAM and is now empty.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The internal write-buffer is empty and has no content that needs to be commited.
 * @retval false The internal write-buffer has data that needs to be committed.
 */
NRF_STATIC_INLINE bool nrf_rramc_empty_buffer_check(NRF_RRAMC_Type const * p_reg);

/**
 * @brief Function for getting the RRAMC peripheral configuration.
 *
 * @param[in]  p_reg    Pointer to the structure of registers of the peripheral.
 * @param[out] p_config Pointer to the structure to be filled with RRAMC configuration data.
 */
NRF_STATIC_INLINE void nrf_rramc_config_get(NRF_RRAMC_Type const * p_reg,
                                            nrf_rramc_config_t *   p_config);

/**
 * @brief Function for setting the RRAMC peripheral configuration.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the structure with configuration to be set.
 */
NRF_STATIC_INLINE void nrf_rramc_config_set(NRF_RRAMC_Type *           p_reg,
                                            nrf_rramc_config_t const * p_config);

/**
 * @brief Function for getting preload timeout value for waiting for a next write.
 *
 * @param[in]  p_reg    Pointer to the structure of registers of the peripheral.
 * @param[out] p_config Pointer to the structure to be filled with information about
 *                      preload timeout value.
 */
NRF_STATIC_INLINE void nrf_rramc_ready_next_timeout_get(NRF_RRAMC_Type const *           p_reg,
                                                        nrf_rramc_ready_next_timeout_t * p_config);

/**
 * @brief Function for setting preload timeout value for waiting for a next write.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the structure filled with information about$ preload
 *                     timeout value.
 */
NRF_STATIC_INLINE
void nrf_rramc_ready_next_timeout_set(NRF_RRAMC_Type *                       p_reg,
                                      nrf_rramc_ready_next_timeout_t const * p_config);

/**
 * @brief Function for getting the RRAMC power configuration.
 *
 * @param[in]  p_reg    Pointer to the structure of registers of the peripheral.
 * @param[out] p_config Pointer to the structure to be filled with information about
 *                      power configuration.
 */
NRF_STATIC_INLINE void nrf_rramc_power_config_get(NRF_RRAMC_Type const * p_reg,
                                                  nrf_rramc_power_t *    p_config);

/**
 * @brief Function for setting the RRAMC power configuration.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the structure filled with information about power configuration.
 */
NRF_STATIC_INLINE void nrf_rramc_power_config_set(NRF_RRAMC_Type *          p_reg,
                                                  nrf_rramc_power_t const * p_config);

/**
 * @brief Function for checking if the erasing operation of the whole RRAM main block has been started.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Erase of chip started.
 * @retval false No operation.
 */
NRF_STATIC_INLINE bool nrf_rramc_erase_all_check(NRF_RRAMC_Type const * p_reg);

/**
 * @brief Function for erasing whole RRAM main block, that includes the SICR and the UICR.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_rramc_erase_all_set(NRF_RRAMC_Type * p_reg);

/**
 * @brief Function for setting the configuration of the specified RRAMC region.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] region   Region number.
 * @param[in] p_config Pointer to the configuration structure.
 */
NRF_STATIC_INLINE void nrf_rramc_region_config_set(NRF_RRAMC_Type *                  p_reg,
                                                   uint8_t                           region,
                                                   nrf_rramc_region_config_t const * p_config);

/**
 * @brief Function for getting the configuration of the specified RRAMC region.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] region   Region number.
 * @param[in] p_config Pointer to the structure to be filled with RRAMC region settings.
 */
NRF_STATIC_INLINE void nrf_rramc_region_config_get(NRF_RRAMC_Type const *      p_reg,
                                                   uint8_t                     region,
                                                   nrf_rramc_region_config_t * p_config);

/**
 * @brief Function for getting the raw configuration of the specified RRAMC region.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] region Region number.
 *
 * @return Raw configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_rramc_region_config_raw_get(NRF_RRAMC_Type const * p_reg,
                                                           uint8_t                region);

/**
 * @brief Function for writing a byte to RRAM memory.
 *
 * @note If write buffer is enabled, new data might not be immediately committed
 *       to the target memory.
 *
 * @warning Before calling this function, the caller must ensure that write mode
 *          is enabled using @ref nrf_rramc_config_set.
 *
 * @param[in] address Address of the byte to write.
 * @param[in] value   Value to write.
 */
NRF_STATIC_INLINE void nrf_rramc_byte_write(uint32_t address, uint8_t value);

/**
 * @brief Function for writing a halfword to RRAM memory.
 *
 * @note If write buffer is enabled, new data might not be immediately committed
 *       to the target memory.
 *
 * @warning Before calling this function, the caller must ensure that write mode
 *          is enabled using @ref nrf_rramc_config_set.
 *
 * @param[in] address Address of the halfword to write.
 * @param[in] value   Value to write.
 */
NRF_STATIC_INLINE void nrf_rramc_halfword_write(uint32_t address, uint16_t value);

/**
 * @brief Function for writing a word to RRAM memory.
 *
 * @note If write buffer is enabled, new data might not be immediately committed
 *       to the target memory.
 *
 * @warning Before calling this function, the caller must ensure that write mode
 *          is enabled using @ref nrf_rramc_config_set.
 *
 * @param[in] address Address of the word to write.
 * @param[in] value   Value to write.
 */
NRF_STATIC_INLINE void nrf_rramc_word_write(uint32_t address, uint32_t value);

/**
 * @brief Function for writing a given number of bytes from a specified buffer
 *        into RRAM memory.
 *
 * @note If write buffer is enabled, new data might not be immediately committed
 *       to the target memory.
 *
 * @warning Before calling this function, the caller must ensure that write mode
 *          is enabled using @ref nrf_rramc_config_set.
 *
 * @param[in] address   Destination address in RRAM.
 * @param[in] src       Pointer to the buffer from where data will be copied.
 * @param[in] num_bytes Number of bytes to write.
 *
 */
NRF_STATIC_INLINE void nrf_rramc_buffer_write(uint32_t address, void * src, uint32_t num_bytes);

/**
 * @brief Function for reading a byte from the RRAM memory.
 *
 * @param[in] address Address of the byte to read.
 *
 * @return Value read from RRAM memory.
 */
NRF_STATIC_INLINE uint8_t nrf_rramc_byte_read(uint32_t address);

/**
 * @brief Function for reading a halfword from the RRAM memory.
 *
 * @param[in] address Address of the halfword to read.
 *
 * @return Value read from RRAM memory.
 */
NRF_STATIC_INLINE uint16_t nrf_rramc_halfword_read(uint32_t address);

/**
 * @brief Function for reading a word from the RRAM memory.
 *
 * @param[in] address Address of the word to read.
 *
 * @return Value read from RRAM memory.
 */
NRF_STATIC_INLINE uint32_t nrf_rramc_word_read(uint32_t address);

/**
 * @brief Function for reading a given number of bytes from RRAM memory
 *        into a specified buffer.
 *
 * @param[in] dst       Pointer to the buffer to store the data.
 * @param[in] address   Address of the first byte to read.
 * @param[in] num_bytes Number of bytes to read.
 */
NRF_STATIC_INLINE void nrf_rramc_buffer_read(void * dst, uint32_t address, uint32_t num_bytes);

/**
 * @brief Function for setting RRAMC low power mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mode  RRAMC low power mode.
 */
NRF_STATIC_INLINE void nrf_rramc_lp_mode_set(NRF_RRAMC_Type * p_reg, nrf_rramc_lp_mode_t mode);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_rramc_task_trigger(NRF_RRAMC_Type * p_reg, nrf_rramc_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_rramc_task_address_get(NRF_RRAMC_Type const * p_reg,
                                                      nrf_rramc_task_t       task)
{
    return ((uint32_t)p_reg + (uint32_t)task);
}

NRF_STATIC_INLINE void nrf_rramc_event_clear(NRF_RRAMC_Type * p_reg, nrf_rramc_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_rramc_event_check(NRF_RRAMC_Type const * p_reg, nrf_rramc_event_t event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_rramc_event_address_get(NRF_RRAMC_Type const * p_reg,
                                                       nrf_rramc_event_t      event)
{
    return ((uint32_t)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE void nrf_rramc_int_enable(NRF_RRAMC_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_rramc_int_disable(NRF_RRAMC_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_rramc_int_enable_check(NRF_RRAMC_Type const * p_reg,
                                                      uint32_t               mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE uint32_t nrf_rramc_int_pending_get(NRF_RRAMC_Type const * p_reg)
{
    return p_reg->INTPEND;
}

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_rramc_subscribe_set(NRF_RRAMC_Type * p_reg,
                                               nrf_rramc_task_t task,
                                               uint8_t          channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_rramc_subscribe_clear(NRF_RRAMC_Type * p_reg, nrf_rramc_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_rramc_subscribe_get(NRF_RRAMC_Type const * p_reg,
                                                   nrf_rramc_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_rramc_publish_set(NRF_RRAMC_Type *  p_reg,
                                             nrf_rramc_event_t event,
                                             uint8_t           channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_rramc_publish_clear(NRF_RRAMC_Type * p_reg, nrf_rramc_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_rramc_publish_get(NRF_RRAMC_Type const * p_reg,
                                                 nrf_rramc_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

NRF_STATIC_INLINE bool nrf_rramc_ready_check(NRF_RRAMC_Type const * p_reg)
{
    return ((p_reg->READY & RRAMC_READY_READY_Msk) >>
            RRAMC_READY_READY_Pos) ==
            RRAMC_READY_READY_Ready;
}

NRF_STATIC_INLINE bool nrf_rramc_write_ready_check(NRF_RRAMC_Type const * p_reg)
{
    return ((p_reg->READYNEXT & RRAMC_READYNEXT_READYNEXT_Msk) >>
            RRAMC_READYNEXT_READYNEXT_Pos) ==
            RRAMC_READYNEXT_READYNEXT_Ready;
}

NRF_STATIC_INLINE uint32_t nrf_rramc_error_access_addr_get(NRF_RRAMC_Type const * p_reg)
{
    return (uint32_t)p_reg->ACCESSERRORADDR;
}

NRF_STATIC_INLINE bool nrf_rramc_empty_buffer_check(NRF_RRAMC_Type const * p_reg)
{
    return ((p_reg->BUFSTATUS.WRITEBUFEMPTY & RRAMC_BUFSTATUS_WRITEBUFEMPTY_EMPTY_Msk) >>
            RRAMC_BUFSTATUS_WRITEBUFEMPTY_EMPTY_Pos) ==
            RRAMC_BUFSTATUS_WRITEBUFEMPTY_EMPTY_Empty;
}

NRF_STATIC_INLINE void nrf_rramc_config_get(NRF_RRAMC_Type const * p_reg,
                                            nrf_rramc_config_t *   p_config)
{
    p_config->mode_write = (bool)((p_reg->CONFIG & RRAMC_CONFIG_WEN_Msk) >>
                                  RRAMC_CONFIG_WEN_Pos);
    p_config->write_buff_size = (uint32_t)((p_reg->CONFIG & RRAMC_CONFIG_WRITEBUFSIZE_Msk) >>
                                           RRAMC_CONFIG_WRITEBUFSIZE_Pos);
}

NRF_STATIC_INLINE void nrf_rramc_config_set(NRF_RRAMC_Type *           p_reg,
                                            nrf_rramc_config_t const * p_config)
{
    NRFX_ASSERT(p_config->write_buff_size <= NRF_RRAMC_CONFIG_WRITE_BUFF_SIZE_MAX);

    p_reg->CONFIG = ((uint32_t)p_config->mode_write      << RRAMC_CONFIG_WEN_Pos) |
                    ((uint32_t)p_config->write_buff_size << RRAMC_CONFIG_WRITEBUFSIZE_Pos);
}

NRF_STATIC_INLINE void nrf_rramc_ready_next_timeout_get(NRF_RRAMC_Type const *           p_reg,
                                                        nrf_rramc_ready_next_timeout_t * p_config)
{
    p_config->value = (uint16_t)((p_reg->READYNEXTTIMEOUT & RRAMC_READYNEXTTIMEOUT_VALUE_Msk) >>
                                 RRAMC_READYNEXTTIMEOUT_VALUE_Pos);
    p_config->enable = ((p_reg->READYNEXTTIMEOUT & RRAMC_READYNEXTTIMEOUT_EN_Msk)
                        >> RRAMC_READYNEXTTIMEOUT_EN_Pos) == RRAMC_READYNEXTTIMEOUT_EN_Enable;
}

NRF_STATIC_INLINE void
nrf_rramc_ready_next_timeout_set(NRF_RRAMC_Type *                       p_reg,
                                 nrf_rramc_ready_next_timeout_t const * p_config)
{
    NRFX_ASSERT(p_config->value <= NRF_RRAMC_READYNEXTTIMEOUT_MAX);

    p_reg->READYNEXTTIMEOUT = ((uint32_t)p_config->value << RRAMC_READYNEXTTIMEOUT_VALUE_Pos) |
                              ((p_config->enable ? RRAMC_READYNEXTTIMEOUT_EN_Enable :
                                                   RRAMC_READYNEXTTIMEOUT_EN_Disable)
                               << RRAMC_READYNEXTTIMEOUT_EN_Pos);
}

NRF_STATIC_INLINE void nrf_rramc_power_config_get(NRF_RRAMC_Type const * p_reg,
                                                  nrf_rramc_power_t *    p_config)
{
    p_config->access_timeout = (uint16_t)((p_reg->POWER.CONFIG &
                                          RRAMC_POWER_CONFIG_ACCESSTIMEOUT_Msk) >>
                                          RRAMC_POWER_CONFIG_ACCESSTIMEOUT_Pos);
    p_config->abort_on_pof = ((p_reg->POWER.CONFIG &
                                RRAMC_POWER_CONFIG_POF_Msk) >>
                                RRAMC_POWER_CONFIG_POF_Pos) ==
                                RRAMC_POWER_CONFIG_POF_Abort;
}

NRF_STATIC_INLINE void nrf_rramc_power_config_set(NRF_RRAMC_Type *          p_reg,
                                                  nrf_rramc_power_t const * p_config)
{
    p_reg->POWER.CONFIG =
            ((uint32_t)p_config->access_timeout << RRAMC_POWER_CONFIG_ACCESSTIMEOUT_Pos) |
            ((uint32_t)p_config->abort_on_pof   << RRAMC_POWER_CONFIG_POF_Pos);
}

NRF_STATIC_INLINE bool nrf_rramc_erase_all_check(NRF_RRAMC_Type const * p_reg)
{
    return ((p_reg->ERASE.ERASEALL & RRAMC_ERASE_ERASEALL_ERASE_Msk) >>
            RRAMC_ERASE_ERASEALL_ERASE_Pos) ==
            RRAMC_ERASE_ERASEALL_ERASE_Erase;
}

NRF_STATIC_INLINE void nrf_rramc_erase_all_set(NRF_RRAMC_Type * p_reg)
{
    p_reg->ERASE.ERASEALL = RRAMC_ERASE_ERASEALL_ERASE_Erase;
}

NRF_STATIC_INLINE void nrf_rramc_region_config_set(NRF_RRAMC_Type *                  p_reg,
                                                   uint8_t                           region,
                                                   nrf_rramc_region_config_t const * p_config)
{
    p_reg->REGION[region].ADDRESS = p_config->address;
    p_reg->REGION[region].CONFIG = (((p_config->permissions
                                      & NRF_RRAMC_REGION_CONFIG_PERM_MASK)) |
                                    ((p_config->writeonce ? RRAMC_REGION_CONFIG_WRITEONCE_Enabled :
                                                            RRAMC_REGION_CONFIG_WRITEONCE_Disabled)
                                     << RRAMC_REGION_CONFIG_WRITEONCE_Pos) |
                                    ((p_config->lock ? RRAMC_REGION_CONFIG_LOCK_Enabled:
                                                       RRAMC_REGION_CONFIG_LOCK_Disabled)
                                     << RRAMC_REGION_CONFIG_LOCK_Pos) |
                                    ((p_config->size_kb << RRAMC_REGION_CONFIG_SIZE_Pos)
                                     & RRAMC_REGION_CONFIG_SIZE_Msk));
}

NRF_STATIC_INLINE void nrf_rramc_region_config_get(NRF_RRAMC_Type const *      p_reg,
                                                   uint8_t                     region,
                                                   nrf_rramc_region_config_t * p_config)
{
    uint32_t reg = p_reg->REGION[region].CONFIG;
    p_config->permissions = reg & NRF_RRAMC_REGION_CONFIG_PERM_MASK;
    p_config->writeonce   = ((reg & RRAMC_REGION_CONFIG_WRITEONCE_Msk)
                             >> RRAMC_REGION_CONFIG_WRITE_Pos) ==
                            RRAMC_REGION_CONFIG_WRITEONCE_Enabled;
    p_config->lock        = ((reg & RRAMC_REGION_CONFIG_LOCK_Msk)
                             >> RRAMC_REGION_CONFIG_LOCK_Pos) == RRAMC_REGION_CONFIG_LOCK_Enabled;
    p_config->size_kb     = (reg & RRAMC_REGION_CONFIG_SIZE_Msk) >> RRAMC_REGION_CONFIG_SIZE_Pos;
    p_config->address     = p_reg->REGION[region].ADDRESS;
}

NRF_STATIC_INLINE uint32_t nrf_rramc_region_config_raw_get(NRF_RRAMC_Type const * p_reg,
                                                           uint8_t                region)
{
    return p_reg->REGION[region].CONFIG;
}

NRF_STATIC_INLINE void nrf_rramc_byte_write(uint32_t address, uint8_t value)
{
    *(volatile uint8_t *)address = value;
}

NRF_STATIC_INLINE void nrf_rramc_halfword_write(uint32_t address, uint16_t value)
{
    *(volatile uint16_t *)address = value;
}

NRF_STATIC_INLINE void nrf_rramc_word_write(uint32_t address, uint32_t value)
{
    *(volatile uint32_t *)address = value;
}

NRF_STATIC_INLINE void nrf_rramc_buffer_write(uint32_t address, void * src, uint32_t num_bytes)
{
    memcpy((void *)address, src, num_bytes);
}

NRF_STATIC_INLINE uint8_t nrf_rramc_byte_read(uint32_t address)
{
    return *(volatile uint8_t *)address;
}

NRF_STATIC_INLINE uint16_t nrf_rramc_halfword_read(uint32_t address)
{
    return *(volatile uint16_t *)address;
}

NRF_STATIC_INLINE uint32_t nrf_rramc_word_read(uint32_t address)
{
    return *(volatile uint32_t *)address;
}

NRF_STATIC_INLINE void nrf_rramc_buffer_read(void * dst, uint32_t address, uint32_t num_bytes)
{
    memcpy(dst, (void *)address, num_bytes);
}

NRF_STATIC_INLINE void nrf_rramc_lp_mode_set(NRF_RRAMC_Type * p_reg, nrf_rramc_lp_mode_t mode)
{
    p_reg->POWER.LOWPOWERCONFIG = (p_reg->POWER.LOWPOWERCONFIG &
                                   ~RRAMC_POWER_LOWPOWERCONFIG_MODE_Msk) |
                                  (((uint32_t)mode << RRAMC_POWER_LOWPOWERCONFIG_MODE_Pos) &
                                   RRAMC_POWER_LOWPOWERCONFIG_MODE_Msk);
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_RRAM_H__
