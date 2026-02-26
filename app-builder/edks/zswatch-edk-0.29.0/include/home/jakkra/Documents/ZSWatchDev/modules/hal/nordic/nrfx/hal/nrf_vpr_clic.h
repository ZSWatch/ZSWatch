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

#ifndef NRF_VPR_CLIC_H_
#define NRF_VPR_CLIC_H_

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_vpr_clic_hal VPR CLIC HAL
 * @{
 * @ingroup nrf_vpr
 * @brief   Hardware access layer for managing the VPR RISC-V CPU Interrupt Controller (VPR CLIC).
 */

/** @brief Interrupts count. */
#define NRF_VPR_CLIC_IRQ_COUNT CLIC_CLIC_CLICINT_MaxCount

/** @brief Interrupt privilege modes available. */
typedef enum
{
    NRF_VPR_CLIC_MODE_M   = CLIC_CLIC_CLICCFG_NMBITS_ModeM, /**< All interrupts are M-mode only. */
} nrf_vpr_clic_mode_t;

/** @brief Interrupt priority level. */
typedef enum
{
    NRF_VPR_CLIC_PRIORITY_LEVEL0 = CLIC_CLIC_CLICINT_PRIORITY_PRIOLEVEL0, /**< Priority level 0. */
    NRF_VPR_CLIC_PRIORITY_LEVEL1 = CLIC_CLIC_CLICINT_PRIORITY_PRIOLEVEL1, /**< Priority level 1. */
    NRF_VPR_CLIC_PRIORITY_LEVEL2 = CLIC_CLIC_CLICINT_PRIORITY_PRIOLEVEL2, /**< Priority level 2. */
    NRF_VPR_CLIC_PRIORITY_LEVEL3 = CLIC_CLIC_CLICINT_PRIORITY_PRIOLEVEL3, /**< Priority level 3. */
} nrf_vpr_clic_priority_t;

/** @brief Macro for converting integer priority level to @ref nrf_vpr_clic_priority_t. */
#define NRF_VPR_CLIC_INT_TO_PRIO(x) ((x) == 0 ? NRF_VPR_CLIC_PRIORITY_LEVEL0 : \
                                    ((x) == 1 ? NRF_VPR_CLIC_PRIORITY_LEVEL1 : \
                                    ((x) == 2 ? NRF_VPR_CLIC_PRIORITY_LEVEL2 : \
                                    ((x) == 3 ? NRF_VPR_CLIC_PRIORITY_LEVEL3 : 0))))

/** @brief VPR CLIC configuration structure. */
typedef struct
{
    bool                hw_vectoring;   /**< Selective interrupt hardware vectoring. */
    uint8_t             level_encoding; /**< Interrupt level encoding. */
    nrf_vpr_clic_mode_t privilege_mode; /**< Interrupt privilege mode. */
} nrf_vpr_clic_config_t;

/** @brief VPR CLIC information structure. */
typedef struct
{
    uint16_t interrupt_count; /**< Maximum number of interrupts supported. */
    uint8_t  version;         /**< Version of CLIC. */
    uint8_t  trigger_count;   /**< Number of maximum interrupt triggers supported. */
} nrf_vpr_clic_info_t;

/** @brief Interrupt trigger and polarity types. */
typedef enum
{
    NRF_VPR_CLIC_TRIGGER_EDGE_POS  = CLIC_CLIC_CLICINT_TRIG_EdgeTriggered,  /**< Interrupts are positive edge-triggered. */
} nrf_vpr_clic_trigger_t;

/** @brief Interrupt privilege. */
typedef enum
{
    NRF_VPR_CLIC_PRIV_MACHINE    = CLIC_CLIC_CLICINT_MODE_MachineMode,    /**< Machine mode. */
} nrf_vpr_clic_priv_t;

/** @brief Interrupt attributes structure. */
typedef struct
{
    bool                   hw_vectoring; /**< Selective interrupt hardware vectoring. */
    nrf_vpr_clic_trigger_t trigger;      /**< Trigger type and polarity for the interrupt. */
    nrf_vpr_clic_priv_t    privilege;    /**< Privilege mode. */
} nrf_vpr_clic_attr_t;

/**
 * @brief Function for getting the CLIC configuration.
 *
 * @param[in]  p_reg    Pointer to the structure of registers of the peripheral.
 * @param[out] p_config Pointer to the VPR CLIC configuration structure.
 */
NRF_STATIC_INLINE void nrf_vpr_clic_config_get(NRF_CLIC_Type const *   p_reg,
                                               nrf_vpr_clic_config_t * p_config);

/**
 * @brief Function for getting the CLIC information.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_info Pointer to the VPR CLIC information structure.
 */
NRF_STATIC_INLINE void nrf_vpr_clic_info_get(NRF_CLIC_Type const * p_reg,
                                             nrf_vpr_clic_info_t * p_info);

