/*
 * Copyright (c) 2022 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_MRAMC_H__
#define NRF_MRAMC_H__

#include <nrfx.h>
#include <nrf_bitmask.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_mramc_hal MRAMC HAL
 * @{
 * @ingroup nrf_mramc
 * @brief   Hardware access layer for managing the Magnetoresistive Random Access Memory Controller (MRAMC) peripheral.
 */

#if (defined(MRAMC_CONFIGNVR_PAGE_UREN_Msk) && defined(MRAMC_CONFIGNVR_PAGE_UWEN_Msk)) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether CONFIGNVR.PAGE[n] registers have upper part of NVR page read and write protection fields. */
#define NRF_MRAMC_HAS_CONFIGNVR_PAGE_UPPER_PROTECT 1
#else
#define NRF_MRAMC_HAS_CONFIGNVR_PAGE_UPPER_PROTECT 0
#endif

#if (defined(MRAMC_CONFIGNVR_PAGE_LREN_Msk) && defined(MRAMC_CONFIGNVR_PAGE_LWEN_Msk) && \
     defined(MRAMC_CONFIGNVR_PAGE_LRSIZE_Msk) && defined(MRAMC_CONFIGNVR_PAGE_LWSIZE_Msk)) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether CONFIGNVR.PAGE[n] registers have lower part of NVR page read and write protection fields. */
#define NRF_MRAMC_HAS_CONFIGNVR_PAGE_LOWER_PROTECT 1
#else
#define NRF_MRAMC_HAS_CONFIGNVR_PAGE_LOWER_PROTECT 0
#endif

#if defined(MRAMC_CONFIG_DISABLEECC_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the CONFIG register has a DISABLEECC field. */
#define NRF_MRAMC_HAS_CONFIG_DISABLEECC 1
#else
#define NRF_MRAMC_HAS_CONFIG_DISABLEECC 0
#endif

#if defined(MRAMC_POWER_MASK_VREFVPR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the POWER_MASK register has a VREFVPR field. */
#define NRF_MRAMC_HAS_POWER_VREFVPR 1
#else
#define NRF_MRAMC_HAS_POWER_VREFVPR 0
#endif

#if defined(MRAMC_WAITSTATES_RDY_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the WAITSTATES register has a RDY field. */
#define NRF_MRAMC_HAS_WAITSTATES_RDY 1
#else
#define NRF_MRAMC_HAS_WAITSTATES_RDY 0
#endif

#if defined(MRAMC_POWER_MASK_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the POWER_MASK register is present. */
#define NRF_MRAMC_HAS_POWER_MASK 1
#else
#define NRF_MRAMC_HAS_POWER_MASK 0
#endif

/** @brief Maximum timeout value for entering read mode in MRAMC clock cycles. */
#define NRF_MRAMC_AUTOREADMODE_MAX          MRAMC_AUTOREADMODE_VALUE_Max
/** @brief MRAM word size in bytes. */
#define NRF_MRAMC_BUS_SIZE                  MRAMC_NMRAMWORDSIZE
/** @brief Maximum value of waitstate number. */
#define NRF_MRAMC_WAITSTATENUM_MAX          MRAMC_WAITSTATES_WAITSTATENUM_Max
/** @brief Maximum timeout value for waiting for next write in in MRAMC clock cycles. */
#define NRF_MRAMC_READYNEXTTIMEOUT_MAX      MRAMC_READYNEXTTIMEOUT_VALUE_Max
/** @brief Default timeout value for waiting for next write in in MRAMC clock cycles. */
#define NRF_MRAMC_READYNEXTTIMEOUT_DEFAULT  MRAMC_READYNEXTTIMEOUT_ResetValue
/** @brief Minimum timeout value for low average current read in MRAMC clock cycles. */
#define NRF_MRAMC_LOWAVGCURR_READ_MIN       MRAMC_LOWAVGCURR_READ_VALUE_Min
/** @brief Minimum timeout value for low average current write in MRAMC clock cycles. */
#define NRF_MRAMC_LOWAVGCURR_WRITE_MIN      MRAMC_LOWAVGCURR_WRITE_VALUE_Min
/** @brief Minimum timeout value for low average current erase in MRAMC clock cycles. */
#define NRF_MRAMC_LOWAVGCURR_ERASE_MIN      MRAMC_LOWAVGCURR_ERASE_VALUE_Min
/** @brief Maximum timeout value for low average current read in MRAMC clock cycles. */
#define NRF_MRAMC_LOWAVGCURR_READ_MAX       MRAMC_LOWAVGCURR_READ_VALUE_Max
/** @brief Maximum timeout value for low average current write in MRAMC clock cycles. */
#define NRF_MRAMC_LOWAVGCURR_WRITE_MAX      MRAMC_LOWAVGCURR_WRITE_VALUE_Max
/** @brief Maximum timeout value for low average current erase in MRAMC clock cycles. */
#define NRF_MRAMC_LOWAVGCURR_ERASE_MAX      MRAMC_LOWAVGCURR_ERASE_VALUE_Max
/** @brief Minimum timeout value for automatic power down in MRAMC clock cycles. */
#define NRF_MRAMC_AUTOPOWERDOWN_TIMEOUT_MIN MRAMC_POWER_AUTOPOWERDOWN_TIMEOUTVALUE_Min
/** @brief Maximum timeout value for automatic power down in MRAMC clock cycles. */
#define NRF_MRAMC_AUTOPOWERDOWN_TIMEOUT_MAX MRAMC_POWER_AUTOPOWERDOWN_TIMEOUTVALUE_Max
/** @brief Minimum size of erase operation in MRAM words. */
#define NRF_MRAMC_ERASE_SIZE_MIN            MRAMC_ERASE_SIZE_SIZE_Min
/** @brief Maximum size of erase operation in MRAM words. */
#define NRF_MRAMC_ERASE_SIZE_MAX            MRAMC_ERASE_SIZE_SIZE_Max
/** @brief Maximum number of NVR pages. */
#define NRF_MRAMC_CONFIGNVR_PAGE_MAX        MRAMC_CONFIGNVR_PAGE_MaxCount
/** @brief Minimum size of lower 2KB memory in NR page to disable read access. */
#define NRF_MRAMC_CONFIGNVR_PAGE_LRSIZE_MIN MRAMC_CONFIGNVR_PAGE_LRSIZE_Min
/** @brief Minimum size of lower 2KB memory in NR page to disable write access. */
#define NRF_MRAMC_CONFIGNVR_PAGE_LWSIZE_MIN MRAMC_CONFIGNVR_PAGE_LWSIZE_Min
/** @brief Maximum size of lower 2KB memory in NR page to disable read access. */
#define NRF_MRAMC_CONFIGNVR_PAGE_LRSIZE_MAX MRAMC_CONFIGNVR_PAGE_LRSIZE_Max
/** @brief Maximum size of lower 2KB memory in NR page to disable write access. */
#define NRF_MRAMC_CONFIGNVR_PAGE_LWSIZE_MAX MRAMC_CONFIGNVR_PAGE_LWSIZE_Max

/** @brief MRAMC events. */
typedef enum
{
    NRF_MRAMC_EVENT_READY         = offsetof(NRF_MRAMC_Type, EVENTS_READY),         ///< Current operation is completed.
    NRF_MRAMC_EVENT_READYNEXT     = offsetof(NRF_MRAMC_Type, EVENTS_READYNEXT),     ///< Ready to accept a new write operation.
    NRF_MRAMC_EVENT_ECCERROR      = offsetof(NRF_MRAMC_Type, EVENTS_ECCERROR),      ///< ECC error detected but not corrected.
    NRF_MRAMC_EVENT_ECCERRORCORR  = offsetof(NRF_MRAMC_Type, EVENTS_ECCERRORCORR),  ///< ECC error detected and corrected.
    NRF_MRAMC_EVENT_TRIMCONFIGREQ = offsetof(NRF_MRAMC_Type, EVENTS_TRIMCONFIGREQ), ///< MRAM trim configuration request. The MRAMC is ready for the MRAM trim configuration.
    NRF_MRAMC_EVENT_ACCESSERR     = offsetof(NRF_MRAMC_Type, EVENTS_ACCESSERR),     ///< MRAM access error.
} nrf_mramc_event_t;

