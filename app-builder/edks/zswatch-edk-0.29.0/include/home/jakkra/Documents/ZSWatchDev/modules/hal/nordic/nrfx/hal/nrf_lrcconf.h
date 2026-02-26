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

#ifndef NRF_LRCCONF_H__
#define NRF_LRCCONF_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @defgroup nrf_lrcconf_hal LRCCONF HAL
 * @{
 * @ingroup nrf_lrc
 * @brief   Hardware access layer for managing the Local Resource Controller Configuration (LRCCONF) peripheral.
 */

#if defined(LRCCONF_TASKS_REQHFXO_TASKS_REQHFXO_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether HFXO requesting is present. */
#define NRF_LRCCONF_HAS_HFXO 1
#else
#define NRF_LRCCONF_HAS_HFXO 0
#endif

#if defined(LRCCONF_CLKCTRL_SRC_BYPASS_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether clock source bypassing is present. */
#define NRF_LRCCONF_HAS_BYPASS 1
#else
#define NRF_LRCCONF_HAS_BYPASS 0
#endif

#if defined(LRCCONF_AX2XWAITSTATES_MaxCount) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether AXI waitstates are present. */
#define NRF_LRCCONF_HAS_AXI_WAITSTATES 1
#else
#define NRF_LRCCONF_HAS_AXI_WAITSTATES 0
#endif

/** @brief Number of clocks supported by LRCCONF. */
#define NRF_LRCCONF_CLK_COUNT LRCCONF_CLKSTAT_MaxCount

#if NRF_LRCCONF_HAS_AXI_WAITSTATES
/** @brief Size of AXI bridge waitstates array. */
#define NRF_LRCCONF_AXI_WAITSTATES_ARRAY_SIZE LRCCONF_AX2XWAITSTATES_MaxCount
#endif

/** @brief Tasks. */
typedef enum
{
    NRF_LRCCONF_TASK_CLKSTART_0        = offsetof(NRF_LRCCONF_Type, TASKS_REQCLKSRC[0]),       /**< Request the clock source for clock 0. */
    NRF_LRCCONF_TASK_CLKSTART_1        = offsetof(NRF_LRCCONF_Type, TASKS_REQCLKSRC[1]),       /**< Request the clock source for clock 1. */
    NRF_LRCCONF_TASK_CLKSTART_2        = offsetof(NRF_LRCCONF_Type, TASKS_REQCLKSRC[2]),       /**< Request the clock source for clock 2. */
    NRF_LRCCONF_TASK_CLKSTART_3        = offsetof(NRF_LRCCONF_Type, TASKS_REQCLKSRC[3]),       /**< Request the clock source for clock 3. */
    NRF_LRCCONF_TASK_CLKSTART_4        = offsetof(NRF_LRCCONF_Type, TASKS_REQCLKSRC[4]),       /**< Request the clock source for clock 4. */
    NRF_LRCCONF_TASK_CLKSTART_5        = offsetof(NRF_LRCCONF_Type, TASKS_REQCLKSRC[5]),       /**< Request the clock source for clock 5. */
    NRF_LRCCONF_TASK_CLKSTART_6        = offsetof(NRF_LRCCONF_Type, TASKS_REQCLKSRC[6]),       /**< Request the clock source for clock 6. */
    NRF_LRCCONF_TASK_CLKSTART_7        = offsetof(NRF_LRCCONF_Type, TASKS_REQCLKSRC[7]),       /**< Request the clock source for clock 7. */
    NRF_LRCCONF_TASK_CLKSTOP_0         = offsetof(NRF_LRCCONF_Type, TASKS_STOPREQCLKSRC[0]),   /**< Stop requesting the clock source for clock 0. */
    NRF_LRCCONF_TASK_CLKSTOP_1         = offsetof(NRF_LRCCONF_Type, TASKS_STOPREQCLKSRC[1]),   /**< Stop requesting the clock source for clock 1. */
    NRF_LRCCONF_TASK_CLKSTOP_2         = offsetof(NRF_LRCCONF_Type, TASKS_STOPREQCLKSRC[2]),   /**< Stop requesting the clock source for clock 2. */
    NRF_LRCCONF_TASK_CLKSTOP_3         = offsetof(NRF_LRCCONF_Type, TASKS_STOPREQCLKSRC[3]),   /**< Stop requesting the clock source for clock 3. */
    NRF_LRCCONF_TASK_CLKSTOP_4         = offsetof(NRF_LRCCONF_Type, TASKS_STOPREQCLKSRC[4]),   /**< Stop requesting the clock source for clock 4. */
    NRF_LRCCONF_TASK_CLKSTOP_5         = offsetof(NRF_LRCCONF_Type, TASKS_STOPREQCLKSRC[5]),   /**< Stop requesting the clock source for clock 5. */
    NRF_LRCCONF_TASK_CLKSTOP_6         = offsetof(NRF_LRCCONF_Type, TASKS_STOPREQCLKSRC[6]),   /**< Stop requesting the clock source for clock 6. */
    NRF_LRCCONF_TASK_CLKSTOP_7         = offsetof(NRF_LRCCONF_Type, TASKS_STOPREQCLKSRC[7]),   /**< Stop requesting the clock source for clock 7. */
    NRF_LRCCONF_TASK_CONSTLAT_ENABLE   = offsetof(NRF_LRCCONF_Type, TASKS_CONSTLAT.ENABLE),    /**< Enable constant latency mode. */
    NRF_LRCCONF_TASK_CONSTLAT_DISABLE  = offsetof(NRF_LRCCONF_Type, TASKS_CONSTLAT.DISABLE),   /**< Disable constant latency mode. */
    NRF_LRCCONF_TASK_SYSTEMOFFNOTREADY = offsetof(NRF_LRCCONF_Type, TASKS_SYSTEMOFF.NOTREADY), /**< Indicate being not ready to system off .*/
    NRF_LRCCONF_TASK_SYSTEMOFFREADY    = offsetof(NRF_LRCCONF_Type, TASKS_SYSTEMOFF.READY),    /**< Indicate being ready to system off .*/
#if NRF_LRCCONF_HAS_HFXO
    NRF_LRCCONF_TASK_REQHFXO           = offsetof(NRF_LRCCONF_Type, TASKS_REQHFXO),            /**< Request HFXO. */
    NRF_LRCCONF_TASK_STOPREQHFXO       = offsetof(NRF_LRCCONF_Type, TASKS_STOPREQHFXO),        /**< Stop requesting HFXO. */
#endif
} nrf_lrcconf_task_t;

/** @brief Events. */
typedef enum
{
    NRF_LRCCONF_EVENT_CLKSTARTED_0 = offsetof(NRF_LRCCONF_Type, EVENTS_CLKSRCSTARTED[0]), /**< Clock 0 started. */
    NRF_LRCCONF_EVENT_CLKSTARTED_1 = offsetof(NRF_LRCCONF_Type, EVENTS_CLKSRCSTARTED[1]), /**< Clock 1 started. */
    NRF_LRCCONF_EVENT_CLKSTARTED_2 = offsetof(NRF_LRCCONF_Type, EVENTS_CLKSRCSTARTED[2]), /**< Clock 2 started. */
    NRF_LRCCONF_EVENT_CLKSTARTED_3 = offsetof(NRF_LRCCONF_Type, EVENTS_CLKSRCSTARTED[3]), /**< Clock 3 started. */
    NRF_LRCCONF_EVENT_CLKSTARTED_4 = offsetof(NRF_LRCCONF_Type, EVENTS_CLKSRCSTARTED[4]), /**< Clock 4 started. */
    NRF_LRCCONF_EVENT_CLKSTARTED_5 = offsetof(NRF_LRCCONF_Type, EVENTS_CLKSRCSTARTED[5]), /**< Clock 5 started. */
    NRF_LRCCONF_EVENT_CLKSTARTED_6 = offsetof(NRF_LRCCONF_Type, EVENTS_CLKSRCSTARTED[6]), /**< Clock 6 started. */
    NRF_LRCCONF_EVENT_CLKSTARTED_7 = offsetof(NRF_LRCCONF_Type, EVENTS_CLKSRCSTARTED[7]), /**< Clock 7 started. */
#if NRF_LRCCONF_HAS_HFXO
    NRF_LRCCONF_EVENT_HFXOSTARTED  = offsetof(NRF_LRCCONF_Type, EVENTS_HFXOSTARTED),      /**< HFXO started. */
#endif
} nrf_lrcconf_event_t;

/** @brief Clock sources. */
typedef enum
{
    NRF_LRCCONF_CLK_SRC_OPEN_LOOP   = LRCCONF_CLKSTAT_SRC_SRC_OpenLoop,   /**< Open loop mode. */
    NRF_LRCCONF_CLK_SRC_CLOSED_LOOP = LRCCONF_CLKSTAT_SRC_SRC_ClosedLoop, /**< Closed loop mode. */
} nrf_lrcconf_clk_src_t;

/** @brief Power domain mask. */
typedef enum
{
    NRF_LRCCONF_POWER_MAIN     = LRCCONF_POWERON_MAIN_Msk,    /**< Mask for main power domain. */
    NRF_LRCCONF_POWER_DOMAIN_0 = LRCCONF_POWERON_ACTIVE0_Msk, /**< Mask for power domain 0. */
    NRF_LRCCONF_POWER_DOMAIN_1 = LRCCONF_POWERON_ACTIVE1_Msk, /**< Mask for power domain 1. */
    NRF_LRCCONF_POWER_DOMAIN_2 = LRCCONF_POWERON_ACTIVE2_Msk, /**< Mask for power domain 2. */
    NRF_LRCCONF_POWER_DOMAIN_3 = LRCCONF_POWERON_ACTIVE3_Msk, /**< Mask for power domain 3. */
    NRF_LRCCONF_POWER_DOMAIN_4 = LRCCONF_POWERON_ACTIVE4_Msk, /**< Mask for power domain 4. */
    NRF_LRCCONF_POWER_DOMAIN_5 = LRCCONF_POWERON_ACTIVE5_Msk, /**< Mask for power domain 5. */
    NRF_LRCCONF_POWER_DOMAIN_6 = LRCCONF_POWERON_ACTIVE6_Msk, /**< Mask for power domain 6. */
    NRF_LRCCONF_POWER_DOMAIN_7 = LRCCONF_POWERON_ACTIVE7_Msk, /**< Mask for power domain 7. */
} nrf_lrcconf_power_domain_mask_t;

/**
 * @brief Function for starting a task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Requested task.
 */
NRF_STATIC_INLINE void nrf_lrcconf_task_trigger(NRF_LRCCONF_Type * p_reg, nrf_lrcconf_task_t task);

/**
 * @brief Function for returning the address of a task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Requested task.
 *
 * @return Address of the requested task register.
 */
NRF_STATIC_INLINE uint32_t nrf_lrcconf_task_address_get(NRF_LRCCONF_Type const * p_reg,
                                                        nrf_lrcconf_task_t       task);

/**
 * @brief Function for returning the address of an event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Requested event.
 *
 * @return Address of the requested event register.
 */
NRF_STATIC_INLINE uint32_t nrf_lrcconf_event_address_get(NRF_LRCCONF_Type const * p_reg,
                                                         nrf_lrcconf_event_t      event);

/**
 * @brief Function for clearing an event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be cleared.
 */
NRF_STATIC_INLINE void nrf_lrcconf_event_clear(NRF_LRCCONF_Type *  p_reg,
                                               nrf_lrcconf_event_t event);

/**
 * @brief Function for retrieving the state of the specified event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_lrcconf_event_check(NRF_LRCCONF_Type const * p_reg,
                                               nrf_lrcconf_event_t      event);

/**
 * @brief Function for getting CLKSTART task by its index.
 *
 * @param[in] index Index of the CLKSTART task.
 *
 * @return CLKSTART task.
 */
NRF_STATIC_INLINE nrf_lrcconf_task_t nrf_lrcconf_clkstart_task_get(uint8_t index);

/**
 * @brief Function for getting CLKSTOP task by its index.
 *
 * @param[in] index Index of the CLKSTOP task.
 *
 * @return CLKSTOP task.
 */
NRF_STATIC_INLINE nrf_lrcconf_task_t nrf_lrcconf_clkstop_task_get(uint8_t index);

/**
 * @brief Function for getting CLKSTARTED event by its index.
 *
 * @param[in] index Index of the CLKSTARTED event.
 *
 * @return CLKSTARTED event.
 */
NRF_STATIC_INLINE nrf_lrcconf_event_t nrf_lrcconf_clkstarted_event_get(uint8_t index);

/**
 * @brief Function for getting power domain on mask by its index.
 *
 * @param[in] index Index of the domain.
 *
 * @return Enum value for domain.
 */
NRF_STATIC_INLINE
nrf_lrcconf_power_domain_mask_t nrf_lrcconf_power_domain_on_get(uint8_t index);

/**
 * @brief Function for retrieving the status indicating whether TASK_CLKSTART task has been
 *        triggered.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] clock Clock index.
 *
 * @retval true  Clock start task has been triggered.
 * @retval false Clock start task has not been triggered.
 */
NRF_STATIC_INLINE bool nrf_lrcconf_clock_run_status_check(NRF_LRCCONF_Type const * p_reg,
                                                          uint8_t                  clock);

/**
 * @brief Function for retrieving the source of the specified clock.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] clock Clock index.
 *
 * @return Clock source.
 */
NRF_STATIC_INLINE
nrf_lrcconf_clk_src_t nrf_lrcconf_clock_source_get(NRF_LRCCONF_Type const * p_reg,
                                                   uint8_t                  clock);
/**
 * @brief Function for setting the specified clock to remain running.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] clock     Clock index.
 * @param[in] alwaysrun True if forcing the clock to remain on is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_lrcconf_clock_always_run_force_set(NRF_LRCCONF_Type * p_reg,
                                                              uint8_t            clock,
                                                              bool               alwaysrun);

/**
 * @brief Function for checking if the specified clock is configured to remain running.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] clock Clock index.
 *
 * @retval true  The clock is configured to remain on.
 * @retval false The clock is not configured to remain on.
 */
NRF_STATIC_INLINE bool nrf_lrcconf_clock_always_run_check(NRF_LRCCONF_Type const * p_reg,
                                                          uint8_t                  clock);

/**
 * @brief Function for setting the source of the specified clock.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] clock  Clock index.
 * @param[in] source Clock source to be set.
 * @param[in] bypass True if clock source bypass is to be set, false otherwise.
 *                   Ignored for unsupported devices.
 */
NRF_STATIC_INLINE void nrf_lrcconf_clock_source_set(NRF_LRCCONF_Type *    p_reg,
                                                    uint8_t               clock,
                                                    nrf_lrcconf_clk_src_t source,
                                                    bool                  bypass);

/**
 * @brief Function for checking the status of constant latency.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Constant latency enabled.
 * @retval false Constant latency disabled, low power enabled.
 */
NRF_STATIC_INLINE bool nrf_lrcconf_constlatstat_check(NRF_LRCCONF_Type const * p_reg);

/**
 * @brief Function for setting the power domain to remain on.
 *
 * @param[in] p_reg       Pointer to the structure of registers of the peripheral.
 * @param[in] domain_mask Mask of power domains to remain on.
 * @param[in] alwayson    True if forcing the power domain to remain on is to be enabled,
 *                        false otherwise.
 */
NRF_STATIC_INLINE
void nrf_lrcconf_poweron_force_set(NRF_LRCCONF_Type *              p_reg,
                                   nrf_lrcconf_power_domain_mask_t domain_mask,
                                   bool                            alwayson);

/**
 * @brief Function for checking if the power domain is configured to remain on.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] domain Power domain to be checked.
 *
 * @retval true  The domain is configured to remain on.
 * @retval false The domain is not configured to remain on.
 */
NRF_STATIC_INLINE
bool nrf_lrcconf_poweron_force_check(NRF_LRCCONF_Type const *        p_reg,
                                     nrf_lrcconf_power_domain_mask_t domain);

/**
 * @brief Function for setting the power domain to be retained in System ON idle.
 *
 * @param[in] p_reg       Pointer to the structure of registers of the peripheral.
 * @param[in] domain_mask Mask of power domains to be retained.
 * @param[in] retain      True if power domain retention is to be enabled, flase otherwise.
 */
NRF_STATIC_INLINE
void nrf_lrcconf_retain_set(NRF_LRCCONF_Type *              p_reg,
                            nrf_lrcconf_power_domain_mask_t domain_mask,
                            bool                            retain);

/**
 * @brief Function for checking if the power domain is configured to be retained in System ON idle.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] domain Power domain to be checked.
 *
 * @retval true  The domain is configured to be retained.
 * @retval false The domain is not configured to be retained.
 */
NRF_STATIC_INLINE
bool nrf_lrcconf_retain_check(NRF_LRCCONF_Type const *        p_reg,
                              nrf_lrcconf_power_domain_mask_t domain);

#if NRF_LRCCONF_HAS_AXI_WAITSTATES
/**
 * @brief Function for setting the waitstates for the AXI bridge connection.
 *
 * @param[in] p_reg          Pointer to the structure of registers of the peripheral.
 * @param[in] domain         Functional domain identifier.
 * @param[in] waitstates_num Number of waitstates.
 */
NRF_STATIC_INLINE void nrf_lrcconf_axi_waitstates_set(NRF_LRCCONF_Type * p_reg,
                                                      uint8_t            domain,
                                                      uint8_t            waitstates_num);

/**
 * @brief Function for getting the waitstates for the AXI bridge connection.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] domain Functional domain identifier.
 *
 * @return Number of waitstates
 */
NRF_STATIC_INLINE uint8_t nrf_lrcconf_axi_waitstates_get(NRF_LRCCONF_Type const * p_reg,
                                                         uint8_t                  domain);
#endif

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_lrcconf_task_trigger(NRF_LRCCONF_Type * p_reg, nrf_lrcconf_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_lrcconf_task_address_get(NRF_LRCCONF_Type const * p_reg,
                                                        nrf_lrcconf_task_t       task)
{
    return (uint32_t)p_reg + (uint32_t)task;
}

NRF_STATIC_INLINE uint32_t nrf_lrcconf_event_address_get(NRF_LRCCONF_Type const * p_reg,
                                                         nrf_lrcconf_event_t      event)
{
    return (uint32_t)p_reg + (uint32_t)event;
}

NRF_STATIC_INLINE void nrf_lrcconf_event_clear(NRF_LRCCONF_Type *  p_reg,
                                               nrf_lrcconf_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_lrcconf_event_check(NRF_LRCCONF_Type const * p_reg,
                                               nrf_lrcconf_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE nrf_lrcconf_task_t nrf_lrcconf_clkstart_task_get(uint8_t index)
{
    NRFX_ASSERT(index < NRF_LRCCONF_CLK_COUNT);
    return (nrf_lrcconf_task_t)(NRFX_OFFSETOF(NRF_LRCCONF_Type, TASKS_REQCLKSRC[index]));
}

NRF_STATIC_INLINE nrf_lrcconf_task_t nrf_lrcconf_clkstop_task_get(uint8_t index)
{
    NRFX_ASSERT(index < NRF_LRCCONF_CLK_COUNT);
    return (nrf_lrcconf_task_t)(NRFX_OFFSETOF(NRF_LRCCONF_Type, TASKS_STOPREQCLKSRC[index]));
}

NRF_STATIC_INLINE nrf_lrcconf_event_t nrf_lrcconf_clkstarted_event_get(uint8_t index)
{
    NRFX_ASSERT(index < NRF_LRCCONF_CLK_COUNT);
    return (nrf_lrcconf_event_t)(NRFX_OFFSETOF(NRF_LRCCONF_Type, EVENTS_CLKSRCSTARTED[index]));
}

NRF_STATIC_INLINE
nrf_lrcconf_power_domain_mask_t nrf_lrcconf_power_domain_on_get(uint8_t index)
{
    NRFX_ASSERT(index < 8);
    return (nrf_lrcconf_power_domain_mask_t)(LRCCONF_POWERON_ACTIVE0_Msk << index);
}

NRF_STATIC_INLINE bool nrf_lrcconf_clock_run_status_check(NRF_LRCCONF_Type const * p_reg,
                                                          uint8_t                  clock)
{
    NRFX_ASSERT(clock < NRF_LRCCONF_CLK_COUNT);
    return p_reg->CLKSTAT[clock].RUN & LRCCONF_CLKSTAT_RUN_STATUS_Msk;
}

NRF_STATIC_INLINE
nrf_lrcconf_clk_src_t nrf_lrcconf_clock_source_get(NRF_LRCCONF_Type const * p_reg,
                                                   uint8_t                  clock)
{
    NRFX_ASSERT(clock < NRF_LRCCONF_CLK_COUNT);
    return (nrf_lrcconf_clk_src_t)((p_reg->CLKSTAT[clock].SRC & LRCCONF_CLKSTAT_SRC_SRC_Msk) >>
                                  LRCCONF_CLKSTAT_SRC_SRC_Pos);
}

NRF_STATIC_INLINE void nrf_lrcconf_clock_always_run_force_set(NRF_LRCCONF_Type * p_reg,
                                                              uint8_t            clock,
                                                              bool               alwaysrun)
{
    NRFX_ASSERT(clock < NRF_LRCCONF_CLK_COUNT);
    p_reg->CLKCTRL[clock].ALWAYSRUN = alwaysrun ?
                          LRCCONF_CLKCTRL_ALWAYSRUN_FORCE_AlwaysRun :
                          LRCCONF_CLKCTRL_ALWAYSRUN_FORCE_Automatic;
}

NRF_STATIC_INLINE bool nrf_lrcconf_clock_always_run_check(NRF_LRCCONF_Type const * p_reg,
                                                          uint8_t                  clock)
{
    NRFX_ASSERT(clock < NRF_LRCCONF_CLK_COUNT);
    return p_reg->CLKCTRL[clock].ALWAYSRUN & LRCCONF_CLKCTRL_ALWAYSRUN_FORCE_AlwaysRun;
}

NRF_STATIC_INLINE void nrf_lrcconf_clock_source_set(NRF_LRCCONF_Type *    p_reg,
                                                    uint8_t               clock,
                                                    nrf_lrcconf_clk_src_t source,
                                                    bool                  bypass)
{
    NRFX_ASSERT(clock < NRF_LRCCONF_CLK_COUNT);
    uint32_t clkmsk = LRCCONF_CLKCTRL_SRC_SRC_Msk |
        NRFX_COND_CODE_1(NRF_LRCCONF_HAS_BYPASS, (LRCCONF_CLKCTRL_SRC_BYPASS_Msk), (0));
    p_reg->CLKCTRL[clock].SRC = (p_reg->CLKCTRL[clock].SRC & ~clkmsk) |
#if NRF_LRCCONF_HAS_BYPASS
                          ((bypass ? LRCCONF_CLKCTRL_SRC_BYPASS_Enable :
                                     LRCCONF_CLKCTRL_SRC_BYPASS_Disable)
                                  << LRCCONF_CLKCTRL_SRC_BYPASS_Pos) |
#endif
                          ((source << LRCCONF_CLKCTRL_SRC_SRC_Pos) & LRCCONF_CLKCTRL_SRC_SRC_Msk);
    (void)bypass;
}

NRF_STATIC_INLINE bool nrf_lrcconf_constlatstat_check(NRF_LRCCONF_Type const * p_reg)
{
    return p_reg->CONSTLATSTAT & LRCCONF_CONSTLATSTAT_STATUS_Msk;
}

NRF_STATIC_INLINE
void nrf_lrcconf_poweron_force_set(NRF_LRCCONF_Type *              p_reg,
                                   nrf_lrcconf_power_domain_mask_t domain_mask,
                                   bool                            alwayson)
{
    p_reg->POWERON = ((p_reg->POWERON & ~domain_mask) | (alwayson ? domain_mask : 0));
}

NRF_STATIC_INLINE
bool nrf_lrcconf_poweron_force_check(NRF_LRCCONF_Type const *        p_reg,
                                     nrf_lrcconf_power_domain_mask_t domain)
{
    return p_reg->POWERON & domain;
}

NRF_STATIC_INLINE
void nrf_lrcconf_retain_set(NRF_LRCCONF_Type *              p_reg,
                            nrf_lrcconf_power_domain_mask_t domain_mask,
                            bool                            retain)
{
    p_reg->RETAIN = ((p_reg->RETAIN & ~domain_mask) | (retain ? domain_mask : 0));
}

NRF_STATIC_INLINE
bool nrf_lrcconf_retain_check(NRF_LRCCONF_Type const *        p_reg,
                              nrf_lrcconf_power_domain_mask_t domain)
{
    return p_reg->RETAIN & domain;
}

#if NRF_LRCCONF_HAS_AXI_WAITSTATES
NRF_STATIC_INLINE void nrf_lrcconf_axi_waitstates_set(NRF_LRCCONF_Type * p_reg,
                                                      uint8_t            domain,
                                                      uint8_t            waitstates_num)
{
    NRFX_ASSERT(domain < NRF_LRCCONF_AXI_WAITSTATES_ARRAY_SIZE);
    NRFX_ASSERT(waitstates_num <=
               (LRCCONF_AX2XWAITSTATES_WAITSTATES_Msk >> LRCCONF_AX2XWAITSTATES_WAITSTATES_Pos));
    p_reg->AX2XWAITSTATES[domain] = ((uint32_t)waitstates_num <<
                                    LRCCONF_AX2XWAITSTATES_WAITSTATES_Pos) &
                                    LRCCONF_AX2XWAITSTATES_WAITSTATES_Msk;
}

NRF_STATIC_INLINE uint8_t nrf_lrcconf_axi_waitstates_get(NRF_LRCCONF_Type const * p_reg,
                                                         uint8_t                  domain)
{
    NRFX_ASSERT(domain < NRF_LRCCONF_AXI_WAITSTATES_ARRAY_SIZE);
    return (uint8_t)((p_reg->AX2XWAITSTATES[domain] & LRCCONF_AX2XWAITSTATES_WAITSTATES_Msk) >>
                    LRCCONF_AX2XWAITSTATES_WAITSTATES_Pos);
}
#endif
#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_LRCCONF_H__