/**
 * @brief Function for setting the specified interrupt to be pending.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] irq_num Number of interrupt to be triggered.
 */
NRF_STATIC_INLINE void nrf_vpr_clic_int_pending_set(NRF_CLIC_Type * p_reg, uint32_t irq_num);

/**
 * @brief Function for clearing the pending status for the specified interrupt.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] irq_num Number of interrupt to be cleared.
 */
NRF_STATIC_INLINE void nrf_vpr_clic_int_pending_clear(NRF_CLIC_Type * p_reg, uint32_t irq_num);

/**
 * @brief Function for checking if the specified interrupt is pending.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] irq_num Number of interrupt to be checked.
 *
 * @retval true  Interrupt is pending.
 * @retval false Interrupt is not pending.
 */
NRF_STATIC_INLINE bool nrf_vpr_clic_int_pending_check(NRF_CLIC_Type const * p_reg, uint32_t irq_num);

/**
 * @brief Function for enabling or disabling the specified interrupt.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] irq_num Number of interrupt to be enabled or disabled.
 * @param[in] enable  True if interrupt is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_vpr_clic_int_enable_set(NRF_CLIC_Type * p_reg,
                                                   uint32_t        irq_num,
                                                   bool            enable);

/**
 * @brief Function for checking if the specified interrupt is enabled.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] irq_num Number of interrupt to be checked.
 *
 * @retval true  Interrupt is enabled.
 * @retval false Interrupt is disabled.
 */
NRF_STATIC_INLINE bool nrf_vpr_clic_int_enable_check(NRF_CLIC_Type const * p_reg, uint32_t irq_num);

/**
 * @brief Function for setting the priority of the specified interrupt.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] irq_num  Number of interrupt.
 * @param[in] priority Priority to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_clic_int_priority_set(NRF_CLIC_Type *         p_reg,
                                                     uint32_t                irq_num,
                                                     nrf_vpr_clic_priority_t priority);

/**
 * @brief Function for getting the priority of the specified interrupt.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] irq_num Number of interrupt.
 *
 * @return Priority of the specified interrupt.
 */
NRF_STATIC_INLINE
nrf_vpr_clic_priority_t nrf_vpr_clic_int_priority_get(NRF_CLIC_Type const * p_reg,
                                                      uint32_t              irq_num);

/**
 * @brief Function for getting the CLIC attributes.
 *
 * @param[in]  p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in]  irq_num Number of interrupt.
 * @param[out] p_attr  Pointer to the structure to be filled with VPR CLIC attributes.
 */
NRF_STATIC_INLINE void nrf_vpr_clic_int_attr_get(NRF_CLIC_Type const *  p_reg,
                                                 uint32_t               irq_num,
                                                 nrf_vpr_clic_attr_t *  p_attr);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_vpr_clic_config_get(NRF_CLIC_Type const *   p_reg,
                                               nrf_vpr_clic_config_t * p_config)
{
    NRFX_ASSERT(p_config);
    uint32_t cfg = p_reg->CLIC.CLICCFG;

    p_config->hw_vectoring   = (cfg & CLIC_CLIC_CLICCFG_NVBITS_Msk) >> CLIC_CLIC_CLICCFG_NVBITS_Pos;
    p_config->level_encoding = (cfg & CLIC_CLIC_CLICCFG_NLBITS_Msk) >> CLIC_CLIC_CLICCFG_NLBITS_Pos;
    p_config->privilege_mode = (nrf_vpr_clic_mode_t)((cfg & CLIC_CLIC_CLICCFG_NMBITS_Msk)
                                                     >> CLIC_CLIC_CLICCFG_NMBITS_Pos);
}

NRF_STATIC_INLINE void nrf_vpr_clic_info_get(NRF_CLIC_Type const * p_reg,
                                             nrf_vpr_clic_info_t * p_info)
{
    NRFX_ASSERT(p_info);
    uint32_t inf = p_reg->CLIC.CLICINFO;

    p_info->interrupt_count = (inf & CLIC_CLIC_CLICINFO_NUMINTERRUPTS_Msk)
                              >> CLIC_CLIC_CLICINFO_NUMINTERRUPTS_Pos;
    p_info->version         = (inf & CLIC_CLIC_CLICINFO_VERSION_Msk)
                              >> CLIC_CLIC_CLICINFO_VERSION_Pos;
    p_info->trigger_count   = (inf & CLIC_CLIC_CLICINFO_NUMTRIGGER_Msk)
                              >> CLIC_CLIC_CLICINFO_NUMTRIGGER_Pos;
}

NRF_STATIC_INLINE void nrf_vpr_clic_int_pending_set(NRF_CLIC_Type * p_reg, uint32_t irq_num)
{
    NRFX_ASSERT(irq_num < NRF_VPR_CLIC_IRQ_COUNT);
    p_reg->CLIC.CLICINT[irq_num] = (p_reg->CLIC.CLICINT[irq_num] & ~CLIC_CLIC_CLICINT_IP_Msk) |
                                   (CLIC_CLIC_CLICINT_IP_Pending << CLIC_CLIC_CLICINT_IP_Pos);
}

