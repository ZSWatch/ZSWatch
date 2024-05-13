/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2024.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

#include "managers/zsw_app_manager.h"
#include "ble/ble_comm_central.h"

#include "peripherals_ui.h"

LOG_MODULE_REGISTER(peripherals_app);
LV_IMG_DECLARE(zephyr_icon_round);

extern const lv_peripheral_info_t default_info;

static lv_peripherals_type_t selected_peripheral = LV_PERIPHERALS_TYPE_LAST;

static void close_button_handler(lv_peripherals_page_t page);
static void peripheral_selected_handler(lv_peripherals_type_t peripheral_type);
static void scan_start_handler(void);
static void peripheral_remove_handler(void);
static void discovered_peripheral_clicked_handler(uint32_t index);
static void on_new_peripheral_discovered_handler(const char *name);
static void on_peripheral_connected_handler(const char *name, ble_comm_central_periph_type_t periph_type, ble_comm_central_periph_status_t periph_status);
static ble_comm_central_periph_type_t lv_peripherals_to_ble_comm_peripherals_type(lv_peripherals_type_t periph_type);
static void set_page(lv_peripherals_page_t next_page);

static const lv_peripherals_handlers_t ui_evt_handlers = {
    .close_button_handler = close_button_handler,
    .peripheral_selected_handler = peripheral_selected_handler,
    .scan_start_handler = scan_start_handler,
    .peripheral_remove_handler = peripheral_remove_handler,
    .discovered_peripheral_clicked_handler = discovered_peripheral_clicked_handler
};

static void peripherals_app_start(lv_obj_t *root, lv_group_t *group)
{
    selected_peripheral = LV_PERIPHERALS_TYPE_LAST;
    lv_peripherals_create(group, &ui_evt_handlers);
}

static void peripherals_app_stop(void)
{
    lv_peripherals_remove();
}

static application_t app = {
    .name = "Peripherals",
    .icon = &zephyr_icon_round,
    .start_func = peripherals_app_start,
    .stop_func = peripherals_app_stop
};

static int peripherals_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

static void close_button_handler(lv_peripherals_page_t page)
{
    if (LV_PERIPHERALS_PAGE_LIST == page) {
        zsw_app_manager_app_close_request(&app);
    } else if (LV_PERIPHERALS_PAGE_INFO == page) {
        set_page(LV_PERIPHERALS_PAGE_LIST);
    } else if (LV_PERIPHERALS_PAGE_SCAN == page) {
        ble_comm_central_scan_stop();
        //lv_peripherals_clear_discovered_peripherals_list();
        // TODO: connect
        set_page(LV_PERIPHERALS_PAGE_INFO);
    }
}

static void peripheral_selected_handler(lv_peripherals_type_t peripheral_type)
{
    selected_peripheral = peripheral_type;
    set_page(LV_PERIPHERALS_PAGE_INFO);
}

static void scan_start_handler(void)
{
    ble_comm_central_periph_type_t periph_type = lv_peripherals_to_ble_comm_peripherals_type(selected_peripheral);

    if (0 != ble_comm_central_scan_start(periph_type, on_new_peripheral_discovered_handler, on_peripheral_connected_handler)) {
        LOG_ERR("Start of BLE scanning failed.");
        return;
    }
    set_page(LV_PERIPHERALS_PAGE_SCAN);
}

static void peripheral_remove_handler(void)
{
    ble_comm_central_periph_type_t peripheral_type = lv_peripherals_to_ble_comm_peripherals_type(selected_peripheral);
    ble_comm_central_disconnect(peripheral_type);
    lv_peripherals_update_info_page(&default_info);
}

static void discovered_peripheral_clicked_handler(uint32_t index)
{
    LOG_INF("Try to connect to peripheral %d", index);
    ble_comm_central_periph_type_t periph_type = lv_peripherals_to_ble_comm_peripherals_type(selected_peripheral);
    ble_comm_central_scan_stop();
    ble_comm_central_connect(periph_type, (size_t)index);
}

static void on_new_peripheral_discovered_handler(const char *name)
{
    const char unknown_txt[] = "Unknown";
    const char* actual_name = unknown_txt;
    if (strlen(name) > 0) {
        actual_name = name;
    }
    LOG_INF("New peripheral discovered: %s", actual_name);
    lv_peripherals_add_discovered_peripheral(actual_name);
}

static void on_peripheral_connected_handler(const char *name, ble_comm_central_periph_type_t periph_type, ble_comm_central_periph_status_t periph_status)
{
    set_page(LV_PERIPHERALS_PAGE_INFO);
}

static ble_comm_central_periph_type_t lv_peripherals_to_ble_comm_peripherals_type(lv_peripherals_type_t periph_type)
{
    static const ble_comm_central_periph_type_t ble_comm_periph_type_map[LV_PERIPHERALS_TYPE_LAST] = {
        [LV_PERIPHERALS_TYPE_HEART_RATE_MONITOR] = BLE_COMM_CENTRAL_PERIPH_TYPE_HEART_RATE_MONITOR,
        [LV_PERIPHERALS_TYPE_CYCLING_CANDENCE_SENSOR] = BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_CADENCE_SENSOR,
        [LV_PERIPHERALS_TYPE_CYCLING_SPEED_SENSOR] = BLE_COMM_CENTRAL_PERIPH_TYPE_CYCLING_SPEED_SENSOR
    };
    return ble_comm_periph_type_map[periph_type];
}

static void set_page(lv_peripherals_page_t next_page)
{
    if (LV_PERIPHERALS_PAGE_LAST == next_page) {
        return;
    }

    if (LV_PERIPHERALS_PAGE_LIST == next_page) {

    } else if (LV_PERIPHERALS_PAGE_INFO == next_page) {
        char peripheral_name[31];
        if (0 == ble_comm_central_get_info(selected_peripheral, peripheral_name)) {
            lv_peripheral_info_t info = {
                .peripheral_type = selected_peripheral,
                .peripheral_name = peripheral_name,
                .status = LV_PERIPHERALS_STATUS_BONDED_CONNECTED
            };
            lv_peripherals_update_info_page(&info);
        } else {
            lv_peripherals_update_info_page(&default_info);
        }
    } else if (LV_PERIPHERALS_PAGE_SCAN == next_page) {
        lv_peripherals_clear_discovered_peripherals_list();
    }

    lv_peripherals_set_page(next_page);
}

SYS_INIT(peripherals_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);