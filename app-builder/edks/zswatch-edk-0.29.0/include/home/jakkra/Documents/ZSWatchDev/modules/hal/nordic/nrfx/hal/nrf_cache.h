/*
 * Copyright (c) 2019 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_CACHE_H__
#define NRF_CACHE_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_cache_hal CACHE HAL
 * @{
 * @ingroup nrf_cache
 * @brief   The hardware access layer for managing the CACHE peripheral.
 */

#if defined(NRF_CACHEDATA) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether CACHEDATA structure is present. */
#define NRF_CACHE_HAS_CACHEDATA 1
#else
#define NRF_CACHE_HAS_CACHEDATA 0
#endif

#if defined(NRF_DCACHEDATA) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether DCACHEDATA structure is present. */
#define NRF_CACHE_HAS_DCACHEDATA 1
#else
#define NRF_CACHE_HAS_DCACHEDATA 0
#endif

#if defined(NRF_ICACHEDATA) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether ICACHEDATA structure is present. */
#define NRF_CACHE_HAS_ICACHEDATA 1
#else
#define NRF_CACHE_HAS_ICACHEDATA 0
#endif

#if NRF_CACHE_HAS_CACHEDATA || NRF_CACHE_HAS_DCACHEDATA || NRF_CACHE_HAS_ICACHEDATA
/** @brief Symbol indicating whether any CACHEDATA structure is present. */
#define NRF_CACHE_HAS_ANY_CACHEDATA 1
#else
#define NRF_CACHE_HAS_ANY_CACHEDATA 0
#endif

