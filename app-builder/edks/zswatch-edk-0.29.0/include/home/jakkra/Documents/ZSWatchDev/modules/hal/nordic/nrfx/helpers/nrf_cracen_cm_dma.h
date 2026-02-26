/*
 * Copyright (c) 2025, Nordic Semiconductor ASA
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

#ifndef NRF_CRACEN_CM_DMA_H__
#define NRF_CRACEN_CM_DMA_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_cracen_cm_dma_hal CRACEN CryptoMaster DMA layer.
 * @{
 * @ingroup nrf_cracen
 * @brief   Helper layer that provides common functionality for the CRACEN Cryptmaster DMA.
 */

/** @brief CRACEN CryptoMaster DMA descriptor. */
typedef struct __PACKED __ALIGN(4) nrf_cracen_cm_dma_desc {
    uint8_t *                       p_addr; /**< Address of the data block. */
    struct nrf_cracen_cm_dma_desc * p_next; /**< Pointer to a possibly chained descriptor. */
    uint32_t                        length; /**< Number of bytes in the data block. */
    uint32_t                        dmatag; /**< User tag (only used for fetch). */
} nrf_cracen_cm_dma_desc_t;

/**
 * @brief This value can be used as a DMA descriptor next address to indicate there is no next
 *        descriptor, and therefore the DMA should stop with current one.
 */
#define NRF_CRACEN_CM_DMA_DESC_STOP ((struct nrf_cracen_cm_dma_desc *)0x1)

/**
 * @brief When this value is OR'ed with a descriptor length field, the DMA will read from/write into
 *        the FIFO extra dummy bytes so the operation will finish at a FIFO word boundary.
 */
#define NRF_CRACEN_CM_DMA_DESC_LENGTH_REALIGN (1UL << 29)

/**
 * @brief When this value is OR'ed into the DMA tag, the data will be routed to the selected crypto
 *        engine configuration interface, otherwise the data is routed to its data interface.
 */
#define NRF_CRACEN_CM_DMA_TAG_CONFIG 0x10UL

/**
 * @brief This can value can be OR'ed into the DMA tag. It will be passed as the "last" side signal
 *        to the selected crypto engine with the data block. It is up to each crypto engine how to
 *        interpret it.
 */
#define NRF_CRACEN_CM_DMA_TAG_LAST 0x20UL

/** @brief Offset of the DataType field in data tags */
#define NRF_CRACEN_CM_DMA_TAG_DATATYPE_OFFSET 6

/** @brief CRACEN CM DMA tag engine selection values. */
typedef enum
{
    NRF_CRACEN_CM_DMA_TAG_ENGINE_AES    = 0x1, ///< Select the AES engine.
    NRF_CRACEN_CM_DMA_TAG_ENGINE_HASH   = 0x3, ///< Select the Hash engine.
    NRF_CRACEN_CM_DMA_TAG_ENGINE_BYPASS = 0xF, ///< Select the engine bypass.
} nrf_cracen_cm_dma_tag_engine_t;

/** @brief CRACEN CM DMA tag data type selection values. */
typedef enum
{
    NRF_CRACEN_CM_DMA_TAG_DATATYPE_AES_PAYLOAD        = (0 << NRF_CRACEN_CM_DMA_TAG_DATATYPE_OFFSET), ///< (for AES engine) Payload (encrypted).
    NRF_CRACEN_CM_DMA_TAG_DATATYPE_AES_HEADER         = (1 << NRF_CRACEN_CM_DMA_TAG_DATATYPE_OFFSET), ///< (for AES engine) Header (not encrypted, but authenticated).
    NRF_CRACEN_CM_DMA_TAG_DATATYPE_HASH_MESSAGE       = (0 << NRF_CRACEN_CM_DMA_TAG_DATATYPE_OFFSET), ///< (for Hash engine) Message.
    NRF_CRACEN_CM_DMA_TAG_DATATYPE_HASH_INIT_DATA     = (1 << NRF_CRACEN_CM_DMA_TAG_DATATYPE_OFFSET), ///< (for Hash engine) Initialization data.
    NRF_CRACEN_CM_DMA_TAG_DATATYPE_HASH_HMAC_KEY      = (2 << NRF_CRACEN_CM_DMA_TAG_DATATYPE_OFFSET), ///< (for Hash engine) HMAC Key.
    NRF_CRACEN_CM_DMA_TAG_DATATYPE_HASH_HMAC_REF_HASH = (3 << NRF_CRACEN_CM_DMA_TAG_DATATYPE_OFFSET), ///< (for Hash engine) Reference hash.
} nrf_cracen_cm_dma_tag_datatype_t;

