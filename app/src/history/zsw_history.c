#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

#include "zsw_history.h"

#define ZSW_HISTORY_HEADER_EXTENSION    "head"
#define ZSW_HISTORY_DATA_EXTENSION      "data"

// Text + 1 byte (_) + 6 byte extension (header) + 1 byte (\0)
static char key_data[ZSW_HISTORY_MAX_KEY_LENGTH + 8];
static char key_header[ZSW_HISTORY_MAX_KEY_LENGTH + 8];

LOG_MODULE_REGISTER(zsw_history, CONFIG_ZSW_HISTORY_LOG_LEVEL);

static int zsw_history_load_header_cb(const char *p_key, size_t len, settings_read_cb read_cb, void *p_cb_arg,
                                      void *p_param)
{
    zsw_history_t *p_history;
    zsw_history_t temp_stored_history;
    uint32_t num_bytes_header;

    p_history = (zsw_history_t *)p_param;

    num_bytes_header = read_cb(p_cb_arg, &temp_stored_history, sizeof(zsw_history_t));
    LOG_DBG("Read %u header bytes, expecting: %d", num_bytes_header, sizeof(zsw_history_t));

    // In case data structure or the user changed either sample size or number of max samples we need to handle that.
    if ((num_bytes_header == 0) || (num_bytes_header != sizeof(zsw_history_t))) {
        LOG_ERR("Invalid header. Struct size changed!");
        zsw_history_del(p_history);
    } else if (temp_stored_history.max_samples != p_history->max_samples) {
        LOG_ERR("max_samples does not match what's stored in settings. Erasing history.");
        zsw_history_del(p_history);
    } else if (temp_stored_history.sample_size != p_history->sample_size) {
        LOG_ERR("sample_size does not match what's stored in settings. Erasing history.");
        zsw_history_del(p_history);
    } else {
        // Everything is fine, we can load the history
        p_history->write_index = temp_stored_history.write_index;
        p_history->num_samples = temp_stored_history.num_samples;
    }

    return 0;
}

static int zsw_history_load_data_cb(const char *p_key, size_t len, settings_read_cb read_cb, void *p_cb_arg,
                                    void *p_param)
{
    zsw_history_t *history;
    uint32_t num_bytes_data;

    history = (zsw_history_t *)p_param;

    num_bytes_data = read_cb(p_cb_arg, history->samples, len);
    LOG_DBG("Read %u data bytes", num_bytes_data);

    if ((num_bytes_data == 0) || (num_bytes_data != len) ||
        (num_bytes_data % history->sample_size) != 0) {
        LOG_ERR("Invalid data!");
        return -EFAULT;
    }

    return 0;
}

int zsw_history_init(zsw_history_t *p_history, uint32_t max_samples, uint8_t sample_size, void *p_samples,
                     const char *p_key)
{
    int32_t error;

    __ASSERT((p_history != NULL) && (p_samples != NULL) && (p_key != NULL), "Invalid parameters for zsw_history_init");

    p_history->write_index = 0;
    p_history->num_samples = 0;
    p_history->max_samples = max_samples;
    p_history->sample_size = sample_size;
    p_history->samples = p_samples;

    memset(p_samples, 0, max_samples * sample_size);
    strcpy(p_history->key, p_key);

    error = settings_subsys_init();
    if (error) {
        LOG_ERR("Error during settings initialization! Error: %i", error);
        return -EFAULT;
    }

    return 0;
}

int zsw_history_del(zsw_history_t *p_history)
{
    int32_t error;

    __ASSERT(p_history != NULL, "Invalid parameter for zsw_history_del");

    memset(p_history->samples, 0, p_history->max_samples * p_history->sample_size);
    p_history->write_index = 0;
    p_history->num_samples = 0;

    // First: Delete the header
    sprintf(key_header, "%s/%s", p_history->key, ZSW_HISTORY_HEADER_EXTENSION);
    error = settings_delete(key_header);
    if (error) {
        LOG_ERR("Error during erasing the header! Error: %i", error);
        return -EFAULT;
    }

    // Second: Delete the data
    sprintf(key_data, "%s/%s", p_history->key, ZSW_HISTORY_DATA_EXTENSION);
    error = settings_delete(key_data);
    if (error) {
        LOG_ERR("Error during erasing the data! Error: %i", error);
        return -EFAULT;
    }

    zsw_history_save(p_history);

    return 0;
}

