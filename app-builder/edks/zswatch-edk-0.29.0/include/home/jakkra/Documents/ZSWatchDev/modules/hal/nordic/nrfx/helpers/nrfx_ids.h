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

#ifndef NRFX_IDS_H__
#define NRFX_IDS_H__

#include <nrfx.h>

#if defined(DOMAIN_NET) || defined(DOMAIN_MODEM)
    #include <nrfx_ipc.h>
#elif defined(DOMAIN_FLPR)
    #include <nrfx_vevif.h>
    #include <nrfx_bellboard.h>
    #include <haly/nrfy_vpr.h>
#else
    #error "No inter-domain signalling supported."
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_ids Generic inter-domain signalling layer.
 * @{
 * @ingroup nrfx
 * @ingroup nrf_ipc
 * @ingroup nrf_vevif
 * @ingroup nrf_bellboard
 *
 * @brief Helper layer that provides the common functionality for the inter-domain signalling (IDS) mechanisms.
 */

/**
 * @brief IDS event handler callback.
 *
 * @param[in] event_idx IDS event index.
 * @param[in] p_context User context.
 */
typedef void (*nrfx_ids_event_handler_t)(uint8_t event_idx, void * p_context);

/** @brief Structure for the IDS instance. */
typedef struct
{
    uint8_t drv_inst_idx; ///< Index of the instance. For internal use only.
    uint8_t int_idx;      ///< Interrupt index. For internal use only.
} nrfx_ids_t;

#ifndef __NRFX_DOXYGEN__
enum {
#if defined(DOMAIN_NET) || defined(DOMAIN_MODEM)
#if NRFX_CHECK(NRFX_IPC_ENABLED)
    NRFX_IDS0_INST_IDX,
#endif
#elif defined(DOMAIN_FLPR)
#if defined(ISA_RISCV)
#if NRFX_CHECK(NRFX_VEVIF_ENABLED)
    NRFX_IDS0_INST_IDX,
#endif
#elif defined(ISA_ARM)
#if NRFX_CHECK(NRFX_BELLBOARD0_ENABLED)
    NRFX_IDS0_INST_IDX,
#endif
#if NRFX_CHECK(NRFX_BELLBOARD1_ENABLED)
    NRFX_IDS1_INST_IDX,
#endif
#if NRFX_CHECK(NRFX_BELLBOARD2_ENABLED)
    NRFX_IDS2_INST_IDX,
#endif
#if NRFX_CHECK(NRFX_BELLBOARD3_ENABLED)
    NRFX_IDS3_INST_IDX,
#endif
#endif /* DOMAIN_FLPR */
#endif
    NRFX_IDS_ENABLED_COUNT
};
#endif /* __NRFX_DOXYGEN__ */

/** @brief Macro for creating a IDS instance. */
#define NRFX_IDS_INSTANCE(id)                               \
{                                                           \
    .drv_inst_idx = NRFX_CONCAT_3(NRFX_IDS, id, _INST_IDX), \
    .int_idx      = id,                                     \
}

/** @brief Macro for creating channel bitmask associated with specified channel index. */
#define NRFX_IDS_CHANNEL(channel) (0x1UL << (channel))

#if defined(DOMAIN_FLPR) || defined(__NRFX_DOXYGEN__)
/** @brief IDS domains. */
typedef enum
{
    NRFX_IDS_DOMAIN_SEC  = 1,                         ///< Reserved. */
    NRFX_IDS_DOMAIN_APP  = NRF_PROCESSOR_APPLICATION, ///< Application domain. */
#if defined(NRF_RADIOCORE_BELLBOARD)
    NRFX_IDS_DOMAIN_NET  = NRF_PROCESSOR_RADIOCORE,   ///< Network domain. */
#endif
    NRFX_IDS_DOMAIN_SYSC = 12,                        ///< Reserved. */
    NRFX_IDS_DOMAIN_PPR  = NRF_PROCESSOR_PPR,         ///< Peripheral Processor */
    NRFX_IDS_DOMAIN_FLPR = NRF_PROCESSOR_FLPR,        ///< Fast Lightweight Processor */
#if defined(NRFX_IDS_DOMAIN_ENUM_EXT)
    NRFX_IDS_DOMAIN_ENUM_EXT
#endif
} nrfx_ids_domain_t;
#elif defined(DOMAIN_NET)
typedef enum
{
    NRFX_IDS_DOMAIN_APP, ///< Application domain. */
    NRFX_IDS_DOMAIN_NET, ///< Network domain. */
} nrfx_ids_domain_t;
#elif defined(DOMAIN_MODEM)
typedef enum
{
    NRFX_IDS_DOMAIN_APP,   ///< Application domain. */
    NRFX_IDS_DOMAIN_MODEM, ///< LTE modem domain. */
} nrfx_ids_domain_t;

#endif

/** @brief Symbol specifying maximum number of available events triggered. */
#if defined(DOMAIN_NET) || defined(DOMAIN_MODEM)
#define NRFX_IDS_EVENTS_TRIGGERED_COUNT IPC_CONF_NUM
#elif defined(DOMAIN_FLPR)
#if defined(ISA_ARM)
#define NRFX_IDS_EVENTS_TRIGGERED_COUNT NRF_BELLBOARD_EVENTS_TRIGGERED_COUNT
#else /* ISA_RISCV */
#define NRFX_IDS_EVENTS_TRIGGERED_COUNT NRF_VPR_EVENTS_TRIGGERED_COUNT
#endif
#endif

/**
 * @brief Function for initializing the IDS instance.
 *
 * @param[in] p_instance         Pointer to IDS instance.
 * @param[in] interrupt_priority Interrupt priority.
 * @param[in] event_handler      Function to be called on interrupt.
 * @param[in] p_context          Context passed to the event handler.
 * @param[in] p_config           Pointer to the structure containing peripheral-specific configuration. Can be NULL.
 *
 * @retval 0         Driver successfully initialized.
 * @retval -EALREADY Driver already initialized.
 */
__STATIC_INLINE int nrfx_ids_init(nrfx_ids_t const *       p_instance,
                                  uint8_t                  interrupt_priority,
                                  nrfx_ids_event_handler_t event_handler,
                                  void *                   p_context,
                                  void const *             p_config)
{
#if defined(DOMAIN_NET) || defined(DOMAIN_MODEM)
    (void)p_instance;
    int err_code = nrfx_ipc_init(interrupt_priority,
                                 (nrfx_ipc_handler_t)event_handler,
                                 p_context);
    if (err_code == 0)
    {
        nrfx_ipc_config_load((nrfx_ipc_config_t const *)p_config);
    }
    return err_code;
#elif defined(DOMAIN_FLPR)
#if defined(ISA_ARM)
    (void)p_config;
    return nrfx_bellboard_init((nrfx_bellboard_t const *)p_instance,
                               interrupt_priority,
                               (nrfx_bellboard_event_handler_t)event_handler,
                               p_context);
#else /* ISA_RISCV */
    (void)p_instance;
    (void)p_config;
    return nrfx_vevif_init((nrf_vpr_clic_priority_t)interrupt_priority,
                           (nrfx_vevif_event_handler_t)event_handler,
                           p_context);
#endif
#endif /* DOMAIN_FLPR */
}

/**
 * @brief Function for uninitializing the IDS instance.
 *
 * @param[in] p_instance Pointer to IDS instance.
 */
__STATIC_INLINE void nrfx_ids_uninit(nrfx_ids_t const * p_instance)
{
#if defined(DOMAIN_NET) || defined(DOMAIN_MODEM)
    (void)p_instance;
    nrfx_ipc_uninit();
#elif defined(DOMAIN_FLPR)
#if defined(ISA_ARM)
    nrfx_bellboard_uninit((nrfx_bellboard_t const *)p_instance);
#else /* ISA_RISCV */
    (void)p_instance;
    nrfx_vevif_uninit();
#endif
#endif /* DOMAIN_FLPR */
}

/**
 * @brief Function for enabling specified interrupts in the IDS instance.
 *
 * @param[in] p_instance Pointer to IDS instance.
 * @param[in] mask       Mask of interrupts to be enabled.
 */
__STATIC_INLINE void nrfx_ids_int_enable(nrfx_ids_t const * p_instance, uint32_t mask)
{
#if defined(DOMAIN_NET) || defined(DOMAIN_MODEM)
    (void)p_instance;
    nrfx_ipc_receive_event_group_enable(mask);
#elif defined(DOMAIN_FLPR)
#if defined(ISA_ARM)
    nrfx_bellboard_int_enable((nrfx_bellboard_t const *)p_instance, mask);
#else /* ISA_RISCV */
    (void)p_instance;
    nrfx_vevif_int_enable(mask);
#endif
#endif /* DOMAIN_FLPR */
}

/**
 * @brief Function for disabling interrupt in the IDS instance.
 *
 * @param[in] p_instance Pointer to IDS instance.
 * @param[in] mask       Mask of interrupts to be disabled.
 */
__STATIC_INLINE void nrfx_ids_int_disable(nrfx_ids_t const * p_instance, uint32_t mask)
{
#if defined(DOMAIN_NET) || defined(DOMAIN_MODEM)
    (void)p_instance;
    nrfx_ipc_receive_event_group_disable(mask);
#elif defined(DOMAIN_FLPR)
#if defined(ISA_ARM)
    nrfx_bellboard_int_disable((nrfx_bellboard_t const *)p_instance, mask);
#else /* ISA_RISCV */
    (void)p_instance;
    nrfx_vevif_int_disable(mask);
#endif  /* DOMAIN_FLPR */
#endif
}

/**
 * @brief Function for conveying the inter-domain signal to the specified domain.
 *
 * @param[in] p_instance Pointer to IDS instance.
 * @param[in] domain     Domain to be signalled. May be NULL for peripherals that have only one connection.
 * @param[in] channel    Inter-domain channel for conveying the signal.
 */
__STATIC_INLINE void nrfx_ids_signal(nrfx_ids_t *      p_instance,
                                     nrfx_ids_domain_t domain,
                                     uint8_t           channel)
{
    NRFX_ASSERT(channel < NRFX_IDS_EVENTS_TRIGGERED_COUNT);
#if defined(DOMAIN_NET) || defined(DOMAIN_MODEM)
    (void)domain;
    (void)p_instance;
    nrfx_ipc_signal(channel);
#elif defined(DOMAIN_FLPR)
    (void)p_instance;
    NRF_BELLBOARD_Type * p_bell = NULL;
    NRF_VPR_Type       * p_vpr  = NULL;
    switch (domain)
    {
        case NRFX_IDS_DOMAIN_APP:
            p_bell = NRF_APPLICATION_BELLBOARD;
            break;
#if defined(NRF_RADIOCORE_BELLBOARD)
        case NRFX_IDS_DOMAIN_NET:
            p_bell = NRF_RADIOCORE_BELLBOARD;
            break;
#endif
        case NRFX_IDS_DOMAIN_SEC:
            p_bell = (NRF_BELLBOARD_Type *)NRF_SECDOMBELLBOARD;
            break;

        case NRFX_IDS_DOMAIN_SYSC:
            p_vpr = (NRF_VPR_Type *)NRF_VPR120;
            break;

        case NRFX_IDS_DOMAIN_FLPR:
            p_vpr = NRF_VPR121;
            break;

        case NRFX_IDS_DOMAIN_PPR:
            p_vpr = NRF_VPR130;
            break;

#if defined(NRFX_IDS_DOMAIN_EXT)
        NRFX_IDS_DOMAIN_EXT
#endif

        default:
            NRFX_ASSERT(0);
            break;
    }

    if (p_bell)
    {
        nrfy_bellboard_task_trigger(p_bell, nrf_bellboard_trigger_task_get(channel));
    }
    else
    {
        nrfy_vpr_task_trigger(p_vpr, nrfy_vpr_trigger_task_get(channel));
    }
#endif
}

/** @} */

#if defined(DOMAIN_NET) || defined(DOMAIN_MODEM)
#if NRFX_CHECK(NRFX_IPC_ENABLED)
#define nrfx_ids_0_irq_handler nrfx_ipc_irq_handler
#endif
#elif defined(DOMAIN_FLPR)
#if defined(ISA_RISCV)
#if NRFX_CHECK(NRFX_VEVIF_ENABLED)
#define nrfx_ids_0_irq_handler nrfx_vevif_0_irq_handler
#define nrfx_ids_1_irq_handler nrfx_vevif_1_irq_handler
#define nrfx_ids_2_irq_handler nrfx_vevif_2_irq_handler
#define nrfx_ids_3_irq_handler nrfx_vevif_3_irq_handler
#define nrfx_ids_4_irq_handler nrfx_vevif_4_irq_handler
#define nrfx_ids_5_irq_handler nrfx_vevif_5_irq_handler
#define nrfx_ids_6_irq_handler nrfx_vevif_6_irq_handler
#define nrfx_ids_7_irq_handler nrfx_vevif_7_irq_handler
#define nrfx_ids_8_irq_handler nrfx_vevif_8_irq_handler
#define nrfx_ids_9_irq_handler nrfx_vevif_9_irq_handler
#define nrfx_ids_10_irq_handler nrfx_vevif_10_irq_handler
#define nrfx_ids_11_irq_handler nrfx_vevif_11_irq_handler
#define nrfx_ids_12_irq_handler nrfx_vevif_12_irq_handler
#define nrfx_ids_13_irq_handler nrfx_vevif_13_irq_handler
#define nrfx_ids_14_irq_handler nrfx_vevif_14_irq_handler
#define nrfx_ids_15_irq_handler nrfx_vevif_15_irq_handler
#define nrfx_ids_16_irq_handler nrfx_vevif_16_irq_handler
#define nrfx_ids_17_irq_handler nrfx_vevif_17_irq_handler
#define nrfx_ids_18_irq_handler nrfx_vevif_18_irq_handler
#define nrfx_ids_19_irq_handler nrfx_vevif_19_irq_handler
#define nrfx_ids_20_irq_handler nrfx_vevif_20_irq_handler
#define nrfx_ids_21_irq_handler nrfx_vevif_21_irq_handler
#define nrfx_ids_22_irq_handler nrfx_vevif_22_irq_handler
#define nrfx_ids_23_irq_handler nrfx_vevif_23_irq_handler
#define nrfx_ids_24_irq_handler nrfx_vevif_24_irq_handler
#define nrfx_ids_25_irq_handler nrfx_vevif_25_irq_handler
#define nrfx_ids_26_irq_handler nrfx_vevif_26_irq_handler
#define nrfx_ids_27_irq_handler nrfx_vevif_27_irq_handler
#define nrfx_ids_28_irq_handler nrfx_vevif_28_irq_handler
#define nrfx_ids_29_irq_handler nrfx_vevif_29_irq_handler
#define nrfx_ids_30_irq_handler nrfx_vevif_30_irq_handler
#define nrfx_ids_31_irq_handler nrfx_vevif_31_irq_handler
#endif
#elif defined(ISA_ARM)
#if NRFX_CHECK(NRFX_BELLBOARD0_ENABLED)
#define nrfx_ids_0_irq_handler nrfx_bellboard_0_irq_handler
#endif
#if NRFX_CHECK(NRFX_BELLBOARD1_ENABLED)
#define nrfx_ids_1_irq_handler nrfx_bellboard_1_irq_handler
#endif
#if NRFX_CHECK(NRFX_BELLBOARD2_ENABLED)
#define nrfx_ids_2_irq_handler nrfx_bellboard_2_irq_handler
#endif
#if NRFX_CHECK(NRFX_BELLBOARD3_ENABLED)
#define nrfx_ids_3_irq_handler nrfx_bellboard_3_irq_handler
#endif
#endif
#endif /* DOMAIN_FLPR */

#ifdef __cplusplus
}
#endif

#endif // NRFX_IDS_H__