/** @brief MRAMC interrupts. */
typedef enum
{
    NRF_MRAMC_INT_READY_MASK         = MRAMC_INTENSET_READY_Msk,         ///< Interrupt on READY event.
    NRF_MRAMC_INT_READYNEXT_MASK     = MRAMC_INTENSET_READYNEXT_Msk,     ///< Interrupt on READYNEXT event.
    NRF_MRAMC_INT_ECCERROR_MASK      = MRAMC_INTENSET_ECCERROR_Msk,      ///< Interrupt on ECCERROR event.
    NRF_MRAMC_INT_ECCERRORCORR_MASK  = MRAMC_INTENSET_ECCERRORCORR_Msk,  ///< Interrupt on ECCERRORCORR event.
    NRF_MRAMC_INT_TRIMCONFIGREQ_MASK = MRAMC_INTENSET_TRIMCONFIGREQ_Msk, ///< Interrupt on TRIMCONFIGREQ event.
    NRF_MRAMC_INT_ACCESSERR_MASK     = MRAMC_INTENSET_ACCESSERR_Msk,     ///< Interrupt on ACCESSERR event.
    NRF_MRAMC_ALL_INTS_MASK          = MRAMC_INTENSET_READY_Msk         |
                                       MRAMC_INTENSET_READYNEXT_Msk     |
                                       MRAMC_INTENSET_ECCERROR_Msk      |
                                       MRAMC_INTENSET_ECCERRORCORR_Msk  |
                                       MRAMC_INTENSET_TRIMCONFIGREQ_Msk |
                                       MRAMC_INTENSET_ACCESSERR_Msk       ///< All MRAMC interrupts.
} nrf_mramc_int_mask_t;

/** @brief Write enable (WEN) settings. */
typedef enum
{
    NRF_MRAMC_MODE_WRITE_DISABLE = MRAMC_CONFIG_WEN_DisableWrite,      ///< Write is disabled.
    NRF_MRAMC_MODE_WRITE_NORMAL  = MRAMC_CONFIG_WEN_EnableNormalWrite, ///< Normal write is enabled.
    NRF_MRAMC_MODE_WRITE_DIRECT  = MRAMC_CONFIG_WEN_EnableDirectWrite, ///< Direct write is enabled.
} nrf_mramc_mode_write_t;

/** @brief Erase enable (EEN) settings. */
typedef enum
{
    NRF_MRAMC_MODE_ERASE_DISABLE = MRAMC_CONFIG_EEN_DisableErase,    ///< Erase is disabled but read is allowed.
    NRF_MRAMC_MODE_ERASE_PAGE    = MRAMC_CONFIG_EEN_EnablePageErase, ///< Erase page and read are allowed.
    NRF_MRAMC_MODE_ERASE_WORD    = MRAMC_CONFIG_EEN_EnableWordErase, ///< Erase word and read are allowed.
} nrf_mramc_mode_erase_t;

/** @brief Power mode settings. */
typedef enum
{
    NRF_MRAMC_POWER_INIT_MODE_NO_OPERATION  = MRAMC_POWER_INIT_MODE_NoOperation,      ///< No change in power mode.
    NRF_MRAMC_POWER_INIT_MODE_UP            = MRAMC_POWER_INIT_MODE_PowerUp,          ///< Triggers power-up sequence.
    NRF_MRAMC_POWER_INIT_MODE_DOWN          = MRAMC_POWER_INIT_MODE_PowerDown,        ///< Initiates power-down sequence.
    NRF_MRAMC_POWER_INIT_MODE_DOWN_TRIM_RET = MRAMC_POWER_INIT_MODE_PowerDownTrimRet, ///< Initiates power-down sequence with trim retained.
} nrf_mramc_power_init_t;

/** @brief Power mode status. */
typedef enum
{
    NRF_MRAMC_POWER_STATUS_OFF             = MRAMC_POWER_STATUS_STATE_Off,            ///< MRAM is OFF.
    NRF_MRAMC_POWER_STATUS_POWER_UP_SEQ    = MRAMC_POWER_STATUS_STATE_PowerUpSeq,     ///< MRAM power-up sequence is active.
    NRF_MRAMC_POWER_STATUS_TRIM_CFG_SEQ    = MRAMC_POWER_STATUS_STATE_TrimConfigSeq,  ///< Request for the MRAM trim configuration.
    NRF_MRAMC_POWER_STATUS_TRIM_CFG_WAIT   = MRAMC_POWER_STATUS_STATE_TrimConfigWait, ///< Waiting for the MRAM trim configuration to complete.
    NRF_MRAMC_POWER_STATUS_TRIM_CFG_DONE   = MRAMC_POWER_STATUS_STATE_TrimConfigDone, ///< The MRAM trim configuration to completed (This is momentary state).
    NRF_MRAMC_POWER_STATUS_STANDBY         = MRAMC_POWER_STATUS_STATE_Standby,        ///< MRAM is in standby mode.
    NRF_MRAMC_POWER_STATUS_POWER_DOWN_SEQ  = MRAMC_POWER_STATUS_STATE_PowerDownSeq,   ///< MRAM power-down sequence is active.
    NRF_MRAMC_POWER_STATUS_OFF_TRIM_RETAIN = MRAMC_POWER_STATUS_STATE_OffTrimRetain,  ///< MRAM is OFF with trim configuration in retain.
} nrf_mramc_power_status_t;

/** @brief Trim configuration is completed - values for read operation. */
typedef enum
{
    NRF_MRAMC_TRIM_READ_UNKNOWN   = MRAMC_TRIM_DONE_TRIMCOMPLETED_TrimUnknown,   ///< Read: Trim configuration is unknown.
    NRF_MRAMC_TRIM_READ_COMPLETED = MRAMC_TRIM_DONE_TRIMCOMPLETED_TrimCompleted, ///< Read: Trim configuration data write is completed.
} nrf_mramc_trim_t;

/** @brief Error Correction Code (ECC) information. */
typedef struct
{
    uint32_t error_addr; ///< Address of the first ECC error that could not be corrected.
    uint32_t corr_addr;  ///< Address of the first ECC error that was corrected.
    bool     error;      ///< ECC error detected which could not be corrected.
    bool     corr;       ///< ECC error detected and corrected.
} nrf_mramc_ecc_t;

/** @brief MRAMC configuration structure. */
typedef struct
{
    nrf_mramc_mode_write_t mode_write;  ///< Write enable settings.
    nrf_mramc_mode_erase_t mode_erase;  ///< Erase enable settings.
#if NRF_MRAMC_HAS_CONFIG_DISABLEECC
    bool                   disable_ecc; ///< Disable ECC. It is enabled by default.
#endif
} nrf_mramc_config_t;


/** @brief Waitstates for MRAM read access.*/
typedef struct
{
    uint8_t  waitstate; ///< Register to read the current number of waitstate for the MRAM access and set a new value.
#if NRF_MRAMC_HAS_WAITSTATES_RDY
    bool     ready;     ///< Use RDY (ready) signal from the MRAM macro in addiiton to waitstates.
#endif
} nrf_mramc_waitstates_t;


/** @brief Preload timeout value for waiting for a next write. */
typedef struct
{
    uint16_t value;        ///< Preload value.
    bool     direct_write; ///< Triggers a write to MRAMC macro on the ready next timeout in direct write mode (@ref nrf_mramc_mode_write_t).
} nrf_mramc_readynext_timeout_t;

/** @brief Preload timeout value for low average current in case of read, write, and erase. */
typedef struct
{
    uint16_t read;  ///< Read preload timeout value for low average current in case of read.
    uint16_t write; ///< Preload timeout value for low average current in case of write.
    uint16_t erase; ///< Preload timeout value for low average current in case of erase.
} nrf_mramc_lowavgcurr_t;

/** @brief Structure for configurating the automatic power-down feature using the inactive time period of MRAM. */
typedef struct
{
    bool     enable;         ///< Enable the automatic power-down feature.
    bool     power_down_cfg; ///< Power down mode when the timeout happens.
    uint16_t timeout_value;  ///< Timeout value for the power-down.
} nrf_mramc_power_autopowerdown_t;