void zsw_history_add(zsw_history_t *p_history, const void *p_sample)
{
    uint8_t *start;

    __ASSERT((p_history != NULL) && (p_sample != NULL), "Invalid parameters for zsw_history_add");

    start = p_history->samples;
    start += p_history->sample_size * p_history->write_index;

    LOG_DBG("Add sample with size %d at index %d", p_history->sample_size, p_history->write_index);
    memcpy(start, p_sample, p_history->sample_size);

    if (p_history->write_index < (p_history->max_samples - 1)) {
        p_history->write_index++;
    } else {
        p_history->write_index = 0;
    }

    p_history->num_samples = MIN(p_history->num_samples + 1, p_history->max_samples);
}

void zsw_history_get(const zsw_history_t *p_history, void *p_sample, uint32_t index)
{
    uint8_t *start;

    __ASSERT((p_history != NULL) && (p_sample != NULL) &&
             index < p_history->max_samples, "Invalid parameters for zsw_history_get");

    start = (uint8_t *)p_history->samples;

    if (p_history->num_samples == p_history->max_samples) {
        start += ((p_history->write_index + index) % p_history->max_samples) * p_history->sample_size;
    } else {
        start += index * p_history->sample_size;
    }

    memcpy(p_sample, start, p_history->sample_size);
}

int zsw_history_load(zsw_history_t *p_history)
{
    int32_t error;

    __ASSERT((p_history != NULL) &&
             (strlen(p_history->key) <= ZSW_HISTORY_MAX_KEY_LENGTH), "Invalid parameters for zsw_history_load");

    sprintf(key_header, "%s/%s", p_history->key, ZSW_HISTORY_HEADER_EXTENSION);
    error = settings_load_subtree_direct(key_header, zsw_history_load_header_cb, p_history);

    if (error) {
        LOG_ERR("Error during header loading! Error: %i. Erasing history.", error);
        zsw_history_del(p_history);
    }

    LOG_DBG("Load header with key %s", key_header);
    LOG_DBG("   Num: %u", p_history->max_samples);
    LOG_DBG("   Sample size: %u", p_history->sample_size);
    LOG_DBG("   Write index: %u", p_history->write_index);
    LOG_DBG("   Num samples: %u", p_history->num_samples);

    // Load the data
    sprintf(key_data, "%s/%s", p_history->key, ZSW_HISTORY_DATA_EXTENSION);
    error = settings_load_subtree_direct(key_data, zsw_history_load_data_cb, p_history);
    LOG_DBG("Load data with key %s", key_data);
    if (error) {
        LOG_ERR("Error during data loading! Error: %i", error);
        zsw_history_del(p_history);
        return 0;
    }

    return 0;
}

int zsw_history_save(zsw_history_t *p_history)
{
    int32_t error;

    __ASSERT((p_history != NULL) &&
             (strlen(p_history->key) <= ZSW_HISTORY_MAX_KEY_LENGTH), "Invalid parameters for zsw_history_save");

    // First: Store the header
    sprintf(key_header, "%s/%s", p_history->key, ZSW_HISTORY_HEADER_EXTENSION);
    LOG_DBG("Storing header with key %s", key_header);
    error = settings_save_one(key_header, p_history, sizeof(zsw_history_t));
    if (error) {
        LOG_ERR("Error during saving of history header! Error: %i", error);
        return -EFAULT;
    }

    // Second: Save the data
    sprintf(key_data, "%s/%s", p_history->key, ZSW_HISTORY_DATA_EXTENSION);
    error = settings_save_one(key_data, p_history->samples, p_history->write_index * p_history->sample_size);
    if (error) {
        LOG_ERR("Error during saving of history data! Error: %i", error);
        return -EFAULT;
    }

    return 0;
}

int zsw_history_samples(zsw_history_t *p_history)
{
    __ASSERT(p_history != NULL, "Invalid parameters for zsw_history_samples");

    return p_history->num_samples;
}