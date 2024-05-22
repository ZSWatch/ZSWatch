#pragma once

#include <stdint.h>
#include <stddef.h>

#define ZSW_HISTORY_MAX_KEY_LENGTH      64

/** @brief ZSWatch history object definition.
*/
typedef struct {
    uint32_t write_index;
    uint32_t max_samples;                       /**< Length of the sample storage in samples. */
    uint8_t sample_size;                        /**< Size of a sample in bytes. */
    char key[ZSW_HISTORY_MAX_KEY_LENGTH];       /**< */
    void *samples;                              /**< Pointer to sample storage. */
} zsw_history_t;

/** @brief              Initialize a history object.
 *  @param p_history    History object
 *  @param max_samples  Length of the sample storage in samples
 *  @param sample_size  Size of one sample in the sample storage
 *  @param p_samples    Pointer to the sample storage
 *  @param p_key        Pointer to the NVS key (max. length 64 bytes)
 *  @return             0 when successful
*/
int zsw_history_init(zsw_history_t *p_history, uint32_t max_samples, uint8_t sample_size, void *samples,
                     const char *p_key);

/** @brief              Clear the sample storage and reset the sample counter.
 *  @param p_history    History object
 *  @return             0 when successful
*/
int zsw_history_del(zsw_history_t *p_history);

/** @brief              Add a sample to the history.
 *  @param p_history    History object
 *  @param p_sample     Pointer to sample object
 *  @return             0 when successful
*/
void zsw_history_add(zsw_history_t *p_history, const void *p_sample);

/** @brief              Get a sample from the history.
 *  @param p_history    History object
 *  @param p_sample     Pointer to sample object
 *  @param index        Sample index
*/
void zsw_history_get(const zsw_history_t *p_history, void *p_sample, uint32_t index);

/** @brief              Load a history from the NVS.
 *  @param p_history    History object
 *  @return             0 when successfulconst
*/
int zsw_history_load(zsw_history_t *p_history);

/** @brief              Stores the history in the NVS.
 *  @param p_history    History object
 *  @param p_sample     Pointer to sample data
 *  @return             0 when successful
*/
int zsw_history_save(zsw_history_t *p_history, const void *p_sample);

/** @brief              Get the number of samples stored in the history.
 *  @param p_history    History object
 *  @return             Number of samples
*/
int zsw_history_samples(zsw_history_t *p_history);