#if NRF_MRAMC_HAS_POWER_MASK
/** @brief Mask for the various voltages supplies when initiating power-up/down. */
typedef struct
{
    bool vdd;     ///< Mask VDD.
    bool vdda;    ///< Mask VDDA.
    bool vddcfg;  ///< Mask VDD_CFG.
#if NRF_MRAMC_HAS_POWER_VREFVPR
    bool vrefvpr; ///< Mask VREFVPR.
#else
    bool vpr;     ///< Mask VPR.
    bool vref;    ///< Mask VREF.
#endif
} nrf_mramc_power_conf_t;
#endif

/** @brief Configuration structure for NVR page n. */
typedef struct
{
    nrf_mramc_mode_write_t wen;    ///< Write enable.
    nrf_mramc_mode_erase_t een;    ///< Erase enable.
    bool                   lock;   ///< Enables the lock for this register.
    bool                   uren;   ///< Enable read access to the upper part of NVR page, where the upper part size is NVR page size - 2KB.
    bool                   uwen;   ///< Enable write access to the upper part of NVR page, where the upper part size is NVR page size - 2KB.
    bool                   lren;   ///< Enable read access to the lower range of the NVR page, as defined by LRSIZE.
    bool                   lwen;   ///< Enable write access to the lower range of the NVR page, as defined by LWSIZE.
    uint8_t                lrsize; ///< Size of part of the lower 2KB memory in the NVR page to disable read access, expressed in 128-byte units.
    uint8_t                lwsize; ///< Size of part of the lower 2KB memory in the NVR page to disable write access, expressed in 128-byte units.
} nrf_mramc_config_nvr_t;

/**
 * @brief Function for clearing the specified MRAMC event.
 *
 * @param[in] p_reg Pointer to the peripheral register structure.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_mramc_event_clear(NRF_MRAMC_Type * p_reg, nrf_mramc_event_t event);

/**
 * @brief Function for retrieving the state of the MRAMC event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_mramc_event_check(NRF_MRAMC_Type const * p_reg,
                                             nrf_mramc_event_t      event);

/**
 * @brief Function for getting the address of the specified MRAMC event register.
 *
 * @param[in] p_reg Pointer to the peripheral register structure.
 * @param[in] event Requested event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_mramc_event_address_get(NRF_MRAMC_Type const * p_reg,
                                                       nrf_mramc_event_t      event);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the peripheral register structure.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_mramc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_mramc_int_enable(NRF_MRAMC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the peripheral register structure.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_mramc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_mramc_int_disable(NRF_MRAMC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_mramc_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_mramc_int_enable_check(NRF_MRAMC_Type const * p_reg,
                                                      uint32_t               mask);

/**
 * @brief Function for retrieving the state of pending interrupts.
 *
 * @note States of pending interrupt are saved as a bitmask.
 *       One set at particular position means that interrupt for event is pending.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Bitmask with information about pending interrupts.
 *         Use @ref nrf_mramc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE uint32_t nrf_mramc_int_pending_get(NRF_MRAMC_Type const * p_reg);

/**
 * @brief Function for checking current MRAMC operation status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Current operation is completed and MRAMC is ready.
 * @retval false MRAMC is busy.
 */
NRF_STATIC_INLINE bool nrf_mramc_ready_get(NRF_MRAMC_Type const * p_reg);

/**
 * @brief Function for checking READYNEXT register status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  MRAMC is ready to accept a new write operation.
 * @retval false MRAMC cannot accept any write operation now.
 */
NRF_STATIC_INLINE bool nrf_mramc_readynext_get(NRF_MRAMC_Type const * p_reg);

/**
 * @brief Function for getting Error Correction Code (ECC) information.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_data Pointer to the data structure to be filled with ECC information.
 */
NRF_STATIC_INLINE void nrf_mramc_ecc_get(NRF_MRAMC_Type const * p_reg,
                                         nrf_mramc_ecc_t *      p_data);

/**
 * @brief Function for clearing flag indicating detected error which could not be corrected.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_mramc_ecc_error_clear(NRF_MRAMC_Type * p_reg);

/**
 * @brief Function for clearing flag indicating detected error which was corrected.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_mramc_ecc_corr_clear(NRF_MRAMC_Type * p_reg);

/**
 * @brief Function for setting the MRAMC peripheral configuration.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the structure containing configuration to be set.
 */
NRF_STATIC_INLINE void nrf_mramc_config_set(NRF_MRAMC_Type *           p_reg,
                                            nrf_mramc_config_t const * p_config);

/**
 * @brief Function for getting the MRAMC peripheral configuration.
 *
 * @param[in]  p_reg    Pointer to the structure of registers of the peripheral.
 * @param[out] p_config Pointer to the structure to be filled with MRAMC configuration data.
 */
NRF_STATIC_INLINE void nrf_mramc_config_get(NRF_MRAMC_Type const * p_reg,
                                            nrf_mramc_config_t *   p_config);

/**
 * @brief Function for setting the timeout value for automatic read mode.
 *
 * @note The timeout value is number of MRAMC clock cycles. This feature is disabled
 *       when the value is 0. The CONFIG is updated on timeout.
 *       Timeout value must be in range: 0-4095.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] timeout Timeout value.
 */
NRF_STATIC_INLINE void nrf_mramc_autoreadmode_set(NRF_MRAMC_Type * p_reg, uint16_t timeout);

/**
 * @brief Function for getting the timeout value for automatic read mode.
 *
 * @note The timeout value is number of MRAMC clock cycles. This feature is disabled
 *       when the value is 0.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Timeout value.
 */
NRF_STATIC_INLINE uint16_t nrf_mramc_autoreadmode_get(NRF_MRAMC_Type const * p_reg);

/**
 * @brief Function for setting wait states for MRAM read access.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_data Pointer to the structure filled with information about waitstates
 *                   for MRAM read access.
 */
NRF_STATIC_INLINE void nrf_mramc_waitstates_set(NRF_MRAMC_Type *               p_reg,
                                                nrf_mramc_waitstates_t const * p_data);

/**
 * @brief Function for reading wait states for MRAM read access.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_data Pointer to the data structure to be filled with waitstates information
 *                    for MRAM read access.
 */
NRF_STATIC_INLINE void nrf_mramc_waitstates_get(NRF_MRAMC_Type const *   p_reg,
                                                nrf_mramc_waitstates_t * p_data);

/**
 * @brief Function for setting preload timeout value for waiting for a next write.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_data Pointer to the structure filled with information about$ preload
 *                   timeout value.
 */
NRF_STATIC_INLINE
void nrf_mramc_readynext_timeout_set(NRF_MRAMC_Type *                      p_reg,
                                     nrf_mramc_readynext_timeout_t const * p_data);

/**
 * @brief Function for reading preload timeout value for waiting for a next write.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_data Pointer to the structure to be filled with information about
 *             preload timeout value.
 */
NRF_STATIC_INLINE
void nrf_mramc_readynext_timeout_get(NRF_MRAMC_Type const *          p_reg,
                                     nrf_mramc_readynext_timeout_t * p_data);

/**
 * @brief Function for setting preload timeout value for low average current in case of read,
 *        write and erase.
 *
 * @note The timeout value is number of MRAMC clock cycles, feature is disabled when the value is 0.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_data Pointer to the structure filled with information about preload timeout value
 *                   for average current in case of read, write, and errase operation.
 */
NRF_STATIC_INLINE void nrf_mramc_lowavgcurr_set(NRF_MRAMC_Type *               p_reg,
                                                nrf_mramc_lowavgcurr_t const * p_data);

/**
 * @brief Function for getting preload timeout value for low average current in case of read,
 *        write and erase.
 *
 * @note The timeout value is number of MRAMC clock cycles, feature is disabled when the value is 0.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_data Pointer to the structure to be filled with information about preload timeout
 *                    value for average current in case of read, write, and errase operation.
 */
NRF_STATIC_INLINE void nrf_mramc_lowavgcurr_get(NRF_MRAMC_Type const *   p_reg,
                                                nrf_mramc_lowavgcurr_t * p_data);

/**
 * @brief Function for setting power-up/down sequence.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mode  Power init mode.
 */
NRF_STATIC_INLINE void nrf_mramc_power_init_set(NRF_MRAMC_Type *       p_reg,
                                                nrf_mramc_power_init_t mode);

/**
 * @brief Function for getting power-up/down sequence.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Power init mode.
 */
NRF_STATIC_INLINE nrf_mramc_power_init_t nrf_mramc_power_init_get(NRF_MRAMC_Type const * p_reg);