/**
 * @brief Generate a tag targeting a configuration register in the the AES crypto engine.
 *
 * @param[in] reg_off Register offset, one of @ref nrf_cracen_cm_aes_reg_offset_t.
 */
#define NRF_CRACEN_CM_DMA_TAG_AES_CONFIG(reg_off) \
	(NRF_CRACEN_CM_DMA_TAG_ENGINE_AES | NRF_CRACEN_CM_DMA_TAG_CONFIG | (reg_off << 8))

/** @brief CRACEN CM AES crypto engine configuration interface registers offsets. */
typedef enum
{
    NRF_CRACEN_CM_AES_REG_OFFSET_CONFIG = 0x0,  ///< Config register.
    NRF_CRACEN_CM_AES_REG_OFFSET_KEY    = 0x8,  ///< AES key (up to 32 bytes).
    NRF_CRACEN_CM_AES_REG_OFFSET_IV     = 0x28, ///< AES initialization vector (16 bytes).
    NRF_CRACEN_CM_AES_REG_OFFSET_IV2    = 0x38, ///< AES initialization vector, used for context switching (16 bytes).
    NRF_CRACEN_CM_AES_REG_OFFSET_KEY2   = 0x48, ///< AES tweak key (for XTS only) (up to 32 bytes).
    NRF_CRACEN_CM_AES_REG_OFFSET_MASK   = 0x68, ///< Initial value for LFSR (used for countermeasure).
} nrf_cracen_cm_aes_reg_offset_t;

/** @brief CRACEN CM AES crypto engine configuration modes of operation. */
typedef enum
{
    NRF_CRACEN_CM_AES_CONFIG_MODE_ECB      = 0x001,  ///< ECB mode.
    NRF_CRACEN_CM_AES_CONFIG_MODE_CBC      = 0x002,  ///< CBC mode.
    NRF_CRACEN_CM_AES_CONFIG_MODE_CTR      = 0x004,  ///< CTR mode.
    NRF_CRACEN_CM_AES_CONFIG_MODE_CFB      = 0x008,  ///< CFB mode.
    NRF_CRACEN_CM_AES_CONFIG_MODE_OFB      = 0x010,  ///< OFB mode.
    NRF_CRACEN_CM_AES_CONFIG_MODE_CCM      = 0x020,  ///< CCM mode.
    NRF_CRACEN_CM_AES_CONFIG_MODE_GMC_GMAC = 0x040,  ///< GMC/GMAC mode.
    NRF_CRACEN_CM_AES_CONFIG_MODE_XTS      = 0x080,  ///< XTS mode.
    NRF_CRACEN_CM_AES_CONFIG_MODE_CMAC     = 0x100,  ///< CMAC mode.
} nrf_cracen_cm_aes_config_mode_t;

/** @brief CRACEN CM AES crypto engine configuration key selection. */
typedef enum
{
    NRF_CRACEN_CM_AES_CONFIG_KEY_SW_PROGRAMMED = 0x0,  ///< Use key programmed by CPU (normal mode).
    NRF_CRACEN_CM_AES_CONFIG_KEY_AESKEY        = 0x1,  ///< Use special HW input key AESKey[255:0].
} nrf_cracen_cm_aes_config_key_t;

/**
 * @brief Generate the value to be written in the AES crypto engine configuration register given its
 *        parameters.
 *
 * @param[in] mode_of_operation AES mode of operation, one of @ref nrf_cracen_cm_aes_config_mode_t.
 * @param[in] key_sel           SW or HW key selection, one of @ref nrf_cracen_cm_aes_config_key_t.
 * @param[in] context_save      True if operation in AES mode is not final and the engine will return the context.
 * @param[in] context_load      True if operation in AES mode is not initial and the context is provided as input.
 * @param[in] decrypt           True if decryption is to be executed, false if encryption.
 *
 * @note This configuration register can only be written using the cryptomaster DMA engine.
 */
#define NRF_CRACEN_CM_AES_CONFIG(mode_of_operation, key_sel, context_save, context_load, decrypt) \
    (  (((key_sel >> 2) & 0x7) << 28)     \
     | ((mode_of_operation & 0x1FF) << 8) \
     | ((key_sel & 0x3) << 6)             \
     | ((context_save ? 1 : 0) << 5 )     \
     | ((context_load ? 1 : 0) << 4 )     \
     | (decrypt ? 1 : 0)                  \
    )

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_CRACEN_CM_DMA_H__