#if defined(CACHEDATA_SET_WAY_DU_DATA_Data_Msk) || defined(DCACHEDATA_SET_WAY_DU_DATA_Data_Msk) || \
    defined(ICACHEDATA_SET_WAY_DU_DATA_Data_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether CACHEDATA data units field is present. */
#define NRF_CACHE_HAS_CACHEDATA_DU 1
#else
#define NRF_CACHE_HAS_CACHEDATA_DU 0
#endif

#if defined(NRF_CACHEINFO) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether CACHEINFO structure is present. */
#define NRF_CACHE_HAS_CACHEINFO 1
#else
#define NRF_CACHE_HAS_CACHEINFO 0
#endif

#if defined(NRF_DCACHEINFO) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether DCACHEINFO structure is present. */
#define NRF_CACHE_HAS_DCACHEINFO 1
#else
#define NRF_CACHE_HAS_DCACHEINFO 0
#endif

#if defined(NRF_ICACHEINFO) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether ICACHEINFO structure is present. */
#define NRF_CACHE_HAS_ICACHEINFO 1
#else
#define NRF_CACHE_HAS_ICACHEINFO 0
#endif

#if NRF_CACHE_HAS_CACHEINFO || NRF_CACHE_HAS_DCACHEINFO || NRF_CACHE_HAS_ICACHEINFO
/** @brief Symbol indicating whether any CACHEINFO structure is present. */
#define NRF_CACHE_HAS_ANY_CACHEINFO 1
#else
#define NRF_CACHE_HAS_ANY_CACHEINFO 0
#endif

#if defined(CACHEINFO_SET_WAY_D0_Msk) || defined(DCACHEINFO_SET_WAY_INFO_D0_Msk) || \
    defined(ICACHEINFO_SET_WAY_INFO_D0_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether dirtiness check functionality for cache is supported. */
#define NRF_CACHE_HAS_CACHEINFO_DU_DIRTY 1
#else
#define NRF_CACHE_HAS_CACHEINFO_DU_DIRTY 0
#endif

#if defined(CACHEINFO_SET_WAY_DUV0_Msk) || defined(CACHEINFO_SET_WAY_INFO_DUV0_Msk) || \
    defined(DCACHEINFO_SET_WAY_INFO_DUV0_Msk) || defined(ICACHEINFO_SET_WAY_INFO_DUV0_Msk) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether data unit validation functionality for cache is supported. */
#define NRF_CACHE_HAS_CACHEINFO_DU_VALIDATION 1
#else
#define NRF_CACHE_HAS_CACHEINFO_DU_VALIDATION 0
#endif

#if defined(CACHEINFO_SET_WAY_INFO_DUV0_Msk) || defined(DCACHEINFO_SET_WAY_INFO_D0_Msk) || \
    defined(ICACHEINFO_SET_WAY_INFO_D0_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether cache info has INFO register. */
#define NRF_CACHE_HAS_CACHEINFO_SET_WAY_INFO 1
#else
#define NRF_CACHE_HAS_CACHEINFO_SET_WAY_INFO 0
#endif

#if defined(CACHE_TASKS_INVALIDATECACHE_TASKS_INVALIDATECACHE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether cache tasks are supported. */
#define NRF_CACHE_HAS_TASKS 1
#else
#define NRF_CACHE_HAS_TASKS 0
#endif

#if defined(CACHE_TASKS_CLEANCACHE_TASKS_CLEANCACHE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the CLEAN cache/line tasks are supported. */
#define NRF_CACHE_HAS_TASK_CLEAN 1
#else
#define NRF_CACHE_HAS_TASK_CLEAN 0
#endif

#if defined(CACHE_TASKS_ERASE_TASKS_ERASE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the cache ERASE task is supported. */
#define NRF_CACHE_HAS_TASK_ERASE 1
#else
#define NRF_CACHE_HAS_TASK_ERASE 0
#endif

#if NRF_CACHE_HAS_TASK_ERASE || defined(CACHE_ERASE_ERASE_Erase) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the cache ERASE is supported. */
#define NRF_CACHE_HAS_ERASE 1
#else
#define NRF_CACHE_HAS_ERASE 0
#endif

#if defined(CACHE_TASKS_INVALIDATELINE_TASKS_INVALIDATELINE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the cache INVALIDATELINE task is supported. */
#define NRF_CACHE_HAS_TASK_INVALIDATELINE 1
#else
#define NRF_CACHE_HAS_TASK_INVALIDATELINE 0
#endif

#if defined(CACHE_LINEADDR_ADDR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the cache LINEADDR task is supported. */
#define NRF_CACHE_HAS_LINEADDR 1
#else
#define NRF_CACHE_HAS_LINEADDR 0
#endif

#if defined(CACHE_WRITELOCK_WRITELOCK_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the cache WRITELOCK task is supported. */
#define NRF_CACHE_HAS_WRITELOCK 1
#else
#define NRF_CACHE_HAS_WRITELOCK 0
#endif

#if defined(CACHE_TASKS_FLUSHCACHE_TASKS_FLUSHCACHE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the FLUSH cache/line tasks are supported. */
#define NRF_CACHE_HAS_TASK_FLUSH 1
#else
#define NRF_CACHE_HAS_TASK_FLUSH 0
#endif

#if defined(CACHE_TASKS_SAVE_TASKS_SAVE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether save and restore tasks are supported. */
#define NRF_CACHE_HAS_TASK_SAVE_RESTORE 1
#else
#define NRF_CACHE_HAS_TASK_SAVE_RESTORE 0
#endif

#if defined(CACHE_STATUS_READY_Msk) || defined(CACHE_STATUS_BUSY_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether status check is supported. */
#define NRF_CACHE_HAS_STATUS 1
#else
#define NRF_CACHE_HAS_STATUS 0
#endif

#if defined(CACHE_MODE_MODE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether splitting the dedicated RAM between cache and generic memory is supported. */
#define NRF_CACHE_HAS_RAM_MODE 1
#else
#define NRF_CACHE_HAS_RAM_MODE 0
#endif

#if defined(CACHE_MODE_RAMSIZE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether configuration of split of dedicated RAM between cache and generic memory is supported. */
#define NRF_CACHE_HAS_RAMSIZE 1
#else
#define NRF_CACHE_HAS_RAMSIZE 0
#endif

#if defined(CACHE_DEBUGLOCK_DEBUGLOCK_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether cache DEBUGLOCK is supported. */
#define NRF_CACHE_HAS_DEBUGLOCK 1
#else
#define NRF_CACHE_HAS_DEBUGLOCK 0
#endif

#if NRF_CACHE_HAS_CACHEDATA
/** @brief Max number of words in CACHEDATA data units. */
#define NRF_CACHEDATA_DATA_WORDS_IN_UNIT_MAX CACHEDATA_SET_WAY_DU_DATA_MaxCount
/** @brief Max number of CACHEDATA data units. */
#define NRF_CACHEDATA_DATA_UNITS_MAX CACHEDATA_SET_WAY_DU_MaxCount
/** @brief Max number of CACHEDATA words. */
#define NRF_CACHEDATA_WORD_INDEX_MAX \
        (NRF_CACHEDATA_DATA_WORDS_IN_UNIT_MAX * NRF_CACHEDATA_DATA_UNITS_MAX)
/** @brief Max number of CACHEDATA ways. */
#define NRF_CACHEDATA_WAY_INDEX_MAX CACHEDATA_SET_WAY_MaxCount
/** @brief Max number of CACHEDATA sets. */
#define NRF_CACHEDATA_SET_INDEX_MAX CACHEDATA_SET_MaxCount
#endif

#if NRF_CACHE_HAS_DCACHEDATA
/** @brief Max number of words in DCACHEDATA data units. */
#define NRF_DCACHEDATA_DATA_WORDS_IN_UNIT_MAX DCACHEDATA_SET_WAY_DU_DATA_MaxCount
/** @brief Max number of DCACHEDATA data units. */
#define NRF_DCACHEDATA_DATA_UNITS_MAX DCACHEDATA_SET_WAY_DU_MaxCount
/** @brief Max number of DCACHEDATA words. */
#define NRF_DCACHEDATA_WORD_INDEX_MAX \
        (NRF_DCACHEDATA_DATA_WORDS_IN_UNIT_MAX * NRF_DCACHEDATA_DATA_UNITS_MAX)
/** @brief Max number of DCACHEDATA ways. */
#define NRF_DCACHEDATA_WAY_INDEX_MAX DCACHEDATA_SET_WAY_MaxCount
/** @brief Max number of DCACHEDATA sets. */
#define NRF_DCACHEDATA_SET_INDEX_MAX DCACHEDATA_SET_MaxCount
#endif

#if NRF_CACHE_HAS_ICACHEDATA
#if defined(ICACHEDATA_SET_WAY_DU_DATA_MaxCount)
/** @brief Max number of words in ICACHEDATA data units. */
#define NRF_ICACHEDATA_DATA_WORDS_IN_UNIT_MAX ICACHEDATA_SET_WAY_DU_DATA_MaxCount
/** @brief Max number of ICACHEDATA data units. */
#define NRF_ICACHEDATA_DATA_UNITS_MAX ICACHEDATA_SET_WAY_DU_MaxCount
/** @brief Max number of ICACHEDATA words. */
#define NRF_ICACHEDATA_WORD_INDEX_MAX \
        (NRF_ICACHEDATA_DATA_WORDS_IN_UNIT_MAX * NRF_ICACHEDATA_DATA_UNITS_MAX)
/** @brief Max number of ICACHEDATA ways. */
#define NRF_ICACHEDATA_WAY_INDEX_MAX ICACHEDATA_SET_WAY_MaxCount
/** @brief Max number of ICACHEDATA sets. */
#define NRF_ICACHEDATA_SET_INDEX_MAX ICACHEDATA_SET_MaxCount
#else
/** @brief Max number of words in ICACHEDATA data units. */
#define NRF_ICACHEDATA_DATA_WORDS_IN_UNIT_MAX CACHEDATA_SET_WAY_DU_DATA_MaxCount
/** @brief Max number of ICACHEDATA data units. */
#define NRF_ICACHEDATA_DATA_UNITS_MAX CACHEDATA_SET_WAY_DU_MaxCount
/** @brief Max number of ICACHEDATA words. */
#define NRF_ICACHEDATA_WORD_INDEX_MAX \
        (NRF_ICACHEDATA_DATA_WORDS_IN_UNIT_MAX * NRF_ICACHEDATA_DATA_UNITS_MAX)
/** @brief Max number of ICACHEDATA ways. */
#define NRF_ICACHEDATA_WAY_INDEX_MAX CACHEDATA_SET_WAY_MaxCount
/** @brief Max number of ICACHEDATA sets. */
#define NRF_ICACHEDATA_SET_INDEX_MAX CACHEDATA_SET_MaxCount
#endif
#endif

#if NRF_CACHE_HAS_CACHEINFO
/** @brief Max number of words in CACHEINFO data units. */
#define NRF_CACHEINFO_DATA_WORDS_IN_UNIT_MAX CACHEDATA_SET_WAY_DU_DATA_MaxCount
/** @brief Max number of CACHEINFO data units. */
#define NRF_CACHEINFO_DATA_UNITS_MAX CACHEDATA_SET_WAY_DU_MaxCount
/** @brief Max number of CACHEINFO words. */
#define NRF_CACHEINFO_WORD_INDEX_MAX \
        (NRF_CACHEINFO_DATA_WORDS_IN_UNIT_MAX * NRF_CACHEINFO_DATA_UNITS_MAX)
/** @brief Max number of CACHEINFO ways. */
#define NRF_CACHEINFO_WAY_INDEX_MAX CACHEINFO_SET_WAY_MaxCount
/** @brief Max number of CACHEINFO sets. */
#define NRF_CACHEINFO_SET_INDEX_MAX CACHEINFO_SET_MaxCount
#endif

#if NRF_CACHE_HAS_DCACHEINFO
/** @brief Max number of words in DCACHEINFO data units. */
#define NRF_DCACHEINFO_DATA_WORDS_IN_UNIT_MAX DCACHEDATA_SET_WAY_DU_DATA_MaxCount
/** @brief Max number of DCACHEINFO data units. */
#define NRF_DCACHEINFO_DATA_UNITS_MAX DCACHEDATA_SET_WAY_DU_MaxCount
/** @brief Max number of DCACHEINFO words. */
#define NRF_DCACHEINFO_WORD_INDEX_MAX \
        (NRF_DCACHEINFO_DATA_WORDS_IN_UNIT_MAX * NRF_DCACHEINFO_DATA_UNITS_MAX)
/** @brief Max number of DCACHEINFO ways. */
#define NRF_DCACHEINFO_WAY_INDEX_MAX DCACHEINFO_SET_WAY_MaxCount
/** @brief Max number of CACHEINFO sets. */
#define NRF_DCACHEINFO_SET_INDEX_MAX DCACHEINFO_SET_MaxCount
#endif

#if NRF_CACHE_HAS_ICACHEINFO
#if defined(ICACHEDATA_SET_WAY_DU_DATA_MaxCount)
/** @brief Max number of words in ICACHEINFO data units. */
#define NRF_ICACHEINFO_DATA_WORDS_IN_UNIT_MAX ICACHEDATA_SET_WAY_DU_DATA_MaxCount
/** @brief Max number of ICACHEINFO data units. */
#define NRF_ICACHEINFO_DATA_UNITS_MAX ICACHEDATA_SET_WAY_DU_MaxCount
/** @brief Max number of ICACHEINFO words. */
#define NRF_ICACHEINFO_WORD_INDEX_MAX \
        (NRF_ICACHEINFO_DATA_WORDS_IN_UNIT_MAX * NRF_ICACHEINFO_DATA_UNITS_MAX)
/** @brief Max number of ICACHEINFO ways. */
#define NRF_ICACHEINFO_WAY_INDEX_MAX ICACHEINFO_SET_WAY_MaxCount
/** @brief Max number of ICACHEINFO sets. */
#define NRF_ICACHEINFO_SET_INDEX_MAX ICACHEINFO_SET_MaxCount
/** @brief Mask of ICACHEINFO TAG field. */
#define NRF_ICACHEINFO_TAG_MASK ICACHEINFO_SET_WAY_INFO_TAG_Msk
/** @brief Mask of ICACHEINFO V field. */
#define NRF_ICACHEINFO_V_MASK ICACHEINFO_SET_WAY_INFO_V_Msk
/** @brief Position of ICACHEINFO V field. */
#define NRF_ICACHEINFO_V_POS ICACHEINFO_SET_WAY_INFO_V_Pos
/** @brief Valid value of ICACHEINFO V field. */
#define NRF_ICACHEINFO_V_VALID ICACHEINFO_SET_WAY_INFO_V_Valid
/** @brief Mask of ICACHEINFO MRU field. */
#define NRF_ICACHEINFO_MRU_MASK ICACHEINFO_SET_WAY_INFO_MRU_Msk
/** @brief Position of ICACHEINFO MRU field. */
#define NRF_ICACHEINFO_MRU_POS ICACHEINFO_SET_WAY_INFO_MRU_Pos
/** @brief Mask of ICACHEINFO DUV0 field. */
#define NRF_ICACHEINFO_DUV0_MASK ICACHEINFO_SET_WAY_INFO_DUV0_Msk
/** @brief Position of ICACHEINFO DUV0 field. */
#define NRF_ICACHEINFO_DUV0_POS ICACHEINFO_SET_WAY_INFO_DUV0_Pos
/** @brief Valid value of ICACHEINFO DUV0 field. */
#define NRF_ICACHEINFO_DUV0_VALID ICACHEINFO_SET_WAY_INFO_DUV0_Valid
/** @brief Mask of ICACHEINFO DUV1 field. */
#define NRF_ICACHEINFO_DUV1_MASK ICACHEINFO_SET_WAY_INFO_DUV1_Msk
/** @brief Position of ICACHEINFO DUV1 field. */
#define NRF_ICACHEINFO_DUV1_POS ICACHEINFO_SET_WAY_INFO_DUV1_Pos
/** @brief Valid value of ICACHEINFO DUV1 field. */
#define NRF_ICACHEINFO_DUV1_VALID ICACHEINFO_SET_WAY_INFO_DUV1_Valid
/** @brief Mask of ICACHEINFO DUV2 field. */
#define NRF_ICACHEINFO_DUV2_MASK ICACHEINFO_SET_WAY_INFO_DUV2_Msk
/** @brief Position of ICACHEINFO DUV2 field. */
#define NRF_ICACHEINFO_DUV2_POS ICACHEINFO_SET_WAY_INFO_DUV2_Pos
/** @brief Valid value of ICACHEINFO DUV2 field. */
#define NRF_ICACHEINFO_DUV2_VALID ICACHEINFO_SET_WAY_INFO_DUV2_Valid
/** @brief Mask of ICACHEINFO DUV3 field. */
#define NRF_ICACHEINFO_DUV3_MASK ICACHEINFO_SET_WAY_INFO_DUV3_Msk
/** @brief Position of ICACHEINFO DUV3 field. */
#define NRF_ICACHEINFO_DUV3_POS ICACHEINFO_SET_WAY_INFO_DUV3_Pos
/** @brief Valid value of ICACHEINFO DUV3 field. */
#define NRF_ICACHEINFO_DUV3_VALID ICACHEINFO_SET_WAY_INFO_DUV3_Valid
#else
/** @brief Max number of words in ICACHEINFO data units. */
#define NRF_ICACHEINFO_DATA_WORDS_IN_UNIT_MAX CACHEDATA_SET_WAY_DU_DATA_MaxCount
/** @brief Max number of ICACHEINFO data units. */
#define NRF_ICACHEINFO_DATA_UNITS_MAX CACHEDATA_SET_WAY_DU_MaxCount
/** @brief Max number of ICACHEINFO words. */
#define NRF_ICACHEINFO_WORD_INDEX_MAX \
        (NRF_ICACHEINFO_DATA_WORDS_IN_UNIT_MAX * NRF_ICACHEINFO_DATA_UNITS_MAX)
/** @brief Max number of ICACHEINFO ways. */
#define NRF_ICACHEINFO_WAY_INDEX_MAX CACHEINFO_SET_WAY_MaxCount
/** @brief Max number of ICACHEINFO sets. */
#define NRF_ICACHEINFO_SET_INDEX_MAX CACHEINFO_SET_MaxCount
/** @brief Mask of ICACHEINFO TAG field. */
#define NRF_ICACHEINFO_TAG_MASK CACHEINFO_SET_WAY_INFO_TAG_Msk
/** @brief Mask of ICACHEINFO V field. */
#define NRF_ICACHEINFO_V_MASK CACHEINFO_SET_WAY_INFO_V_Msk
/** @brief Position of ICACHEINFO V field. */
#define NRF_ICACHEINFO_V_POS CACHEINFO_SET_WAY_INFO_V_Pos
/** @brief Valid value of ICACHEINFO V field. */
#define NRF_ICACHEINFO_V_VALID CACHEINFO_SET_WAY_INFO_V_Valid
/** @brief Mask of ICACHEINFO MRU field. */
#define NRF_ICACHEINFO_MRU_MASK CACHEINFO_SET_WAY_INFO_MRU_Msk
/** @brief Position of ICACHEINFO MRU field. */
#define NRF_ICACHEINFO_MRU_POS CACHEINFO_SET_WAY_INFO_MRU_Pos
/** @brief Mask of ICACHEINFO DUV0 field. */
#define NRF_ICACHEINFO_DUV0_MASK CACHEINFO_SET_WAY_INFO_DUV0_Msk
/** @brief Position of ICACHEINFO DUV0 field. */
#define NRF_ICACHEINFO_DUV0_POS CACHEINFO_SET_WAY_INFO_DUV0_Pos
/** @brief Valid value of ICACHEINFO DUV0 field. */
#define NRF_ICACHEINFO_DUV0_VALID CACHEINFO_SET_WAY_INFO_DUV0_Valid
/** @brief Mask of ICACHEINFO DUV1 field. */
#define NRF_ICACHEINFO_DUV1_MASK CACHEINFO_SET_WAY_INFO_DUV1_Msk
/** @brief Position of ICACHEINFO DUV1 field. */
#define NRF_ICACHEINFO_DUV1_POS CACHEINFO_SET_WAY_INFO_DUV1_Pos
/** @brief Valid value of ICACHEINFO DUV1 field. */
#define NRF_ICACHEINFO_DUV1_VALID CACHEINFO_SET_WAY_INFO_DUV1_Valid
/** @brief Mask of ICACHEINFO DUV2 field. */
#define NRF_ICACHEINFO_DUV2_MASK CACHEINFO_SET_WAY_INFO_DUV2_Msk
/** @brief Position of ICACHEINFO DUV2 field. */
#define NRF_ICACHEINFO_DUV2_POS CACHEINFO_SET_WAY_INFO_DUV2_Pos
/** @brief Valid value of ICACHEINFO DUV2 field. */
#define NRF_ICACHEINFO_DUV2_VALID CACHEINFO_SET_WAY_INFO_DUV2_Valid
/** @brief Mask of ICACHEINFO DUV3 field. */
#define NRF_ICACHEINFO_DUV3_MASK CACHEINFO_SET_WAY_INFO_DUV3_Msk
/** @brief Position of ICACHEINFO DUV3 field. */
#define NRF_ICACHEINFO_DUV3_POS CACHEINFO_SET_WAY_INFO_DUV3_Pos
/** @brief Valid value of ICACHEINFO DUV3 field. */
#define NRF_ICACHEINFO_DUV3_VALID CACHEINFO_SET_WAY_INFO_DUV3_Valid
#endif
#endif

#if NRF_CACHE_HAS_RAMSIZE
/** @brief Max enumerator value of RAMSIZE field. */
#define NRF_CACHE_MODE_RAMSIZE_MAX CACHE_MODE_RAMSIZE_Max
#endif

#if NRF_CACHE_HAS_TASKS
/** @brief CACHE tasks. */
typedef enum
{
#if NRF_CACHE_HAS_TASK_CLEAN
    NRF_CACHE_TASK_CLEANCACHE      = offsetof(NRF_CACHE_Type, TASKS_CLEANCACHE),      /**< Clean the whole cache. */
    NRF_CACHE_TASK_CLEANLINE       = offsetof(NRF_CACHE_Type, TASKS_CLEANLINE),       /**< Clean the cache line. */
#endif
#if NRF_CACHE_HAS_TASK_FLUSH
    NRF_CACHE_TASK_FLUSHCACHE      = offsetof(NRF_CACHE_Type, TASKS_FLUSHCACHE),      /**< Flush the whole cache. */
    NRF_CACHE_TASK_FLUSHLINE       = offsetof(NRF_CACHE_Type, TASKS_FLUSHLINE),       /**< Flush the cache line. */
#endif
#if NRF_CACHE_HAS_TASK_SAVE_RESTORE
    NRF_CACHE_TASK_SAVE            = offsetof(NRF_CACHE_Type, TASKS_SAVE),            /**< Save the state to a retained memory space. */
    NRF_CACHE_TASK_RESTORE         = offsetof(NRF_CACHE_Type, TASKS_RESTORE),         /**< Restore the state from a retained memory space. */
#endif
    NRF_CACHE_TASK_INVALIDATECACHE = offsetof(NRF_CACHE_Type, TASKS_INVALIDATECACHE), /**< Invalidate the whole cache. */
#if NRF_CACHE_HAS_TASK_INVALIDATELINE
    NRF_CACHE_TASK_INVALIDATELINE  = offsetof(NRF_CACHE_Type, TASKS_INVALIDATELINE),  /**< Invalidate the cache line. */
#endif
#if NRF_CACHE_HAS_TASK_ERASE
    NRF_CACHE_TASK_ERASE           = offsetof(NRF_CACHE_Type, TASKS_ERASE),           /**< Erase the whole cache. */
#endif
} nrf_cache_task_t;
#endif

/** @brief Cache regions. */
typedef enum
{
    NRF_CACHE_REGION_FLASH = 0, ///< Cache region related to Flash access.
    NRF_CACHE_REGION_XIP   = 1, ///< Cache region related to XIP access.
} nrf_cache_region_t;

#if NRF_CACHE_HAS_RAMSIZE
/** @brief Dedicated RAM size used for cache memory. */
typedef enum
{
    NRF_CACHE_RAMSIZE_ALL     = CACHE_MODE_RAMSIZE_All,     ///< All RAM is used for cache memory.
    NRF_CACHE_RAMSIZE_HALF    = CACHE_MODE_RAMSIZE_Half,    ///< Half of the RAM is used for cache memory.
    NRF_CACHE_RAMSIZE_QUARTER = CACHE_MODE_RAMSIZE_Quarter, ///< Quarter of the RAM is used for cache memory.
    NRF_CACHE_RAMSIZE_NONE    = CACHE_MODE_RAMSIZE_None     ///< None of the RAM is used for cache memory.
} nrf_cache_ramsize_t;
#endif

/**
 * @brief Function for enabling the CACHE peripheral.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_cache_enable(NRF_CACHE_Type * p_reg);

/**
 * @brief Function for disabling the CACHE peripheral.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_cache_disable(NRF_CACHE_Type * p_reg);

/**
 * @brief Function for checking if the CACHE peripheral is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The CACHE is enabled.
 * @retval false The CACHE is not enabled.
 */
NRF_STATIC_INLINE bool nrf_cache_enable_check(NRF_CACHE_Type const * p_reg);

#if NRF_CACHE_HAS_TASK_INVALIDATELINE
/**
 * @brief Function for invalidating the cache content.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_cache_invalidate(NRF_CACHE_Type * p_reg);
#endif

#if NRF_CACHE_HAS_ERASE
/**
 * @brief Function for erasing the cache content.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_cache_erase(NRF_CACHE_Type * p_reg);
#endif

/**
 * @brief Function for checking the status of @ref nrf_cache_erase().
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Erase is finished.
 * @retval false Erase is not complete or has not started.
 */
NRF_STATIC_INLINE bool nrf_cache_erase_status_check(NRF_CACHE_Type const * p_reg);

/**
 * @brief Function for clearing the status of the cache erase.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_cache_erase_status_clear(NRF_CACHE_Type * p_reg);

/**
 * @brief Function for setting the cache profiling.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if cache profiling is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_cache_profiling_set(NRF_CACHE_Type * p_reg, bool enable);

/**
 * @brief Function for clearing the cache profiling counters.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_cache_profiling_counters_clear(NRF_CACHE_Type * p_reg);

/**
 * @brief Function for getting the number of cache hits for instruction fetch from the specified
 *        cache region.
 *
 * @note Separate counters are used for flash region and XIP region.
 * @note Cache profiling must be enabled first. See @ref nrf_cache_profiling_set.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] region Cache region.
 *
 * @return Number of instruction fetch cache hits.
 */
NRF_STATIC_INLINE uint32_t nrf_cache_instruction_hit_counter_get(NRF_CACHE_Type const * p_reg,
                                                                 nrf_cache_region_t     region);

/**
 * @brief Function for getting the number of cache misses for instruction fetch from the specified
 *        cache region.
 *
 * @note Separate counters are used for flash region and XIP region.
 * @note Cache profiling must be enabled first. See @ref nrf_cache_profiling_set.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] region Cache region.
 *
 * @return Number of instruction fetch cache misses.
 */
NRF_STATIC_INLINE uint32_t nrf_cache_instruction_miss_counter_get(NRF_CACHE_Type const * p_reg,
                                                                  nrf_cache_region_t     region);

/**
 * @brief Function for getting the number of cache hits for data fetch from the specified
 *        cache region.
 *
 * @note Separate counters are used for flash region and XIP region.
 * @note Cache profiling must be enabled first. See @ref nrf_cache_profiling_set.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] region Cache region.
 *
 * @return Number of data fetch cache hits.
 */
NRF_STATIC_INLINE uint32_t nrf_cache_data_hit_counter_get(NRF_CACHE_Type const * p_reg,
                                                          nrf_cache_region_t     region);

/**
 * @brief Function for getting the number of cache misses for data fetch from the specified
 *        cache region.
 *
 * @note Separate counters are used for flash region and XIP region.
 * @note Cache profiling must be enabled first. See @ref nrf_cache_profiling_set.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] region Cache region.
 *
 * @return Number of data fetch cache misses.
 */
NRF_STATIC_INLINE uint32_t nrf_cache_data_miss_counter_get(NRF_CACHE_Type const * p_reg,
                                                           nrf_cache_region_t     region);

#if NRF_CACHE_HAS_RAM_MODE
/**
 * @brief Function for setting the cache RAM mode.
 *
 * When configured in the RAM mode, the accesses to internal or external flash will not be cached.
 * In this mode, the cache data contents can be used as the read/write RAM.
 * Only the data content of the cache is available as RAM.
 *
 * @note Enabling the RAM mode causes the RAM to be cleared.
 * @note Disabling the RAM mode causes the cache to be invalidated.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if the cache RAM mode is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_cache_ram_mode_set(NRF_CACHE_Type * p_reg, bool enable);

/**
 * @brief Function for checking whether the cache is in RAM mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return True if the cache RAM mode is enabled, false otherwise.
 */
NRF_STATIC_INLINE bool nrf_cache_ram_mode_check(NRF_CACHE_Type const * p_reg);
#endif

#if NRF_CACHE_HAS_RAMSIZE
/**
 * @brief Function for setting the configuration of splitting the dedicated cache RAM.
 *
 * @details Dedicated cache RAM can be splitted into cache memory and generic memory.
 *          By default, all dedicated RAM is used for cache memory.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] ramsize Dedicated cache RAM split configuration.
 */
NRF_STATIC_INLINE void nrf_cache_ramsize_set(NRF_CACHE_Type * p_reg, nrf_cache_ramsize_t ramsize);

/**
 * @brief Function for getting the configuration of splitting the dedicated cache RAM.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Configuration of dedicated cache RAM split.
 */
NRF_STATIC_INLINE nrf_cache_ramsize_t nrf_cache_ramsize_get(NRF_CACHE_Type const * p_reg);
#endif

#if NRF_CACHE_HAS_DEBUGLOCK
/**
 * @brief Function for blocking the cache content access.
 *
 * To unlock the cache content access, a reset has to be performed.
 *
 * @note Blocking is ignored in the RAM mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_cache_read_lock_enable(NRF_CACHE_Type * p_reg);
#endif

#if NRF_CACHE_HAS_WRITELOCK
/**
 * @brief Function for blocking the cache content updates.
 *
 * Blocking of updates prevents updating of cache content on cache misses,
 * but the peripheral will continue to check for instruction/data fetches
 * in the content already present in the cache.
 *
 * @note Blocking is ignored in the RAM mode.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if cache content update lock is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_cache_update_lock_set(NRF_CACHE_Type * p_reg, bool enable);
#endif

#if NRF_CACHE_HAS_ANY_CACHEDATA
/**
 * @brief Function for getting the cache data word.
 *
 * @note When operating in the RAM mode, the cache data is accessible as a general purpose RAM.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] set   Set that contains the data to get.
 * @param[in] way   Way that contains the data to get.
 * @param[in] word  Data word index to get.
 *
 * @return 32-bit data word.
 */
NRF_STATIC_INLINE uint32_t nrf_cache_data_get(NRF_CACHE_Type const * p_reg,
                                              uint32_t               set,
                                              uint8_t                way,
                                              uint8_t                word);
#endif

#if NRF_CACHE_HAS_ANY_CACHEINFO
/**
 * @brief Function for getting the tag associated with the specified set and way.
 *
 * The tag is used to check if an entry in the cache matches the address that is being fetched.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] set   Set that contains the tag to get.
 * @param[in] way   Way that contains the tag to get.
 *
 * @return Tag value.
 */
NRF_STATIC_INLINE uint32_t nrf_cache_tag_get(NRF_CACHE_Type const * p_reg,
                                             uint32_t               set,
                                             uint8_t                way);

/**
 * @brief Function for checking the validity of a cache line associated with the specified set and way.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] set   Set that contains the cache line to check.
 * @param[in] way   Way that contains the cache line to check.
 *
 * @retval true  Cache line is valid.
 * @retval false Cache line is invalid.
 */
NRF_STATIC_INLINE bool nrf_cache_line_validity_check(NRF_CACHE_Type const * p_reg,
                                                     uint32_t               set,
                                                     uint8_t                way);

/**
 * @brief Function for getting the most recently used way in the specified set.
 *
 * The most recently used way is updated on each fetch from the cache and is used for the cache replacement policy.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] set   Specified set.
 *
 * @return The most recently used way in the specified set.
 */
NRF_STATIC_INLINE uint8_t nrf_cache_mru_get(NRF_CACHE_Type const * p_reg, uint32_t set);

#if NRF_CACHE_HAS_CACHEINFO_DU_VALIDATION
/**
 * @brief Function for checking the validity of a data unit associated with the specified set, way and word.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] set   Set that contains the data unit to check.
 * @param[in] way   Way that contains the data unit to check.
 * @param[in] word  Data word index.
 *
 * @retval true  Data unit is valid.
 * @retval false Data unit is invalid.
 */
NRF_STATIC_INLINE bool nrf_cache_data_unit_validity_check(NRF_CACHE_Type const * p_reg,
                                                          uint32_t               set,
                                                          uint8_t                way,
                                                          uint8_t                word);
#endif // NRF_CACHE_HAS_CACHEINFO_DU_VALIDATION
#endif // NRF_CACHE_HAS_ANY_CACHEINFO

#if NRF_CACHE_HAS_CACHEINFO_DU_DIRTY
/**
 * @brief Function for checking the dirtiness of a data unit associated with the specified set, way and word.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] set   Set that contains the data unit to check.
 * @param[in] way   Way that contains the data unit to check.
 * @param[in] word  Data word index.
 *
 * @retval true  Data unit is dirty.
 * @retval false Data unit is clean.
 */
NRF_STATIC_INLINE bool nrf_cache_is_data_unit_dirty_check(NRF_CACHE_Type const * p_reg,
                                                          uint32_t               set,
                                                          uint8_t                way,
                                                          uint8_t                word);
#endif

#if NRF_CACHE_HAS_TASKS
#if NRF_CACHE_HAS_LINEADDR
/**
 * @brief Function to set the memory address covered by the line to be maintained.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] addr  Cache line adress.
 */
NRF_STATIC_INLINE void nrf_cache_lineaddr_set(NRF_CACHE_Type * p_reg, uint32_t addr);

/**
 * @brief Function to get the memory address covered by the line to be maintained.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Cache line adress.
 */
NRF_STATIC_INLINE uint32_t nrf_cache_lineaddr_get(NRF_CACHE_Type const * p_reg);
#endif

/**
 * @brief Function for triggering the specified CACHE task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task.
 */
NRF_STATIC_INLINE void nrf_cache_task_trigger(NRF_CACHE_Type * p_reg, nrf_cache_task_t task);

/**
 * @brief Function for returning the address of the specified task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task.
 *
 * @return Task address.
 */
NRF_STATIC_INLINE uint32_t nrf_cache_task_address_get(NRF_CACHE_Type const * p_reg,
                                                      nrf_cache_task_t       task);
#endif

#if NRF_CACHE_HAS_STATUS
/**
 * @brief Function for checking if the cache is busy or not.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return True if the cache is busy, false otherwise.
 */
NRF_STATIC_INLINE bool nrf_cache_busy_check(NRF_CACHE_Type const * p_reg);
#endif

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_cache_enable(NRF_CACHE_Type * p_reg)
{
    p_reg->ENABLE = CACHE_ENABLE_ENABLE_Enabled;
}

NRF_STATIC_INLINE void nrf_cache_disable(NRF_CACHE_Type * p_reg)
{
    p_reg->ENABLE = CACHE_ENABLE_ENABLE_Disabled;
}

NRF_STATIC_INLINE bool nrf_cache_enable_check(NRF_CACHE_Type const * p_reg)
{
    return p_reg->ENABLE == CACHE_ENABLE_ENABLE_Enabled;
}

NRF_STATIC_INLINE void nrf_cache_invalidate(NRF_CACHE_Type * p_reg)
{
#if NRF_CACHE_HAS_TASKS
    nrf_cache_task_trigger(p_reg, NRF_CACHE_TASK_INVALIDATECACHE);
#else
    p_reg->INVALIDATE = CACHE_INVALIDATE_INVALIDATE_Invalidate;
#endif
}

#if NRF_CACHE_HAS_ERASE
NRF_STATIC_INLINE void nrf_cache_erase(NRF_CACHE_Type * p_reg)
{
#if NRF_CACHE_HAS_TASKS
    nrf_cache_task_trigger(p_reg, NRF_CACHE_TASK_ERASE);
#else
    p_reg->ERASE = CACHE_ERASE_ERASE_Erase;
#endif
}
#endif

NRF_STATIC_INLINE bool nrf_cache_erase_status_check(NRF_CACHE_Type const * p_reg)
{
#if NRF_CACHE_HAS_STATUS
    return (!nrf_cache_busy_check(p_reg));
#else
    return (p_reg->ERASESTATUS & CACHE_ERASESTATUS_ERASESTATUS_Msk) ==
        (CACHE_ERASESTATUS_ERASESTATUS_Finished << CACHE_ERASESTATUS_ERASESTATUS_Pos);
#endif
}

NRF_STATIC_INLINE void nrf_cache_erase_status_clear(NRF_CACHE_Type * p_reg)
{
#if NRF_CACHE_HAS_TASKS
    /* No task for erasing the status */
    (void)p_reg;
#else
    p_reg->ERASESTATUS = 0;
#endif
}

NRF_STATIC_INLINE void nrf_cache_profiling_set(NRF_CACHE_Type * p_reg, bool enable)
{
#if defined(CACHE_PROFILINGENABLE_ENABLE_Msk)
    p_reg->PROFILINGENABLE =
        (enable ? CACHE_PROFILINGENABLE_ENABLE_Enable : CACHE_PROFILINGENABLE_ENABLE_Disable);
#elif defined(CACHE_PROFILING_ENABLE_ENABLE_Msk)
    p_reg->PROFILING.ENABLE =
        (enable ? CACHE_PROFILING_ENABLE_ENABLE_Enable : CACHE_PROFILING_ENABLE_ENABLE_Disable);
#endif
}

NRF_STATIC_INLINE void nrf_cache_profiling_counters_clear(NRF_CACHE_Type * p_reg)
{
#if defined(CACHE_PROFILINGCLEAR_CLEAR_Msk)
    p_reg->PROFILINGCLEAR = (CACHE_PROFILINGCLEAR_CLEAR_Clear << CACHE_PROFILINGCLEAR_CLEAR_Pos);
#elif defined(CACHE_PROFILING_CLEAR_CLEAR_Msk)
    p_reg->PROFILING.CLEAR = (CACHE_PROFILING_CLEAR_CLEAR_Clear << CACHE_PROFILING_CLEAR_CLEAR_Pos);
#endif
}

NRF_STATIC_INLINE uint32_t nrf_cache_instruction_hit_counter_get(NRF_CACHE_Type const * p_reg,
                                                                 nrf_cache_region_t     region)
{
#if defined(CACHE_PROFILING_IHIT_HITS_Msk)
    return p_reg->PROFILING[region].IHIT;
#elif defined(CACHE_PROFILING_HIT_HITS_Msk)
    (void)region;
    return p_reg->PROFILING.HIT;
#endif
}

NRF_STATIC_INLINE uint32_t nrf_cache_instruction_miss_counter_get(NRF_CACHE_Type const * p_reg,
                                                                  nrf_cache_region_t     region)
{
#if defined(CACHE_PROFILING_IMISS_MISSES_Msk)
    return p_reg->PROFILING[region].IMISS;
#elif defined(CACHE_PROFILING_MISS_MISSES_Msk)
    (void)region;
    return p_reg->PROFILING.MISS;
#endif
}

NRF_STATIC_INLINE uint32_t nrf_cache_data_hit_counter_get(NRF_CACHE_Type const * p_reg,
                                                          nrf_cache_region_t     region)
{
#if defined(CACHE_PROFILING_DHIT_HITS_Msk)
    return p_reg->PROFILING[region].DHIT;
#elif defined(CACHE_PROFILING_HIT_HITS_Msk)
    (void)region;
    return p_reg->PROFILING.HIT;
#endif
}

NRF_STATIC_INLINE uint32_t nrf_cache_data_miss_counter_get(NRF_CACHE_Type const * p_reg,
                                                           nrf_cache_region_t     region)
{
#if defined(CACHE_PROFILING_DMISS_MISSES_Msk)
    return p_reg->PROFILING[region].DMISS;
#elif defined(CACHE_PROFILING_MISS_MISSES_Msk)
    (void)region;
    return p_reg->PROFILING.MISS;
#endif
}

#if NRF_CACHE_HAS_RAM_MODE
NRF_STATIC_INLINE void nrf_cache_ram_mode_set(NRF_CACHE_Type * p_reg, bool enable)
{
    p_reg->MODE = ((p_reg->MODE & ~CACHE_MODE_MODE_Msk) |
                   ((enable ? CACHE_MODE_MODE_Ram : CACHE_MODE_MODE_Cache)
                   << CACHE_MODE_MODE_Pos));
}

NRF_STATIC_INLINE bool nrf_cache_ram_mode_check(NRF_CACHE_Type const * p_reg)
{
    return (p_reg->MODE & CACHE_MODE_MODE_Msk) == (CACHE_MODE_MODE_Ram << CACHE_MODE_MODE_Pos);
}
#endif

#if NRF_CACHE_HAS_RAMSIZE
NRF_STATIC_INLINE void nrf_cache_ramsize_set(NRF_CACHE_Type * p_reg, nrf_cache_ramsize_t ramsize)
{
    p_reg->MODE = ((p_reg->MODE & ~CACHE_MODE_RAMSIZE_Msk) | (ramsize << CACHE_MODE_RAMSIZE_Pos));
}

NRF_STATIC_INLINE nrf_cache_ramsize_t nrf_cache_ramsize_get(NRF_CACHE_Type const * p_reg)
{
    return (nrf_cache_ramsize_t)((p_reg->MODE & CACHE_MODE_RAMSIZE_Msk) >> CACHE_MODE_RAMSIZE_Pos);
}
#endif

#if NRF_CACHE_HAS_DEBUGLOCK
NRF_STATIC_INLINE void nrf_cache_read_lock_enable(NRF_CACHE_Type * p_reg)
{
    p_reg->DEBUGLOCK = CACHE_DEBUGLOCK_DEBUGLOCK_Locked;
}
#endif

#if NRF_CACHE_HAS_WRITELOCK
NRF_STATIC_INLINE void nrf_cache_update_lock_set(NRF_CACHE_Type * p_reg, bool enable)
{
    p_reg->WRITELOCK =
        (enable ? CACHE_WRITELOCK_WRITELOCK_Locked : CACHE_WRITELOCK_WRITELOCK_Unlocked);
}
#endif

#if NRF_CACHE_HAS_ANY_CACHEDATA
NRF_STATIC_INLINE uint32_t nrf_cache_data_get(NRF_CACHE_Type const * p_reg,
                                              uint32_t               set,
                                              uint8_t                way,
                                              uint8_t                word)
{
#if NRF_CACHE_HAS_CACHEDATA_DU
#if NRF_CACHE_HAS_CACHEDATA
    (void)p_reg;

    NRFX_ASSERT(word < NRF_CACHEDATA_WORD_INDEX_MAX);
    NRFX_ASSERT(way < NRF_CACHEDATA_WAY_INDEX_MAX);
    NRFX_ASSERT(set < NRF_CACHEDATA_SET_INDEX_MAX);

    uint8_t du   = (word / NRF_CACHEDATA_DATA_WORDS_IN_UNIT_MAX);
    uint8_t data = (uint8_t)(word - (du * NRF_CACHEDATA_DATA_WORDS_IN_UNIT_MAX));

    return NRF_CACHEDATA->SET[set].WAY[way].DU[du].DATA[data];
#endif // NRF_CACHE_HAS_CACHEDATA
#if NRF_CACHE_HAS_DCACHEDATA
    if (p_reg == NRF_DCACHE)
    {
        NRFX_ASSERT(word < NRF_DCACHEDATA_WORD_INDEX_MAX);
        NRFX_ASSERT(way < NRF_DCACHEDATA_WAY_INDEX_MAX);
        NRFX_ASSERT(set < NRF_DCACHEDATA_SET_INDEX_MAX);

        uint8_t du   = (word / NRF_DCACHEDATA_DATA_WORDS_IN_UNIT_MAX);
        uint8_t data = (uint8_t)(word - (du * NRF_DCACHEDATA_DATA_WORDS_IN_UNIT_MAX));

        return NRF_DCACHEDATA->SET[set].WAY[way].DU[du].DATA[data];
    }
#endif // NRF_CACHE_HAS_DCACHEDATA
#if NRF_CACHE_HAS_ICACHEDATA
    if (p_reg == NRF_ICACHE)
    {
        NRFX_ASSERT(word < NRF_ICACHEDATA_WORD_INDEX_MAX);
        NRFX_ASSERT(way < NRF_ICACHEDATA_WAY_INDEX_MAX);
        NRFX_ASSERT(set < NRF_ICACHEDATA_SET_INDEX_MAX);

        uint8_t du   = (word / NRF_ICACHEDATA_DATA_WORDS_IN_UNIT_MAX);
        uint8_t data = (uint8_t)(word - (du * NRF_ICACHEDATA_DATA_WORDS_IN_UNIT_MAX));

        return NRF_ICACHEDATA->SET[set].WAY[way].DU[du].DATA[data];
    }
#endif // NRF_CACHE_HAS_ICACHEDATA
    NRFX_ASSERT(false);
    return 0;
#else
    (void)p_reg;

    volatile CACHEDATA_SET_WAY_Type const * reg = &NRF_CACHEDATA->SET[set].WAY[way];
    switch (word)
    {
        case 0: return reg->DATA0;
        case 1: return reg->DATA1;
        case 2: return reg->DATA2;
        case 3: return reg->DATA3;
        default:
            NRFX_ASSERT(false);
            return 0;
    }
#endif // NRF_CACHE_HAS_CACHEDATA_DU
}
#endif // NRF_CACHE_HAS_ANY_CACHEDATA

#if NRF_CACHE_HAS_ANY_CACHEINFO
NRF_STATIC_INLINE uint32_t nrf_cache_tag_get(NRF_CACHE_Type const * p_reg,
                                             uint32_t               set,
                                             uint8_t                way)
{
#if NRF_CACHE_HAS_CACHEINFO
    (void)p_reg;
#if defined(CACHEINFO_SET_WAY_INFO_TAG_Msk)
    NRFX_ASSERT(way < NRF_CACHEINFO_WAY_INDEX_MAX);
    NRFX_ASSERT(set < NRF_CACHEINFO_SET_INDEX_MAX);
    return (NRF_CACHEINFO->SET[set].WAY[way].INFO & CACHEINFO_SET_WAY_INFO_TAG_Msk);
#else
    return (NRF_CACHEINFO->SET[set].WAY[way] & CACHEINFO_SET_WAY_TAG_Msk);
#endif // defined(CACHEINFO_SET_WAY_INFO_TAG_Msk)
#endif // NRF_CACHE_HAS_CACHEINFO
#if NRF_CACHE_HAS_DCACHEINFO
    if (p_reg == NRF_DCACHE)
    {
        NRFX_ASSERT(way < NRF_DCACHEINFO_WAY_INDEX_MAX);
        NRFX_ASSERT(set < NRF_DCACHEINFO_SET_INDEX_MAX);
        return (NRF_DCACHEINFO->SET[set].WAY[way].INFO & DCACHEINFO_SET_WAY_INFO_TAG_Msk);
    }
#endif // NRF_CACHE_HAS_DCACHEINFO
#if NRF_CACHE_HAS_ICACHEINFO
    if (p_reg == NRF_ICACHE)
    {
        NRFX_ASSERT(way < NRF_ICACHEINFO_WAY_INDEX_MAX);
        NRFX_ASSERT(set < NRF_ICACHEINFO_SET_INDEX_MAX);
        return (NRF_ICACHEINFO->SET[set].WAY[way].INFO & NRF_ICACHEINFO_TAG_MASK);
    }
#endif // NRF_CACHE_HAS_ICACHEINFO
    NRFX_ASSERT(false);
    return 0;
}

NRF_STATIC_INLINE bool nrf_cache_line_validity_check(NRF_CACHE_Type const * p_reg,
                                                     uint32_t               set,
                                                     uint8_t                way)
{
#if NRF_CACHE_HAS_CACHEINFO
    (void)p_reg;
#if defined(CACHEINFO_SET_WAY_INFO_V_Msk)
    NRFX_ASSERT(way < NRF_CACHEINFO_WAY_INDEX_MAX);
    NRFX_ASSERT(set < NRF_CACHEINFO_SET_INDEX_MAX);
    return (NRF_CACHEINFO->SET[set].WAY[way].INFO & CACHEINFO_SET_WAY_INFO_V_Msk) ==
        (CACHEINFO_SET_WAY_INFO_V_Valid << CACHEINFO_SET_WAY_INFO_V_Pos);
#else
    return (NRF_CACHEINFO->SET[set].WAY[way] & CACHEINFO_SET_WAY_V_Msk) ==
        (CACHEINFO_SET_WAY_V_Valid << CACHEINFO_SET_WAY_V_Pos);
#endif // defined(CACHEINFO_SET_WAY_INFO_V_Msk)
#endif // NRF_CACHE_HAS_CACHEINFO
#if NRF_CACHE_HAS_DCACHEINFO
    if (p_reg == NRF_DCACHE)
    {
        NRFX_ASSERT(way < NRF_DCACHEINFO_WAY_INDEX_MAX);
        NRFX_ASSERT(set < NRF_DCACHEINFO_SET_INDEX_MAX);
        return (NRF_DCACHEINFO->SET[set].WAY[way].INFO & DCACHEINFO_SET_WAY_INFO_V_Msk) ==
            (DCACHEINFO_SET_WAY_INFO_V_Valid << DCACHEINFO_SET_WAY_INFO_V_Pos);
    }
#endif // NRF_CACHE_HAS_DCACHEINFO
#if NRF_CACHE_HAS_ICACHEINFO
    if (p_reg == NRF_ICACHE)
    {
        NRFX_ASSERT(way < NRF_ICACHEINFO_WAY_INDEX_MAX);
        NRFX_ASSERT(set < NRF_ICACHEINFO_SET_INDEX_MAX);
        return (NRF_ICACHEINFO->SET[set].WAY[way].INFO & NRF_ICACHEINFO_V_MASK) ==
            (NRF_ICACHEINFO_V_VALID << NRF_ICACHEINFO_V_POS);
    }
#endif // NRF_CACHE_HAS_ICACHEINFO
    NRFX_ASSERT(false);
    return 0;
}

NRF_STATIC_INLINE uint8_t nrf_cache_mru_get(NRF_CACHE_Type const * p_reg, uint32_t set)
{
#if NRF_CACHE_HAS_CACHEINFO
    (void)p_reg;
#if defined(CACHEINFO_SET_WAY_INFO_MRU_Pos)
    NRFX_ASSERT(set < NRF_CACHEINFO_SET_INDEX_MAX);
    return ((NRF_CACHEINFO->SET[set].WAY[0].INFO & CACHEINFO_SET_WAY_INFO_MRU_Msk) >>
        CACHEINFO_SET_WAY_INFO_MRU_Pos);
#else
    return ((NRF_CACHEINFO->SET[set].WAY[0] & CACHEINFO_SET_WAY_MRU_Msk) >>
        CACHEINFO_SET_WAY_MRU_Pos);
#endif // defined(CACHEINFO_SET_WAY_INFO_MRU_Pos)
#endif // NRF_CACHE_HAS_CACHEINFO
#if NRF_CACHE_HAS_DCACHEINFO
    if (p_reg == NRF_DCACHE)
    {
        NRFX_ASSERT(set < NRF_DCACHEINFO_SET_INDEX_MAX);
        return ((NRF_DCACHEINFO->SET[set].WAY[0].INFO & DCACHEINFO_SET_WAY_INFO_MRU_Msk) >>
            DCACHEINFO_SET_WAY_INFO_MRU_Pos);
    }
#endif // NRF_CACHE_HAS_DCACHEINFO
#if NRF_CACHE_HAS_ICACHEINFO
    if (p_reg == NRF_ICACHE)
    {
        NRFX_ASSERT(set < NRF_ICACHEINFO_SET_INDEX_MAX);
        return ((NRF_ICACHEINFO->SET[set].WAY[0].INFO & NRF_ICACHEINFO_MRU_MASK) >>
            NRF_ICACHEINFO_MRU_POS);
    }
#endif // NRF_CACHE_HAS_ICACHEINFO
    NRFX_ASSERT(false);
    return 0;
}

#if NRF_CACHE_HAS_CACHEINFO_DU_VALIDATION
NRF_STATIC_INLINE bool nrf_cache_data_unit_validity_check(NRF_CACHE_Type const * p_reg,
                                                          uint32_t               set,
                                                          uint8_t                way,
                                                          uint8_t                word)
{
#if NRF_CACHE_HAS_CACHEINFO
    (void)p_reg;

    NRFX_ASSERT(word < NRF_CACHEINFO_WORD_INDEX_MAX);
    NRFX_ASSERT(way < NRF_CACHEINFO_WAY_INDEX_MAX);
    NRFX_ASSERT(set < NRF_CACHEINFO_SET_INDEX_MAX);

    uint8_t du = (word / NRF_CACHEINFO_DATA_WORDS_IN_UNIT_MAX);
    switch (du)
    {
#if NRF_CACHE_HAS_CACHEINFO_SET_WAY_INFO
        case 0:
            return (NRF_CACHEINFO->SET[set].WAY[way].INFO &
                    CACHEINFO_SET_WAY_INFO_DUV0_Msk) ==
                   (CACHEINFO_SET_WAY_INFO_DUV0_Valid << CACHEINFO_SET_WAY_INFO_DUV0_Pos);
        case 1:
            return (NRF_CACHEINFO->SET[set].WAY[way].INFO &
                    CACHEINFO_SET_WAY_INFO_DUV1_Msk) ==
                   (CACHEINFO_SET_WAY_INFO_DUV1_Valid << CACHEINFO_SET_WAY_INFO_DUV1_Pos);
        case 2:
            return (NRF_CACHEINFO->SET[set].WAY[way].INFO &
                    CACHEINFO_SET_WAY_INFO_DUV2_Msk) ==
                   (CACHEINFO_SET_WAY_INFO_DUV2_Valid << CACHEINFO_SET_WAY_INFO_DUV2_Pos);
        case 3:
            return (NRF_CACHEINFO->SET[set].WAY[way].INFO &
                    CACHEINFO_SET_WAY_INFO_DUV3_Msk) ==
                   (CACHEINFO_SET_WAY_INFO_DUV3_Valid << CACHEINFO_SET_WAY_INFO_DUV3_Pos);
#else
        case 0:
            return (NRF_CACHEINFO->SET[set].WAY[way] & CACHEINFO_SET_WAY_DUV0_Msk) ==
                   (CACHEINFO_SET_WAY_DUV0_Valid << CACHEINFO_SET_WAY_DUV0_Pos);
        case 1:
            return (NRF_CACHEINFO->SET[set].WAY[way] & CACHEINFO_SET_WAY_DUV1_Msk) ==
                   (CACHEINFO_SET_WAY_DUV1_Valid << CACHEINFO_SET_WAY_DUV1_Pos);
        case 2:
            return (NRF_CACHEINFO->SET[set].WAY[way] & CACHEINFO_SET_WAY_DUV2_Msk) ==
                   (CACHEINFO_SET_WAY_DUV2_Valid << CACHEINFO_SET_WAY_DUV2_Pos);
        case 3:
            return (NRF_CACHEINFO->SET[set].WAY[way] & CACHEINFO_SET_WAY_DUV3_Msk) ==
                   (CACHEINFO_SET_WAY_DUV3_Valid << CACHEINFO_SET_WAY_DUV3_Pos);
#endif // NRF_CACHE_HAS_CACHEINFO_SET_WAY_INFO
        default:
            NRFX_ASSERT(false);
            return false;
    }
#endif // NRF_CACHE_HAS_CACHEINFO
#if NRF_CACHE_HAS_DCACHEINFO
    if (p_reg == NRF_DCACHE)
    {
        NRFX_ASSERT(word < NRF_DCACHEINFO_WORD_INDEX_MAX);
        NRFX_ASSERT(way < NRF_DCACHEINFO_WAY_INDEX_MAX);
        NRFX_ASSERT(set < NRF_DCACHEINFO_SET_INDEX_MAX);

        uint8_t du = (word / NRF_DCACHEINFO_DATA_WORDS_IN_UNIT_MAX);
        switch (du)
        {
            case 0:
                return (NRF_DCACHEINFO->SET[set].WAY[way].INFO &
                        DCACHEINFO_SET_WAY_INFO_DUV0_Msk) ==
                       (DCACHEINFO_SET_WAY_INFO_DUV0_Valid << DCACHEINFO_SET_WAY_INFO_DUV0_Pos);
            case 1:
                return (NRF_DCACHEINFO->SET[set].WAY[way].INFO &
                        DCACHEINFO_SET_WAY_INFO_DUV1_Msk) ==
                       (DCACHEINFO_SET_WAY_INFO_DUV1_Valid << DCACHEINFO_SET_WAY_INFO_DUV1_Pos);
            case 2:
                return (NRF_DCACHEINFO->SET[set].WAY[way].INFO &
                        DCACHEINFO_SET_WAY_INFO_DUV2_Msk) ==
                       (DCACHEINFO_SET_WAY_INFO_DUV2_Valid << DCACHEINFO_SET_WAY_INFO_DUV2_Pos);
            case 3:
                return (NRF_DCACHEINFO->SET[set].WAY[way].INFO &
                        DCACHEINFO_SET_WAY_INFO_DUV3_Msk) ==
                       (DCACHEINFO_SET_WAY_INFO_DUV3_Valid << DCACHEINFO_SET_WAY_INFO_DUV3_Pos);
            default:
                NRFX_ASSERT(false);
                return false;
        }
    }
#endif // NRF_CACHE_HAS_DCACHEINFO
#if NRF_CACHE_HAS_ICACHEINFO
    if (p_reg == NRF_ICACHE)
    {
        NRFX_ASSERT(word < NRF_ICACHEINFO_WORD_INDEX_MAX);
        NRFX_ASSERT(way < NRF_ICACHEINFO_WAY_INDEX_MAX);
        NRFX_ASSERT(set < NRF_ICACHEINFO_SET_INDEX_MAX);

        uint8_t du = (word / NRF_ICACHEINFO_DATA_WORDS_IN_UNIT_MAX);
        switch (du)
        {
            case 0:
                return (NRF_ICACHEINFO->SET[set].WAY[way].INFO &
                        NRF_ICACHEINFO_DUV0_MASK) ==
                       (NRF_ICACHEINFO_DUV0_VALID << NRF_ICACHEINFO_DUV0_POS);
            case 1:
                return (NRF_ICACHEINFO->SET[set].WAY[way].INFO &
                        NRF_ICACHEINFO_DUV1_MASK) ==
                       (NRF_ICACHEINFO_DUV1_VALID << NRF_ICACHEINFO_DUV1_POS);
            case 2:
                return (NRF_ICACHEINFO->SET[set].WAY[way].INFO &
                        NRF_ICACHEINFO_DUV1_MASK) ==
                       (NRF_ICACHEINFO_DUV1_VALID << NRF_ICACHEINFO_DUV1_POS);
            case 3:
                return (NRF_ICACHEINFO->SET[set].WAY[way].INFO &
                        NRF_ICACHEINFO_DUV1_MASK) ==
                       (NRF_ICACHEINFO_DUV1_VALID << NRF_ICACHEINFO_DUV1_POS);
            default:
                NRFX_ASSERT(false);
                return false;
        }
    }
#endif // NRF_CACHE_HAS_ICACHEINFO
    NRFX_ASSERT(false);
    return 0;
}
#endif // NRF_CACHE_HAS_CACHEINFO_DU_VALIDATION

#if NRF_CACHE_HAS_CACHEINFO_DU_DIRTY
NRF_STATIC_INLINE bool nrf_cache_is_data_unit_dirty_check(NRF_CACHE_Type const * p_reg,
                                                          uint32_t               set,
                                                          uint8_t                way,
                                                          uint8_t                word)
{
#if NRF_CACHE_HAS_CACHEINFO
    (void)p_reg;

    NRFX_ASSERT(word < NRF_CACHEINFO_WORD_INDEX_MAX);
    NRFX_ASSERT(way < NRF_CACHEINFO_WAY_INDEX_MAX);
    NRFX_ASSERT(set < NRF_CACHEINFO_SET_INDEX_MAX);

    uint8_t du = (word / NRF_CACHEINFO_DATA_WORDS_IN_UNIT_MAX);
    switch (du)
    {
        case 0:
            /* FALLTHROUGH */
        case 1:
            return (NRF_CACHEINFO->SET[set].WAY[way] & CACHEINFO_SET_WAY_D0_Msk) ==
                   (CACHEINFO_SET_WAY_D0_Dirty << CACHEINFO_SET_WAY_D0_Pos);
        case 2:
            /* FALLTHROUGH */
        case 3:
            return (NRF_CACHEINFO->SET[set].WAY[way] & CACHEINFO_SET_WAY_D1_Msk) ==
                   (CACHEINFO_SET_WAY_D1_Dirty << CACHEINFO_SET_WAY_D1_Pos);
        default:
            NRFX_ASSERT(false);
            return false;
    }
#endif // NRF_CACHE_HAS_CACHEINFO
#if NRF_CACHE_HAS_DCACHEINFO
    if (p_reg == NRF_DCACHE)
    {
        NRFX_ASSERT(word < NRF_DCACHEINFO_WORD_INDEX_MAX);
        NRFX_ASSERT(way < NRF_DCACHEINFO_WAY_INDEX_MAX);
        NRFX_ASSERT(set < NRF_DCACHEINFO_SET_INDEX_MAX);

        uint8_t du = (word / NRF_DCACHEINFO_DATA_WORDS_IN_UNIT_MAX);
        switch (du)
        {
            case 0:
                /* FALLTHROUGH */
            case 1:
                return (NRF_DCACHEINFO->SET[set].WAY[way].INFO & DCACHEINFO_SET_WAY_INFO_D0_Msk) ==
                       (DCACHEINFO_SET_WAY_INFO_D0_Dirty << DCACHEINFO_SET_WAY_INFO_D0_Pos);
            case 2:
                /* FALLTHROUGH */
            case 3:
                return (NRF_DCACHEINFO->SET[set].WAY[way].INFO & DCACHEINFO_SET_WAY_INFO_D1_Msk) ==
                       (DCACHEINFO_SET_WAY_INFO_D1_Dirty << DCACHEINFO_SET_WAY_INFO_D1_Pos);
            default:
                NRFX_ASSERT(false);
                return false;
        }
    }
#endif // NRF_CACHE_HAS_DCACHEINFO
#if NRF_CACHE_HAS_ICACHEINFO
    if (p_reg == NRF_ICACHE)
    {
        NRFX_ASSERT(word < NRF_ICACHEINFO_WORD_INDEX_MAX);
        NRFX_ASSERT(way < NRF_ICACHEINFO_WAY_INDEX_MAX);
        NRFX_ASSERT(set < NRF_ICACHEINFO_SET_INDEX_MAX);

        uint8_t du = (word / NRF_ICACHEINFO_DATA_WORDS_IN_UNIT_MAX);
        switch (du)
        {
            case 0:
                /* FALLTHROUGH */
            case 1:
                return (NRF_ICACHEINFO->SET[set].WAY[way].INFO & ICACHEINFO_SET_WAY_INFO_D0_Msk) ==
                       (ICACHEINFO_SET_WAY_INFO_D0_Dirty << ICACHEINFO_SET_WAY_INFO_D0_Pos);
            case 2:
                /* FALLTHROUGH */
            case 3:
                return (NRF_ICACHEINFO->SET[set].WAY[way].INFO & ICACHEINFO_SET_WAY_INFO_D1_Msk) ==
                       (ICACHEINFO_SET_WAY_INFO_D1_Dirty << ICACHEINFO_SET_WAY_INFO_D1_Pos);
            default:
                NRFX_ASSERT(false);
                return false;
        }
    }
#endif // NRF_CACHE_HAS_ICACHEINFO
    NRFX_ASSERT(false);
    return 0;
}
#endif // NRF_CACHE_HAS_CACHEINFO_DU_DIRTY
#endif // NRF_CACHE_HAS_ANY_CACHEINFO

#if NRF_CACHE_HAS_TASKS
#if NRF_CACHE_HAS_LINEADDR
NRF_STATIC_INLINE void nrf_cache_lineaddr_set(NRF_CACHE_Type * p_reg, uint32_t addr)
{
    p_reg->LINEADDR = addr;
}

NRF_STATIC_INLINE uint32_t nrf_cache_lineaddr_get(NRF_CACHE_Type const * p_reg)
{
    return p_reg->LINEADDR;
}
#endif

NRF_STATIC_INLINE void nrf_cache_task_trigger(NRF_CACHE_Type * p_reg, nrf_cache_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_cache_task_address_get(NRF_CACHE_Type const * p_reg,
                                                      nrf_cache_task_t       task)
{
    return (uint32_t)p_reg + (uint32_t)task;
}
#endif

#if NRF_CACHE_HAS_STATUS
NRF_STATIC_INLINE bool nrf_cache_busy_check(NRF_CACHE_Type const * p_reg)
{
#if defined(CACHE_STATUS_READY_Msk)
    return (p_reg->STATUS & CACHE_STATUS_READY_Msk) ==
        (CACHE_STATUS_READY_Busy << CACHE_STATUS_READY_Pos);
#else
    return (p_reg->STATUS & CACHE_STATUS_BUSY_Msk) ==
        (CACHE_STATUS_BUSY_Busy << CACHE_STATUS_BUSY_Pos);
#endif
}
#endif

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_CACHE_H__