/**
 * @brief Function for setting automatic power-down feature using the inactive time period of MRAM.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_data Pointer to the structure filled with information about power-down feature
 *                   configuration.
 */
NRF_STATIC_INLINE
void nrf_mramc_power_autopowerdown_set(NRF_MRAMC_Type *                        p_reg,
                                       nrf_mramc_power_autopowerdown_t const * p_data);

/**
 * @brief Function for getting automatic power-down feature using the inactive time period of MRAM.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_data Pointer to the structure to be filled with information about power-down
 *                    feature configuration.
 */
NRF_STATIC_INLINE
void nrf_mramc_power_autopowerdown_get(NRF_MRAMC_Type const *            p_reg,
                                       nrf_mramc_power_autopowerdown_t * p_data);

#if NRF_MRAMC_HAS_POWER_MASK
/**
 * @brief Function for setting mask for the various voltages supplies when initiating power-up/down.
 *
 * @note All bits must be set to 0 for normal operation of MRAM. Incorrect usage would
 *       result in unknown behavior of MRAM.
 *
 * @warning Do not use this function unless you know the consequences.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_data Pointer to the structure filled with information about various voltages
 *                   supplies when initiating power-up/down.
 */
NRF_STATIC_INLINE void nrf_mramc_power_mask_set(NRF_MRAMC_Type *               p_reg,
                                                nrf_mramc_power_conf_t const * p_data);

/**
 * @brief Function for getting mask for the various voltages supplies when initiating power-up/down.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_data Pointer to the structure to be filled with information about various
 *                    voltages supplies when initiating power-up/down.
 */
NRF_STATIC_INLINE void nrf_mramc_power_mask_get(NRF_MRAMC_Type const *   p_reg,
                                                nrf_mramc_power_conf_t * p_data);
#endif

/**
 * @brief Function for getting the power mode status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Status of the power mode.
 */
NRF_STATIC_INLINE nrf_mramc_power_status_t nrf_mramc_power_status_get(NRF_MRAMC_Type const * p_reg);

#if NRF_MRAMC_HAS_POWER_MASK
/**
 * @brief Function for getting status of the power control signals acknowledgement
 *        during power-up sequence.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_data Pointer to the structure to be filled with information about
 *                    status of the power control signals acknowledgement
 *                    during power-up sequence.
 */
NRF_STATIC_INLINE void nrf_mramc_powerup_ack_get(NRF_MRAMC_Type const *   p_reg,
                                                 nrf_mramc_power_conf_t * p_data);

/**
 * @brief Function for getting status of the power control signals acknowledgement
 *        during power-down sequence.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_data Pointer to the structure to be filled with information about
 *                    status of the power control signals acknowledgement
 *                    during power-down sequence.
 */
NRF_STATIC_INLINE void nrf_mramc_powerdown_ack_get(NRF_MRAMC_Type const *   p_reg,
                                                   nrf_mramc_power_conf_t * p_data);

/**
 * @brief Function for setting the configuration of force ON signal of the power supply.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_data Pointer to the structure filled with information about force ON power supply.
*/
NRF_STATIC_INLINE void nrf_mramc_power_force_on_set(NRF_MRAMC_Type *               p_reg,
                                                    nrf_mramc_power_conf_t const * p_data);

/**
 * @brief Function for getting force ON the power supply.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_data Pointer to the structure to be filled with information about force ON
 *                    power supply.
 */
NRF_STATIC_INLINE void nrf_mramc_power_force_on_get(NRF_MRAMC_Type const *   p_reg,
                                                    nrf_mramc_power_conf_t * p_data);

/**
 * @brief Function for setting force OFF the power supply.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_data Pointer to the structure filled with information about force OFF power supply.
 */
NRF_STATIC_INLINE void nrf_mramc_power_force_off_set(NRF_MRAMC_Type *               p_reg,
                                                     nrf_mramc_power_conf_t const * p_data);

/**
 * @brief Function for getting force OFF the power supply.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_data Pointer to the structure to be filled with information about force OFF
 *                    power supply.
 */
NRF_STATIC_INLINE void nrf_mramc_power_force_off_get(NRF_MRAMC_Type const *   p_reg,
                                                     nrf_mramc_power_conf_t * p_data);
#endif

/**
 * @brief Function for setting data to be written to the MRAM trim configuration.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] data  Data to be written to the MRAM trim configuration.
 */
NRF_STATIC_INLINE void nrf_mramc_trim_datain_set(NRF_MRAMC_Type * p_reg, uint32_t data);

/**
 * @brief Function for getting data written to the MRAM trim configuration.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Data written to the MRAM trim configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_mramc_trim_datain_get(NRF_MRAMC_Type const * p_reg);

/**
 * @brief Function for getting data read from the MRAM trim configuration.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Data read from the MRAM trim configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_mramc_trim_dataout_get(NRF_MRAMC_Type const * p_reg);

/**
 * @brief Function for setting number of bits in the current data to be written to
 *        and read from the trim configuration.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] bits  Number of bits to be written.
 */
NRF_STATIC_INLINE void nrf_mramc_trim_count_set(NRF_MRAMC_Type * p_reg, uint32_t bits);

/**
 * @brief Function for getting number of bits in the current data to be written to
 *        and read from the trim configuration.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Number of bits to be written.
 */
NRF_STATIC_INLINE uint32_t nrf_mramc_trim_count_get(NRF_MRAMC_Type const * p_reg);

/**
 * @brief Function for setting start shifting data for trim configuration.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_mramc_trim_start(NRF_MRAMC_Type * p_reg);

/**
 * @brief Function for getting start shifting data for trim configuration.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Start trim config.
 * @retval false No operation.
 */

NRF_STATIC_INLINE bool nrf_mramc_trim_start_get(NRF_MRAMC_Type const * p_reg);

/**
 * @brief Function for getting status of shifting the MRAM trim configuration data.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Ready for shifting the next MRAM trim configuration data.
 * @retval false Shifting the MRAM trim configuration data is in progress.
 */
NRF_STATIC_INLINE bool nrf_mramc_trim_ready_get(NRF_MRAMC_Type const * p_reg);

/**
 * @brief Function for indicating to the MRAMC that the whole MRAM trim configuration
 *        data write is completed.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_mramc_trim_done_set(NRF_MRAMC_Type * p_reg);

/**
 * @brief Function for getting trim configuration data status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Trim configuration status.
 */
NRF_STATIC_INLINE nrf_mramc_trim_t nrf_mramc_trim_done_get(NRF_MRAMC_Type const * p_reg);

/**
 * @brief Function for erasing a 32-bit word in the MRAM main block.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] address Address of the word that needs to be erased in the MRAM.
 */
NRF_STATIC_INLINE void nrf_mramc_erase_word_set(NRF_MRAMC_Type * p_reg, uint32_t address);

/**
 * @brief Function for getting address of word in the MRAM main block to be erased.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Address of the word.
 */
NRF_STATIC_INLINE uint32_t nrf_mramc_erase_word_get(NRF_MRAMC_Type const * p_reg);

/**
 * @brief Function for erasing an area in the MRAM main block.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] address Start address of the area that needs to be erased in the MRAM.
 * @param[in] size    Number of MRAM words to be erased.
 */
NRF_STATIC_INLINE void nrf_mramc_erase_area_set(NRF_MRAMC_Type * p_reg,
                                                uint32_t         address,
                                                uint32_t         size);

/**
 * @brief Function for getting address and size of the area to be erased in the MRAM main block.
 *
 * @param[in]  p_reg     Pointer to the structure of registers of the peripheral.
 * @param[out] p_address Pointer to the address of the area.
 * @param[out] p_size    Pointer to the number of MRAM words to be erased.
 */
NRF_STATIC_INLINE void nrf_mramc_erase_area_get(NRF_MRAMC_Type const * p_reg,
                                                uint32_t *             p_address,
                                                uint32_t *             p_size);

/**
 * @brief Function for erasing erasing whole MRAM main block and the first NVR page.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_mramc_erase_all(NRF_MRAMC_Type * p_reg);

/**
 * @brief Function for getting ERASE.ERASEALL register value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Start erase of chip.
 * @retval false No operation.
 */
NRF_STATIC_INLINE bool nrf_mramc_erase_all_get(NRF_MRAMC_Type const * p_reg);

