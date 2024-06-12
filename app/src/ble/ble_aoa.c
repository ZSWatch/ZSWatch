/*
 * Copyright 2022 u-blox
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdbool.h>
#include <stdint.h>
#ifdef CONFIG_BT_DF
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/direction.h>

#include "ble_aoa.h"

// Length of CTE in unit of 8 [us]
#define CTE_LEN (0x14U)
// Number of CTE send in single periodic advertising train
#define PER_ADV_EVENT_CTE_COUNT 1
#define PER_ADV_DATA_LEN 200
// Eddystone
#define EDDYSTONE_INSTANCE_ID_LEN 6
#define EDDYSTONE_NAMESPACE_LENGTH 10
// Name space for u-connectLocate
#define NAME_SPACE_ID 'N','I','N','A','-','B','4','T','A','G'

static struct bt_le_ext_adv *m_ext_adv;

static struct bt_le_adv_param m_adv_param =
    BT_LE_ADV_PARAM_INIT(BT_LE_ADV_OPT_EXT_ADV |
                         BT_LE_ADV_OPT_USE_NAME | BT_LE_ADV_OPT_NO_2M,
                         BT_GAP_ADV_FAST_INT_MIN_2,
                         BT_GAP_ADV_FAST_INT_MAX_2,
                         NULL);

static struct bt_le_ext_adv_start_param m_ext_adv_start_param = {
    .timeout = 0,
    .num_events = 0,
};

struct bt_df_adv_cte_tx_param m_cte_params = {
    .cte_len = CTE_LEN,
    .cte_count = PER_ADV_EVENT_CTE_COUNT,
    .cte_type = BT_DF_CTE_TYPE_AOA,
    .num_ant_ids = 0,
    .ant_ids = NULL
};

static bool initialized;

// Offsets of the different data in bt_data m_adv_data[] below
#define ADV_DATA_OFFSET_NAMESPACE 4
#define ADV_DATA_OFFSET_INSTANCE 14
#define ADV_DATA_OFFSET_TX_POWER 3

static struct bt_data m_adv_data[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0xaa, 0xfe),
    BT_DATA_BYTES(BT_DATA_SVC_DATA16,
                  0xaa, 0xfe,       /* Eddystone UUID */
                  0x00,             /* Eddystone-UID frame type */
                  0x03,             /* TX Power */
                  NAME_SPACE_ID,    /* Namespace */
                  0x00, 0x00, 0x00, /* Instance Id */
                  0x00, 0x00, 0x00,
                  0x00,             /* reserved */
                  0x00              /* reserved */
                 )
};

static bool set_adv_params(uint16_t min_ms, uint16_t max_ms)
{
    struct bt_le_per_adv_param per_adv_param = {
        .interval_min = min_ms / 1.25,
        .interval_max = max_ms / 1.25,
        .options = BT_LE_ADV_OPT_USE_TX_POWER | BT_LE_ADV_OPT_NO_2M,
    };
    return bt_le_per_adv_set_param(m_ext_adv, &per_adv_param) == 0;
}

bool bleAoaInit()
{
    bool ok;

    bt_addr_le_t addr;
    size_t cnt = 1;
    bt_id_get(&addr, &cnt);
    uint8_t id[EDDYSTONE_INSTANCE_ID_LEN];
    // Only swap public address. It's done like this in u-connect.
    if (addr.type == BT_ADDR_LE_PUBLIC) {
        for (uint8_t i = 0; i < EDDYSTONE_INSTANCE_ID_LEN; i++) {
            id[i] = addr.a.val[(EDDYSTONE_INSTANCE_ID_LEN - 1) - i];
        }
    } else {
        memcpy(id, addr.a.val, EDDYSTONE_INSTANCE_ID_LEN);
    }
    memcpy((uint8_t *)&m_adv_data[2].data[ADV_DATA_OFFSET_INSTANCE], id, EDDYSTONE_INSTANCE_ID_LEN);

    ok = bt_le_ext_adv_create(&m_adv_param, NULL, &m_ext_adv) == 0;
    ok = ok && bt_le_ext_adv_set_data(m_ext_adv, m_adv_data, ARRAY_SIZE(m_adv_data), NULL, 0) == 0;
    ok = ok && bt_df_set_adv_cte_tx_param(m_ext_adv, &m_cte_params) == 0;
    ok = ok && set_adv_params(500, 500);
    ok = ok && bt_df_adv_cte_tx_enable(m_ext_adv) == 0;

    initialized = ok;
    return ok;
}

bool bleAoaAdvertise(uint16_t min, uint16_t max, bool on)
{
    if (!initialized) {
        return false;
    }
    bool ok = true;
    bt_le_per_adv_stop(m_ext_adv);
    bt_le_ext_adv_stop(m_ext_adv);
    if (on) {
        ok = set_adv_params(min, max) &&
             bt_le_per_adv_start(m_ext_adv) == 0 &&
             bt_le_ext_adv_start(m_ext_adv, &m_ext_adv_start_param) == 0;
    }
    return ok;
}
#else
bool bleAoaInit()
{
    return true;
}

bool bleAoaAdvertise(uint16_t min, uint16_t max, bool on)
{
    return true;
}
#endif