NRF_STATIC_INLINE void nrf_vpr_clic_int_pending_clear(NRF_CLIC_Type * p_reg, uint32_t irq_num)
{
    NRFX_ASSERT(irq_num < NRF_VPR_CLIC_IRQ_COUNT);
    p_reg->CLIC.CLICINT[irq_num] = (p_reg->CLIC.CLICINT[irq_num] & ~CLIC_CLIC_CLICINT_IP_Msk) |
                                   (CLIC_CLIC_CLICINT_IP_NotPending << CLIC_CLIC_CLICINT_IP_Pos);
}

NRF_STATIC_INLINE bool nrf_vpr_clic_int_pending_check(NRF_CLIC_Type const * p_reg, uint32_t irq_num)
{
    NRFX_ASSERT(irq_num < NRF_VPR_CLIC_IRQ_COUNT);

    return ((p_reg->CLIC.CLICINT[irq_num] & CLIC_CLIC_CLICINT_IP_Msk) >> CLIC_CLIC_CLICINT_IP_Pos)
           ==
           CLIC_CLIC_CLICINT_IP_Pending ? true : false;
}

NRF_STATIC_INLINE void nrf_vpr_clic_int_enable_set(NRF_CLIC_Type * p_reg,
                                                   uint32_t        irq_num,
                                                   bool            enable)
{
    NRFX_ASSERT(irq_num < NRF_VPR_CLIC_IRQ_COUNT);
    p_reg->CLIC.CLICINT[irq_num] = (p_reg->CLIC.CLICINT[irq_num] & ~CLIC_CLIC_CLICINT_IE_Msk) |
                                   ((enable ? CLIC_CLIC_CLICINT_IE_Enabled :
                                              CLIC_CLIC_CLICINT_IE_Disabled)
                                    << CLIC_CLIC_CLICINT_IE_Pos);
}

NRF_STATIC_INLINE bool nrf_vpr_clic_int_enable_check(NRF_CLIC_Type const * p_reg, uint32_t irq_num)
{
    NRFX_ASSERT(irq_num < NRF_VPR_CLIC_IRQ_COUNT);

    return ((p_reg->CLIC.CLICINT[irq_num] & CLIC_CLIC_CLICINT_IE_Msk) >> CLIC_CLIC_CLICINT_IE_Pos)
           == CLIC_CLIC_CLICINT_IE_Enabled ? true : false;
}

NRF_STATIC_INLINE void nrf_vpr_clic_int_priority_set(NRF_CLIC_Type *         p_reg,
                                                     uint32_t                irq_num,
                                                     nrf_vpr_clic_priority_t priority)
{
    NRFX_ASSERT(irq_num < NRF_VPR_CLIC_IRQ_COUNT);
    NRFX_ASSERT(priority != 0);

    p_reg->CLIC.CLICINT[irq_num] = (p_reg->CLIC.CLICINT[irq_num] & ~CLIC_CLIC_CLICINT_PRIORITY_Msk)
                                   | (priority << CLIC_CLIC_CLICINT_PRIORITY_Pos);
}

NRF_STATIC_INLINE
nrf_vpr_clic_priority_t nrf_vpr_clic_int_priority_get(NRF_CLIC_Type const * p_reg,
                                                      uint32_t              irq_num)
{
    NRFX_ASSERT(irq_num < NRF_VPR_CLIC_IRQ_COUNT);

    return (nrf_vpr_clic_priority_t)((p_reg->CLIC.CLICINT[irq_num] & CLIC_CLIC_CLICINT_PRIORITY_Msk)
           >> CLIC_CLIC_CLICINT_PRIORITY_Pos);
}

NRF_STATIC_INLINE void nrf_vpr_clic_int_attr_get(NRF_CLIC_Type const *  p_reg,
                                                 uint32_t               irq_num,
                                                 nrf_vpr_clic_attr_t *  p_attr)
{
    NRFX_ASSERT(irq_num < NRF_VPR_CLIC_IRQ_COUNT);
    NRFX_ASSERT(p_attr);
    uint32_t att = p_reg->CLIC.CLICINT[irq_num];

    p_attr->hw_vectoring = (att & CLIC_CLIC_CLICINT_SHV_Msk) >> CLIC_CLIC_CLICINT_SHV_Pos;
    p_attr->trigger      = (nrf_vpr_clic_trigger_t)((att & CLIC_CLIC_CLICINT_TRIG_Msk)
                                                    >> CLIC_CLIC_CLICINT_TRIG_Pos);
    p_attr->privilege    = (nrf_vpr_clic_priv_t)((att & CLIC_CLIC_CLICINT_MODE_Msk)
                                                    >> CLIC_CLIC_CLICINT_MODE_Pos);
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRF_VPR_CLIC_H_ */