/**
 * @brief Function for setting the eraseword lock status.
 *
 * @note When the lock is enabled, the write to the corresponding erase register
 *       is ignored, it remains enabled until the next reset cycle.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] lock  Set true to enable or false to disable a lock.
 *
 */
NRF_STATIC_INLINE void nrf_mramc_erase_word_lock_set(NRF_MRAMC_Type * p_reg, bool lock);

/**
 * @brief Function for getting the eraseword lock status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Lock enabled.
 * @retval false Lock disabled.
 */
NRF_STATIC_INLINE bool nrf_mramc_erase_word_lock_get(NRF_MRAMC_Type const * p_reg);

/**
 * @brief Function for setting the erasearea lock status.
 *
 * @note When the lock is enabled, the write to the corresponding erase register
 *       is ignored, it remains enabled until the next
 *       reset cycle.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] lock  Set true to enable or false to disable a lock.
 *
 */
NRF_STATIC_INLINE void nrf_mramc_erase_area_lock_set(NRF_MRAMC_Type * p_reg, bool lock);

/**
 * @brief Function for getting the erasearea lock status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Lock enabled.
 * @retval false Lock disabled.
 */
NRF_STATIC_INLINE bool nrf_mramc_erase_area_lock_get(NRF_MRAMC_Type const * p_reg);

/**
 * @brief Function for setting the eraseall lock status.
 *
 * @note When the lock is enabled, the write to the corresponding erase register
 *       is ignored, it remains enabled until the next
 *       reset cycle.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] lock  Set true to enable or false to disable a lock.
 *
 */
NRF_STATIC_INLINE void nrf_mramc_erase_all_lock_set(NRF_MRAMC_Type * p_reg, bool lock);

/**
 * @brief Function for getting the eraseall lock status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Lock enabled.
 * @retval false Lock disabled.
 */
NRF_STATIC_INLINE bool nrf_mramc_erase_all_lock_get(NRF_MRAMC_Type const * p_reg);

/**
 * @brief Function for configuring the specified NVR page.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_data Pointer to the settings data.
 * @param[in] page   Page number [0...3].
 */
NRF_STATIC_INLINE void nrf_mramc_config_nvr_set(NRF_MRAMC_Type *               p_reg,
                                                nrf_mramc_config_nvr_t const * p_data,
                                                uint8_t                        page);

/**
 * @brief Function for getting the configuring of the specified NVR page.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_data Pointer to the settings data.
 * @param[in]  page   Page number [0...3].
 */
