#pragma once

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

typedef enum {
    LV_SETTINGS_TYPE_SWITCH,
    LV_SETTINGS_TYPE_SLIDER,
    LV_SETTINGS_TYPE_LABEL,
    LV_SETTINGS_TYPE_LIST_BTN,
    LV_SETTINGS_TYPE_BTN,
    LV_SETTINGS_TYPE_DDLIST,
    LV_SETTINGS_TYPE_NUMSET,
    LV_SETTINGS_TYPE_INV = 0xff,
} lv_settings_type_t;

typedef void(*on_close_cb_t)(void);

typedef struct lv_setting_value {
    lv_settings_type_t type;
    union {
        bool    sw;
        int32_t slider;
    } item;

} lv_setting_value_t;

/*
* Final means the user made the change and unfocused the element.
* For example in a switch, while dragging the "dot" final will be false,
* but when the user release the switch, final will be true.
*/
typedef void(*lv_settings_changed_cb_t)(lv_setting_value_t value, bool final);

typedef struct {
    const char *name;
} lv_settings_label_t;

typedef struct {
    const char *name;
    const char *text;
} lv_settings_button_t;

typedef struct {
    const char  *name;
    bool        *inital_val;
} lv_settings_switch_t;

typedef struct {
    const char *name;
    int32_t     *inital_val;
    int32_t     min_val;
    int32_t     max_val;
} lv_settings_slider_t;

typedef struct lv_settings_item {
    lv_settings_type_t type;
    union {
        lv_settings_label_t     label;
        lv_settings_button_t    btn;
        lv_settings_switch_t    sw;
        lv_settings_slider_t    slider;
    } item;
    const char *icon;
    lv_settings_changed_cb_t    change_callback;
} lv_settings_item_t;

typedef struct lv_settings_page {
    const char         *name;
    on_close_cb_t       closed_cb;
    uint8_t             num_items;
    lv_settings_item_t *items;
} lv_settings_page_t;

/**
 * Create a settings application
 * @param root_item descriptor of the settings button. For example:
 * `lv_settings_menu_item_t root_item = {.name = "Settings", .event_cb = root_event_cb};`
 * @return the created settings button
 */
void lv_settings_create(lv_obj_t *root, lv_settings_page_t *pages, uint8_t num_pages, const char *title,
                        lv_group_t *input_group,
                        on_close_cb_t close_cb);

void settings_ui_remove(void);