NRF_STATIC_INLINE void nrf_mramc_config_nvr_get(NRF_MRAMC_Type const *   p_reg,
                                                nrf_mramc_config_nvr_t * p_data,
                                                uint8_t                  page);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_mramc_event_clear(NRF_MRAMC_Type * p_reg, nrf_mramc_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_mramc_event_check(NRF_MRAMC_Type const * p_reg,
                                             nrf_mramc_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_mramc_event_address_get(NRF_MRAMC_Type const * p_reg,
                                                       nrf_mramc_event_t      event)
{
    return ((uint32_t)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE void nrf_mramc_int_enable(NRF_MRAMC_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_mramc_int_disable(NRF_MRAMC_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_mramc_int_enable_check(NRF_MRAMC_Type const * p_reg,
                                                      uint32_t               mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE uint32_t nrf_mramc_int_pending_get(NRF_MRAMC_Type const * p_reg)
{
    return p_reg->INTPEND;
}

NRF_STATIC_INLINE bool nrf_mramc_ready_get(NRF_MRAMC_Type const * p_reg)
{
    return (bool)p_reg->READY;
}

NRF_STATIC_INLINE bool nrf_mramc_readynext_get(NRF_MRAMC_Type const * p_reg)
{
    return (bool)p_reg->READYNEXT;
}

NRF_STATIC_INLINE void nrf_mramc_ecc_get(NRF_MRAMC_Type const * p_reg,
                                         nrf_mramc_ecc_t *      p_data)
{
    p_data->error_addr = p_reg->ECC.ERRORADDR;
    p_data->corr_addr  = p_reg->ECC.CORRADDR;
    p_data->error      = (bool)p_reg->ECC.ERROR;
    p_data->corr       = (bool)p_reg->ECC.ERRORCORR;
}

NRF_STATIC_INLINE void nrf_mramc_ecc_error_clear(NRF_MRAMC_Type * p_reg)
{
    p_reg->ECC.ERROR = 0;
}

NRF_STATIC_INLINE void nrf_mramc_ecc_corr_clear(NRF_MRAMC_Type * p_reg)
{
    p_reg->ECC.ERRORCORR = 0;
}

NRF_STATIC_INLINE void nrf_mramc_config_set(NRF_MRAMC_Type *           p_reg,
                                            nrf_mramc_config_t const * p_config)
{
    p_reg->CONFIG = ((uint32_t)p_config->mode_write  << MRAMC_CONFIG_WEN_Pos) |
#if NRF_MRAMC_HAS_CONFIG_DISABLEECC
                    ((uint32_t)p_config->disable_ecc << MRAMC_CONFIG_DISABLEECC_Pos) |
#endif
                    ((uint32_t)p_config->mode_erase  << MRAMC_CONFIG_EEN_Pos);
}

NRF_STATIC_INLINE void nrf_mramc_config_get(NRF_MRAMC_Type const * p_reg,
                                            nrf_mramc_config_t *   p_config)
{
    p_config->mode_write  = (nrf_mramc_mode_write_t)((p_reg->CONFIG & MRAMC_CONFIG_WEN_Msk) >>
                                                     MRAMC_CONFIG_WEN_Pos);
    p_config->mode_erase  = (nrf_mramc_mode_erase_t)((p_reg->CONFIG & MRAMC_CONFIG_EEN_Msk) >>
                                                     MRAMC_CONFIG_EEN_Pos);
#if NRF_MRAMC_HAS_CONFIG_DISABLEECC
    p_config->disable_ecc = (bool)(p_reg->CONFIG & MRAMC_CONFIG_DISABLEECC_Msk);
#endif
}

NRF_STATIC_INLINE void nrf_mramc_autoreadmode_set(NRF_MRAMC_Type * p_reg, uint16_t timeout)
{
    NRFX_ASSERT(timeout <= NRF_MRAMC_AUTOREADMODE_MAX);
    p_reg->AUTOREADMODE = (uint32_t)timeout;
}

NRF_STATIC_INLINE uint16_t nrf_mramc_autoreadmode_get(NRF_MRAMC_Type const * p_reg)
{
    return (uint16_t)p_reg->AUTOREADMODE;
}

NRF_STATIC_INLINE void nrf_mramc_waitstates_set(NRF_MRAMC_Type *               p_reg,
                                                nrf_mramc_waitstates_t const * p_data)
{
    NRFX_ASSERT(p_data->waitstate <= NRF_MRAMC_WAITSTATENUM_MAX);
    p_reg->WAITSTATES = ((uint32_t)MRAMC_WAITSTATES_KEY_Enable << MRAMC_WAITSTATES_KEY_Pos) |
#if NRF_MRAMC_HAS_WAITSTATES_RDY
                        ((uint32_t)p_data->ready     	       << MRAMC_WAITSTATES_RDY_Pos) |
#endif
                        ((uint32_t)p_data->waitstate           <<
                         MRAMC_WAITSTATES_WAITSTATENUM_Pos);
}

NRF_STATIC_INLINE void nrf_mramc_waitstates_get(NRF_MRAMC_Type const *   p_reg,
                                                nrf_mramc_waitstates_t * p_data)
{
#if NRF_MRAMC_HAS_WAITSTATES_RDY
    p_data->ready     = (bool)(p_reg->WAITSTATES & MRAMC_WAITSTATES_RDY_Msk);
#endif
    p_data->waitstate = (uint8_t)((p_reg->WAITSTATES & MRAMC_WAITSTATES_WAITSTATENUM_Msk) >>
                                  MRAMC_WAITSTATES_WAITSTATENUM_Pos);
}

NRF_STATIC_INLINE
void nrf_mramc_readynext_timeout_set(NRF_MRAMC_Type *                      p_reg,
                                     nrf_mramc_readynext_timeout_t const * p_data)
{
    NRFX_ASSERT(p_data->value <= NRF_MRAMC_READYNEXTTIMEOUT_MAX);

    p_reg->READYNEXTTIMEOUT = ((uint32_t)p_data->value << MRAMC_READYNEXTTIMEOUT_VALUE_Pos) |
                              ((uint32_t)p_data->direct_write << MRAMC_READYNEXTTIMEOUT_DW_Pos);
}

NRF_STATIC_INLINE
void nrf_mramc_readynext_timeout_get(NRF_MRAMC_Type const *          p_reg,
                                     nrf_mramc_readynext_timeout_t * p_data)
{
    p_data->value = (uint16_t)(p_reg->READYNEXTTIMEOUT & MRAMC_READYNEXTTIMEOUT_VALUE_Msk);
    p_data->direct_write = (bool)(p_reg->READYNEXTTIMEOUT & MRAMC_READYNEXTTIMEOUT_DW_Msk);
}

NRF_STATIC_INLINE void nrf_mramc_lowavgcurr_set(NRF_MRAMC_Type *               p_reg,
                                                nrf_mramc_lowavgcurr_t const * p_data)
{
    NRFX_ASSERT(p_data->read  <= NRF_MRAMC_LOWAVGCURR_READ_MAX);
    NRFX_ASSERT(p_data->write <= NRF_MRAMC_LOWAVGCURR_WRITE_MAX);
    NRFX_ASSERT(p_data->erase <= NRF_MRAMC_LOWAVGCURR_ERASE_MAX);

    p_reg->LOWAVGCURR.READ  = p_data->read;
    p_reg->LOWAVGCURR.WRITE = p_data->write;
    p_reg->LOWAVGCURR.ERASE = p_data->erase;
}

NRF_STATIC_INLINE void nrf_mramc_lowavgcurr_get(NRF_MRAMC_Type const *   p_reg,
                                                nrf_mramc_lowavgcurr_t * p_data)
{
    p_data->read  = (uint16_t)p_reg->LOWAVGCURR.READ;
    p_data->write = (uint16_t)p_reg->LOWAVGCURR.WRITE;
    p_data->erase = (uint16_t)p_reg->LOWAVGCURR.ERASE;
}

NRF_STATIC_INLINE void nrf_mramc_power_init_set(NRF_MRAMC_Type *       p_reg,
                                                nrf_mramc_power_init_t mode)
{
    p_reg->POWER.INIT = (uint32_t)mode | (MRAMC_POWER_INIT_KEY_Enable << MRAMC_POWER_INIT_KEY_Pos);
}

NRF_STATIC_INLINE nrf_mramc_power_init_t nrf_mramc_power_init_get(NRF_MRAMC_Type const * p_reg)
{
    return (nrf_mramc_power_init_t)(p_reg->POWER.INIT & MRAMC_POWER_INIT_MODE_Msk);
}

NRF_STATIC_INLINE
void nrf_mramc_power_autopowerdown_set(NRF_MRAMC_Type *                        p_reg,
                                       nrf_mramc_power_autopowerdown_t const * p_data)
{
    NRFX_ASSERT(p_data->timeout_value <= NRF_MRAMC_AUTOPOWERDOWN_TIMEOUT_MAX);

    p_reg->POWER.AUTOPOWERDOWN =
        (uint32_t)p_data->timeout_value                                                  |
        ((uint32_t)p_data->power_down_cfg << MRAMC_POWER_AUTOPOWERDOWN_POWERDOWNCFG_Pos) |
        ((uint32_t)p_data->enable         << MRAMC_POWER_AUTOPOWERDOWN_ENABLE_Pos);
}

NRF_STATIC_INLINE
void nrf_mramc_power_autopowerdown_get(NRF_MRAMC_Type const *            p_reg,
                                       nrf_mramc_power_autopowerdown_t * p_data)
{
    p_data->timeout_value  =
        (uint16_t)(p_reg->POWER.AUTOPOWERDOWN & MRAMC_POWER_AUTOPOWERDOWN_TIMEOUTVALUE_Msk);
    p_data->power_down_cfg =
        (bool)(p_reg->POWER.AUTOPOWERDOWN & MRAMC_POWER_AUTOPOWERDOWN_POWERDOWNCFG_Msk);
    p_data->enable         =
        (bool)(p_reg->POWER.AUTOPOWERDOWN & MRAMC_POWER_AUTOPOWERDOWN_ENABLE_Msk);
}

#if NRF_MRAMC_HAS_POWER_MASK
NRF_STATIC_INLINE void nrf_mramc_power_mask_set(NRF_MRAMC_Type *               p_reg,
                                                nrf_mramc_power_conf_t const * p_data)
{
    p_reg->POWER.MASK =
        ((uint32_t)MRAMC_POWER_MASK_KEY_Enable << MRAMC_POWER_MASK_KEY_Pos)     |
        ((uint32_t)p_data->vdd                 << MRAMC_POWER_MASK_VDD_Pos)     |
        ((uint32_t)p_data->vdda                << MRAMC_POWER_MASK_VDDA_Pos)    |
        ((uint32_t)p_data->vddcfg              << MRAMC_POWER_MASK_VDDCFG_Pos)  |
#if NRF_MRAMC_HAS_POWER_VREFVPR
        ((uint32_t)p_data->vrefvpr             << MRAMC_POWER_MASK_VREFVPR_Pos);
#else
        ((uint32_t)p_data->vpr                 << MRAMC_POWER_MASK_VPR_Pos)     |
        ((uint32_t)p_data->vref                << MRAMC_POWER_MASK_VREF_Pos);
#endif
}

NRF_STATIC_INLINE void nrf_mramc_power_mask_get(NRF_MRAMC_Type const *   p_reg,
                                                nrf_mramc_power_conf_t * p_data)
{
    p_data->vdd     = (bool)(p_reg->POWER.MASK & MRAMC_POWER_MASK_VDD_Msk);
    p_data->vdda    = (bool)(p_reg->POWER.MASK & MRAMC_POWER_MASK_VDDA_Msk);
    p_data->vddcfg  = (bool)(p_reg->POWER.MASK & MRAMC_POWER_MASK_VDDCFG_Msk);
#if NRF_MRAMC_HAS_POWER_VREFVPR
    p_data->vrefvpr = (bool)(p_reg->POWER.MASK & MRAMC_POWER_MASK_VREFVPR_Msk);
#else
    p_data->vpr     = (bool)(p_reg->POWER.MASK & MRAMC_POWER_MASK_VPR_Msk);
    p_data->vref    = (bool)(p_reg->POWER.MASK & MRAMC_POWER_MASK_VREF_Msk);
#endif
}
#endif

NRF_STATIC_INLINE nrf_mramc_power_status_t nrf_mramc_power_status_get(NRF_MRAMC_Type const * p_reg)
{
    return (nrf_mramc_power_status_t)p_reg->POWER.STATUS;
}

#if NRF_MRAMC_HAS_POWER_MASK
NRF_STATIC_INLINE void nrf_mramc_powerup_ack_get(NRF_MRAMC_Type const *   p_reg,
                                                 nrf_mramc_power_conf_t * p_data)
{
    p_data->vdd     = (bool)(p_reg->POWER.POWERUPACK & MRAMC_POWER_POWERUPACK_VDD_Msk);
    p_data->vdda    = (bool)(p_reg->POWER.POWERUPACK & MRAMC_POWER_POWERUPACK_VDDA_Msk);
    p_data->vddcfg  = (bool)(p_reg->POWER.POWERUPACK & MRAMC_POWER_POWERUPACK_VDDCFG_Msk);
#if NRF_MRAMC_HAS_POWER_VREFVPR
    p_data->vrefvpr = (bool)(p_reg->POWER.POWERUPACK & MRAMC_POWER_POWERUPACK_VREFVPR_Msk);
#else
    p_data->vpr     = (bool)(p_reg->POWER.POWERUPACK & MRAMC_POWER_POWERUPACK_VPR_Msk);
    p_data->vref    = (bool)(p_reg->POWER.POWERUPACK & MRAMC_POWER_POWERUPACK_VREF_Msk);
#endif
}

NRF_STATIC_INLINE void nrf_mramc_powerdown_ack_get(NRF_MRAMC_Type const *   p_reg,
                                                   nrf_mramc_power_conf_t * p_data)
{
    p_data->vdd     = (bool)(p_reg->POWER.POWERDOWNACK & MRAMC_POWER_POWERDOWNACK_VDD_Msk);
    p_data->vdda    = (bool)(p_reg->POWER.POWERDOWNACK & MRAMC_POWER_POWERDOWNACK_VDDA_Msk);
    p_data->vddcfg  = (bool)(p_reg->POWER.POWERDOWNACK & MRAMC_POWER_POWERDOWNACK_VDDCFG_Msk);
#if NRF_MRAMC_HAS_POWER_VREFVPR
    p_data->vrefvpr = (bool)(p_reg->POWER.POWERDOWNACK & MRAMC_POWER_POWERDOWNACK_VREFVPR_Msk);
#else
    p_data->vpr     = (bool)(p_reg->POWER.POWERDOWNACK & MRAMC_POWER_POWERDOWNACK_VPR_Msk);
    p_data->vref    = (bool)(p_reg->POWER.POWERDOWNACK & MRAMC_POWER_POWERDOWNACK_VREF_Msk);
#endif
}

NRF_STATIC_INLINE void nrf_mramc_power_force_on_set(NRF_MRAMC_Type *               p_reg,
                                                    nrf_mramc_power_conf_t const * p_data)
{
    p_reg->POWER.FORCEON =
        ((uint32_t)MRAMC_POWER_FORCEON_KEY_Enable << MRAMC_POWER_FORCEON_KEY_Pos)     |
        ((uint32_t)p_data->vdd                    << MRAMC_POWER_FORCEON_VDD_Pos)     |
        ((uint32_t)p_data->vdda                   << MRAMC_POWER_FORCEON_VDDA_Pos)    |
        ((uint32_t)p_data->vddcfg                 << MRAMC_POWER_FORCEON_VDDCFG_Pos)  |
#if NRF_MRAMC_HAS_POWER_VREFVPR
        ((uint32_t)p_data->vrefvpr                << MRAMC_POWER_FORCEON_VREFVPR_Pos);
#else
        ((uint32_t)p_data->vpr                    << MRAMC_POWER_FORCEON_VPR_Pos)     |
        ((uint32_t)p_data->vref                   << MRAMC_POWER_FORCEON_VREF_Pos);
#endif
}

NRF_STATIC_INLINE void nrf_mramc_power_force_on_get(NRF_MRAMC_Type const *   p_reg,
                                                    nrf_mramc_power_conf_t * p_data)
{
    p_data->vdd     = (bool)(p_reg->POWER.FORCEON & MRAMC_POWER_FORCEON_VDD_Msk);
    p_data->vdda    = (bool)(p_reg->POWER.FORCEON & MRAMC_POWER_FORCEON_VDDA_Msk);
    p_data->vddcfg  = (bool)(p_reg->POWER.FORCEON & MRAMC_POWER_FORCEON_VDDCFG_Msk);
#if NRF_MRAMC_HAS_POWER_VREFVPR
    p_data->vrefvpr = (bool)(p_reg->POWER.FORCEON & MRAMC_POWER_FORCEON_VREFVPR_Msk);
#else
    p_data->vpr     = (bool)(p_reg->POWER.FORCEON & MRAMC_POWER_FORCEON_VPR_Msk);
    p_data->vref    = (bool)(p_reg->POWER.FORCEON & MRAMC_POWER_FORCEON_VREF_Msk);
#endif
}

NRF_STATIC_INLINE void nrf_mramc_power_force_off_set(NRF_MRAMC_Type *               p_reg,
                                                     nrf_mramc_power_conf_t const * p_data)
{
    p_reg->POWER.FORCEOFF =
       ((uint32_t)MRAMC_POWER_FORCEOFF_KEY_Enable << MRAMC_POWER_FORCEOFF_KEY_Pos)     |
       ((uint32_t)p_data->vdd                     << MRAMC_POWER_FORCEOFF_VDD_Pos)     |
       ((uint32_t)p_data->vdda                    << MRAMC_POWER_FORCEOFF_VDDA_Pos)    |
       ((uint32_t)p_data->vddcfg                  << MRAMC_POWER_FORCEOFF_VDDCFG_Pos)  |
#if NRF_MRAMC_HAS_POWER_VREFVPR
       ((uint32_t)p_data->vrefvpr                 << MRAMC_POWER_FORCEOFF_VREFVPR_Pos);
#else
       ((uint32_t)p_data->vpr                     << MRAMC_POWER_FORCEOFF_VPR_Pos)     |
       ((uint32_t)p_data->vref                    << MRAMC_POWER_FORCEOFF_VREF_Pos);
#endif
}

NRF_STATIC_INLINE void nrf_mramc_power_force_off_get(NRF_MRAMC_Type const *   p_reg,
                                                     nrf_mramc_power_conf_t * p_data)
{
    p_data->vdd     = (bool)(p_reg->POWER.FORCEOFF & MRAMC_POWER_FORCEOFF_VDD_Msk);
    p_data->vdda    = (bool)(p_reg->POWER.FORCEOFF & MRAMC_POWER_FORCEOFF_VDDA_Msk);
    p_data->vddcfg  = (bool)(p_reg->POWER.FORCEOFF & MRAMC_POWER_FORCEOFF_VDDCFG_Msk);
#if NRF_MRAMC_HAS_POWER_VREFVPR
    p_data->vrefvpr = (bool)(p_reg->POWER.FORCEOFF & MRAMC_POWER_FORCEOFF_VREFVPR_Msk);
#else
    p_data->vpr     = (bool)(p_reg->POWER.FORCEOFF & MRAMC_POWER_FORCEOFF_VPR_Msk);
    p_data->vref    = (bool)(p_reg->POWER.FORCEOFF & MRAMC_POWER_FORCEOFF_VREF_Msk);
#endif
}
#endif

NRF_STATIC_INLINE void nrf_mramc_trim_datain_set(NRF_MRAMC_Type * p_reg, uint32_t data)
{
    p_reg->TRIM.DATAIN = data;
}

NRF_STATIC_INLINE uint32_t nrf_mramc_trim_datain_get(NRF_MRAMC_Type const * p_reg)
{
    return p_reg->TRIM.DATAIN;
}

NRF_STATIC_INLINE uint32_t nrf_mramc_trim_dataout_get(NRF_MRAMC_Type const * p_reg)
{
    return p_reg->TRIM.DATAOUT;
}

NRF_STATIC_INLINE void nrf_mramc_trim_count_set(NRF_MRAMC_Type * p_reg, uint32_t bits)
{
    p_reg->TRIM.COUNT = bits;
}

NRF_STATIC_INLINE uint32_t nrf_mramc_trim_count_get(NRF_MRAMC_Type const * p_reg)
{
    return p_reg->TRIM.COUNT;
}

NRF_STATIC_INLINE void nrf_mramc_trim_start(NRF_MRAMC_Type * p_reg)
{
    p_reg->TRIM.START = MRAMC_TRIM_START_START_Start |
                        (MRAMC_TRIM_START_KEY_Enable << MRAMC_TRIM_START_KEY_Pos);
}

NRF_STATIC_INLINE bool nrf_mramc_trim_start_get(NRF_MRAMC_Type const * p_reg)
{
    return (bool)(p_reg->TRIM.START & MRAMC_TRIM_START_START_Msk);
}

NRF_STATIC_INLINE bool nrf_mramc_trim_ready_get(NRF_MRAMC_Type const * p_reg)
{
    return (bool)p_reg->TRIM.READY;
}

NRF_STATIC_INLINE void nrf_mramc_trim_done_set(NRF_MRAMC_Type * p_reg)
{
    p_reg->TRIM.DONE = MRAMC_TRIM_DONE_TRIMCOMPLETED_TrimComplete;
}

NRF_STATIC_INLINE nrf_mramc_trim_t nrf_mramc_trim_done_get(NRF_MRAMC_Type const * p_reg)
{
    return (nrf_mramc_trim_t)p_reg->TRIM.DONE;
}

NRF_STATIC_INLINE void nrf_mramc_erase_word_set(NRF_MRAMC_Type * p_reg, uint32_t address)
{
    p_reg->ERASE.ERASEWORD = address;
}

NRF_STATIC_INLINE uint32_t nrf_mramc_erase_word_get(NRF_MRAMC_Type const * p_reg)
{
    return p_reg->ERASE.ERASEWORD;
}

NRF_STATIC_INLINE void nrf_mramc_erase_area_set(NRF_MRAMC_Type * p_reg,
                                                uint32_t         address,
                                                uint32_t         size)
{
    NRFX_ASSERT((size >= NRF_MRAMC_ERASE_SIZE_MIN) && (size <= NRF_MRAMC_ERASE_SIZE_MAX));
    p_reg->ERASE.SIZE      = size;
    p_reg->ERASE.ERASEAREA = address;
}

NRF_STATIC_INLINE void nrf_mramc_erase_area_get(NRF_MRAMC_Type const * p_reg,
                                                uint32_t *             p_address,
                                                uint32_t *             p_size)
{
    *p_address = p_reg->ERASE.ERASEAREA;
    *p_size    = p_reg->ERASE.SIZE;
}

NRF_STATIC_INLINE void nrf_mramc_erase_all(NRF_MRAMC_Type * p_reg)
{
    p_reg->ERASE.ERASEALL = MRAMC_ERASE_ERASEALL_ERASEALL_Erase;
}

NRF_STATIC_INLINE bool nrf_mramc_erase_all_get(NRF_MRAMC_Type const * p_reg)
{
    return (bool)p_reg->ERASE.ERASEALL;
}

NRF_STATIC_INLINE void nrf_mramc_erase_word_lock_set(NRF_MRAMC_Type * p_reg, bool lock)
{
    uint32_t mask = p_reg->ERASE.LOCK;

    if (lock)
    {
        nrf_bitmask_bit_set(MRAMC_ERASE_LOCK_ERASEWORD_Pos, &mask);
    }
    else
    {
        nrf_bitmask_bit_clear(MRAMC_ERASE_LOCK_ERASEWORD_Pos, &mask);
    }

    p_reg->ERASE.LOCK = mask;
}

NRF_STATIC_INLINE bool nrf_mramc_erase_word_lock_get(NRF_MRAMC_Type const * p_reg)
{
    return (bool)(p_reg->ERASE.LOCK & MRAMC_ERASE_LOCK_ERASEWORD_Msk);
}

NRF_STATIC_INLINE void nrf_mramc_erase_area_lock_set(NRF_MRAMC_Type * p_reg, bool lock)
{
    uint32_t mask = p_reg->ERASE.LOCK;

    if (lock)
    {
        nrf_bitmask_bit_set(MRAMC_ERASE_LOCK_ERASEAREA_Pos, &mask);
    }
    else
    {
        nrf_bitmask_bit_clear(MRAMC_ERASE_LOCK_ERASEAREA_Pos, &mask);
    }

    p_reg->ERASE.LOCK = mask;
}

NRF_STATIC_INLINE bool nrf_mramc_erase_area_lock_get(NRF_MRAMC_Type const * p_reg)
{
    return (bool)(p_reg->ERASE.LOCK & MRAMC_ERASE_LOCK_ERASEAREA_Msk);
}

NRF_STATIC_INLINE void nrf_mramc_erase_all_lock_set(NRF_MRAMC_Type * p_reg, bool lock)
{
    uint32_t mask = p_reg->ERASE.LOCK;

    if (lock)
    {
        nrf_bitmask_bit_set(MRAMC_ERASE_LOCK_ERASEALL_Pos, &mask);
    }
    else
    {
        nrf_bitmask_bit_clear(MRAMC_ERASE_LOCK_ERASEALL_Pos, &mask);
    }

    p_reg->ERASE.LOCK = mask;
}

NRF_STATIC_INLINE bool nrf_mramc_erase_all_lock_get(NRF_MRAMC_Type const * p_reg)
{
    return (bool)(p_reg->ERASE.LOCK & MRAMC_ERASE_LOCK_ERASEALL_Msk);
}

NRF_STATIC_INLINE void nrf_mramc_config_nvr_set(NRF_MRAMC_Type *               p_reg,
                                                nrf_mramc_config_nvr_t const * p_data,
                                                uint8_t                        page)
{
    NRFX_ASSERT(page < NRF_MRAMC_CONFIGNVR_PAGE_MAX);
#if NRF_MRAMC_HAS_CONFIGNVR_PAGE_LOWER_PROTECT
    NRFX_ASSERT((p_data->lrsize <= NRF_MRAMC_CONFIGNVR_PAGE_LRSIZE_MAX) &&
                (p_data->lrsize >= NRF_MRAMC_CONFIGNVR_PAGE_LRSIZE_MIN));
    NRFX_ASSERT((p_data->lwsize <= NRF_MRAMC_CONFIGNVR_PAGE_LWSIZE_MAX) &&
                (p_data->lwsize >= NRF_MRAMC_CONFIGNVR_PAGE_LWSIZE_MIN));
#endif

    p_reg->CONFIGNVR.PAGE[page] = ((uint32_t)p_data->wen  << MRAMC_CONFIGNVR_PAGE_WEN_Pos) |
                                  ((uint32_t)p_data->een  << MRAMC_CONFIGNVR_PAGE_EEN_Pos) |
#if NRF_MRAMC_HAS_CONFIGNVR_PAGE_UPPER_PROTECT
                                  ((uint32_t)p_data->uren  << MRAMC_CONFIGNVR_PAGE_UREN_Pos) |
                                  ((uint32_t)p_data->uwen  << MRAMC_CONFIGNVR_PAGE_UWEN_Pos) |
#endif
#if NRF_MRAMC_HAS_CONFIGNVR_PAGE_LOWER_PROTECT
                                  ((uint32_t)p_data->lren  << MRAMC_CONFIGNVR_PAGE_LREN_Pos) |
                                  ((uint32_t)p_data->lwen  << MRAMC_CONFIGNVR_PAGE_LWEN_Pos) |
                                  ((uint32_t)p_data->lrsize  << MRAMC_CONFIGNVR_PAGE_LRSIZE_Pos) |
                                  ((uint32_t)p_data->lwsize  << MRAMC_CONFIGNVR_PAGE_LWSIZE_Pos) |
#endif
                                  ((uint32_t)p_data->lock << MRAMC_CONFIGNVR_PAGE_LOCK_Pos);
}

NRF_STATIC_INLINE void nrf_mramc_config_nvr_get(NRF_MRAMC_Type const *   p_reg,
                                                nrf_mramc_config_nvr_t * p_data,
                                                uint8_t                  page)
{
    NRFX_ASSERT(page < NRF_MRAMC_CONFIGNVR_PAGE_MAX);

    p_data->wen  = (nrf_mramc_mode_write_t)
                    (p_reg->CONFIGNVR.PAGE[page] & MRAMC_CONFIGNVR_PAGE_WEN_Msk);
    p_data->een  = (nrf_mramc_mode_erase_t)
                    (p_reg->CONFIGNVR.PAGE[page] & MRAMC_CONFIGNVR_PAGE_EEN_Msk);
    p_data->lock = (bool)(p_reg->CONFIGNVR.PAGE[page] & MRAMC_CONFIGNVR_PAGE_LOCK_Msk);

#if NRF_MRAMC_HAS_CONFIGNVR_PAGE_UPPER_PROTECT
    p_data->uren = (bool)(p_reg->CONFIGNVR.PAGE[page] & MRAMC_CONFIGNVR_PAGE_UREN_Msk);
    p_data->uwen = (bool)(p_reg->CONFIGNVR.PAGE[page] & MRAMC_CONFIGNVR_PAGE_UWEN_Msk);
#endif
#if NRF_MRAMC_HAS_CONFIGNVR_PAGE_LOWER_PROTECT
    p_data->lren = (bool)(p_reg->CONFIGNVR.PAGE[page] & MRAMC_CONFIGNVR_PAGE_LREN_Msk);
    p_data->lwen = (bool)(p_reg->CONFIGNVR.PAGE[page] & MRAMC_CONFIGNVR_PAGE_LWEN_Msk);
    p_data->lrsize = (uint8_t)
                      ((p_reg->CONFIGNVR.PAGE[page] & MRAMC_CONFIGNVR_PAGE_LRSIZE_Msk) >>
                       MRAMC_CONFIGNVR_PAGE_LRSIZE_Pos);
    p_data->lwsize = (uint8_t)
                      ((p_reg->CONFIGNVR.PAGE[page] & MRAMC_CONFIGNVR_PAGE_LWSIZE_Msk) >>
                       MRAMC_CONFIGNVR_PAGE_LWSIZE_Pos);
#endif
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_MRAMC_H__
