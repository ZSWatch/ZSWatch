#include <info/info_ui.h>
#include <lvgl.h>
#include <utils/zsw_ui_utils.h>
#include <zsw_coredump.h>
#include "assert.h"

static void seconds_to_time_chunks(uint32_t time_seconds, int *days, int *hours, int *minutes, int *seconds);

ZSW_LV_IMG_DECLARE(ui_img_debugging_png);

static lv_obj_t *root_page = NULL;
static on_reset_ui_event_cb_t reset_callback;
static zsw_coredump_sumary_t cached_coredumps[ZSW_COREDUMP_MAX_STORED];
static int num_cached_coredumps;

// Page 1
static lv_obj_t *uptime_label;
static lv_obj_t *resets_label;
static lv_obj_t *runtime_label;
static lv_obj_t *wake_time_label;
static lv_obj_t *disp_pwr_off_label;
static lv_obj_t *inactive_time_label;

// Page 2
static lv_obj_t *mac_addr_label;
static lv_obj_t *ble_remote_addr_label;
static lv_obj_t *ble_remote_info_label;
static lv_obj_t *ble_gatt_status_label;
static lv_obj_t *ble_security_status_label;

// Page 3
static lv_obj_t *ui_Image1;
static lv_obj_t *ui_title;
static lv_obj_t *ui_coredump_list;
static lv_obj_t *ui_coredump_info_field;
static lv_obj_t *ui_Container2;
static lv_obj_t *ui_download;
static lv_obj_t *ui_Label4;
static lv_obj_t *ui_erase;
static lv_obj_t *ui_Label1;
static lv_obj_t *ui_create;
static lv_obj_t *ui_Label2;
static lv_obj_t *ui_Dropdown1;

static void reset_btn_pressed(lv_event_t *e)
{
    if (reset_callback) {
        reset_callback();
    }
}

static void create_page_info_ui(lv_obj_t *parent)
{
    lv_obj_t *temp_obj;
    lv_obj_t *title_label = lv_label_create(parent);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_18, 0);
    lv_label_set_text(title_label, "System info");
    lv_obj_set_style_text_decor(title_label, LV_TEXT_DECOR_UNDERLINE, LV_PART_MAIN);
    lv_obj_align_to(title_label, parent, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *note_label = lv_label_create(parent);
    lv_label_set_text(note_label, "(Since power loss)");
    lv_obj_align_to(note_label, title_label, LV_ALIGN_BOTTOM_MID, 0, 20);

    resets_label = lv_label_create(parent);
    lv_obj_set_width(resets_label, LV_SIZE_CONTENT);
    lv_obj_set_height(resets_label, LV_SIZE_CONTENT);
    lv_obj_set_x(resets_label, 15);
    lv_obj_set_y(resets_label, -45);
    lv_obj_set_align(resets_label, LV_ALIGN_LEFT_MID);

    uptime_label = lv_label_create(parent);
    lv_obj_set_width(uptime_label, LV_SIZE_CONTENT);
    lv_obj_set_height(uptime_label, LV_SIZE_CONTENT);
    lv_obj_set_x(uptime_label, 10);
    lv_obj_set_y(uptime_label, -25);
    lv_obj_set_align(uptime_label, LV_ALIGN_LEFT_MID);

    runtime_label = lv_label_create(parent);
    lv_obj_set_width(runtime_label, LV_SIZE_CONTENT);
    lv_obj_set_height(runtime_label, LV_SIZE_CONTENT);
    lv_obj_set_x(runtime_label, 5);
    lv_obj_set_y(runtime_label, -5);
    lv_obj_set_align(runtime_label, LV_ALIGN_LEFT_MID);

    wake_time_label = lv_label_create(parent);
    lv_obj_set_width(wake_time_label, LV_SIZE_CONTENT);
    lv_obj_set_height(wake_time_label, LV_SIZE_CONTENT);
    lv_obj_set_x(wake_time_label, 5);
    lv_obj_set_y(wake_time_label, 15);
    lv_obj_set_align(wake_time_label, LV_ALIGN_LEFT_MID);

    disp_pwr_off_label = lv_label_create(parent);
    lv_obj_set_width(disp_pwr_off_label, LV_SIZE_CONTENT);
    lv_obj_set_height(disp_pwr_off_label, LV_SIZE_CONTENT);
    lv_obj_set_x(disp_pwr_off_label, 10);
    lv_obj_set_y(disp_pwr_off_label, 35);
    lv_obj_set_align(disp_pwr_off_label, LV_ALIGN_LEFT_MID);

    inactive_time_label = lv_label_create(parent);
    lv_obj_set_width(inactive_time_label, LV_SIZE_CONTENT);
    lv_obj_set_height(inactive_time_label, LV_SIZE_CONTENT);
    lv_obj_set_x(inactive_time_label, 20);
    lv_obj_set_y(inactive_time_label, 50);
    lv_obj_set_align(inactive_time_label, LV_ALIGN_LEFT_MID);

    temp_obj = lv_btn_create(parent);
    lv_obj_set_width(temp_obj, 100);
    lv_obj_set_height(temp_obj, 30);
    lv_obj_set_x(temp_obj, 0);
    lv_obj_set_y(temp_obj, 0);
    lv_obj_set_align(temp_obj, LV_ALIGN_BOTTOM_MID);
    lv_obj_add_event_cb(temp_obj, reset_btn_pressed, LV_EVENT_CLICKED, NULL);

    temp_obj = lv_label_create(temp_obj);
    lv_obj_set_width(temp_obj, LV_SIZE_CONTENT);
    lv_obj_set_height(temp_obj, LV_SIZE_CONTENT);
    lv_obj_set_align(temp_obj, LV_ALIGN_CENTER);
    lv_label_set_text(temp_obj, "Reset");
}

static void create_page_ble_ui(lv_obj_t *parent)
{
    lv_obj_t *title_label = lv_label_create(parent);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_16, 0);
    lv_label_set_text(title_label, "Bluetooth Local");
    lv_obj_set_style_text_decor(title_label, LV_TEXT_DECOR_UNDERLINE, LV_PART_MAIN);
    lv_obj_align_to(title_label, parent, LV_ALIGN_TOP_MID, 0, 20);

    mac_addr_label = lv_label_create(parent);
    lv_label_set_text(mac_addr_label, "00:00:00:00:00:00 (random)");
    lv_obj_align_to(mac_addr_label, title_label, LV_ALIGN_BOTTOM_MID, 0, 15);

    title_label = lv_label_create(parent);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_16, 0);
    lv_label_set_text(title_label, "Remote MAC");
    lv_obj_set_style_text_decor(title_label, LV_TEXT_DECOR_UNDERLINE, LV_PART_MAIN);
    lv_obj_align_to(title_label, mac_addr_label, LV_ALIGN_BOTTOM_MID, 0, 15);

    ble_remote_addr_label = lv_label_create(parent);
    lv_label_set_text(ble_remote_addr_label, "00:00:00:00:00:00 (random)");
    lv_obj_align_to(ble_remote_addr_label, title_label, LV_ALIGN_BOTTOM_MID, 0, 15);

    ble_remote_info_label = lv_label_create(parent);
    lv_label_set_text(ble_remote_info_label, "Interval: -ms, Timeout: -ms");
    lv_obj_align_to(ble_remote_info_label, ble_remote_addr_label, LV_ALIGN_BOTTOM_MID, 0, 15);

    ble_security_status_label = lv_label_create(parent);
    lv_label_set_text(ble_security_status_label, "Security: 0");
    lv_obj_align_to(ble_security_status_label, ble_remote_info_label, LV_ALIGN_BOTTOM_MID, 0, 15);

    ble_gatt_status_label = lv_label_create(parent);
    lv_label_set_text(ble_gatt_status_label, "Not. enabled: - MTU: -");
    lv_obj_align_to(ble_gatt_status_label, ble_security_status_label, LV_ALIGN_BOTTOM_MID, 0, 15);
}

static void on_coredump_button_pressed(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    if (btn == ui_download) {
        zsw_coredump_to_log();
    } else if (btn == ui_erase) {
        lv_dropdown_clear_options(ui_Dropdown1); // TODO Handle when more than one
        lv_label_set_text(ui_coredump_info_field, "No Coredumps :)");
        zsw_coredump_erase(0);
    } else if (btn == ui_create) {
        __ASSERT(0, "User created ASSERT");
    }
}

static void coredump_dropdown_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *dropdown = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        int index = lv_dropdown_get_selected(dropdown);
        __ASSERT(index < num_cached_coredumps, "Invalid dropdown coredump index");
        lv_label_set_text_fmt(ui_coredump_info_field, "%s\n%s Line #%d", cached_coredumps[index].datetime,
                              cached_coredumps[index].file, cached_coredumps[index].line);
    }
}

static void create_coredump_page_ui(lv_obj_t *parent, zsw_coredump_sumary_t *coredumps, int num_coredumps)
{
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(parent, 0, 0);
    ui_Image1 = lv_img_create(parent);
    lv_img_set_src(ui_Image1, ZSW_LV_IMG_USE(ui_img_debugging_png));
    lv_obj_set_width(ui_Image1, LV_SIZE_CONTENT);   /// 32
    lv_obj_set_height(ui_Image1, LV_SIZE_CONTENT);    /// 32
    lv_obj_set_x(ui_Image1, 54);
    lv_obj_set_y(ui_Image1, -89);
    lv_obj_set_align(ui_Image1, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image1, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_title = lv_label_create(parent);
    lv_obj_set_width(ui_title, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_title, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_title, -16);
    lv_obj_set_y(ui_title, 17);
    lv_obj_set_align(ui_title, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_title, "Coredump");
    lv_obj_set_style_text_font(ui_title, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_coredump_list = lv_obj_create(parent);
    lv_obj_remove_style_all(ui_coredump_list);
    lv_obj_set_width(ui_coredump_list, lv_pct(80));
    lv_obj_set_height(ui_coredump_list, lv_pct(34));
    lv_obj_set_x(ui_coredump_list, 0);
    lv_obj_set_y(ui_coredump_list, 20);
    lv_obj_set_align(ui_coredump_list, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_coredump_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_coredump_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_coredump_list, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_coredump_list, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_coredump_list, lv_color_hex(0x495060), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_coredump_list, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_coredump_list, lv_color_hex(0x293031), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_coredump_list, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_coredump_list, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_coredump_list, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_coredump_list, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_coredump_list, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_coredump_list, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_coredump_info_field = lv_label_create(ui_coredump_list);
    lv_obj_set_width(ui_coredump_info_field, lv_pct(100));
    lv_obj_set_height(ui_coredump_info_field, LV_SIZE_CONTENT);    /// 100
    lv_obj_set_x(ui_coredump_info_field, -46);
    lv_obj_set_y(ui_coredump_info_field, -34);
    lv_obj_set_align(ui_coredump_info_field, LV_ALIGN_CENTER);
    lv_label_set_text(ui_coredump_info_field, "");

    ui_Container2 = lv_obj_create(parent);
    lv_obj_remove_style_all(ui_Container2);
    lv_obj_set_height(ui_Container2, 45);
    lv_obj_set_width(ui_Container2, lv_pct(65));
    lv_obj_set_x(ui_Container2, 0);
    lv_obj_set_y(ui_Container2, 85);
    lv_obj_set_align(ui_Container2, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_Container2, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_Container2, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_Container2, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_pad_top(ui_Container2, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_Container2, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_download = lv_btn_create(ui_Container2);
    lv_obj_set_width(ui_download, 40);
    lv_obj_set_height(ui_download, 30);
    lv_obj_set_align(ui_download, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_download, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_download, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_download, lv_color_hex(0x9EC8F6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_download, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_download, on_coredump_button_pressed, LV_EVENT_CLICKED, NULL);

    ui_Label4 = lv_label_create(ui_download);
    lv_obj_set_width(ui_Label4, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label4, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label4, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label4, LV_SYMBOL_DOWNLOAD);

    ui_erase = lv_btn_create(ui_Container2);
    lv_obj_set_width(ui_erase, 40);
    lv_obj_set_height(ui_erase, 30);
    lv_obj_set_align(ui_erase, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_erase, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_erase, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_erase, lv_color_hex(0xC89A2F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_erase, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_erase, on_coredump_button_pressed, LV_EVENT_CLICKED, NULL);

    ui_Label1 = lv_label_create(ui_erase);
    lv_obj_set_width(ui_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label1, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label1, LV_SYMBOL_TRASH);

    ui_create = lv_btn_create(ui_Container2);
    lv_obj_set_width(ui_create, 40);
    lv_obj_set_height(ui_create, 30);
    lv_obj_set_align(ui_create, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_create, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_create, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_create, lv_color_hex(0xF60505), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_create, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_create, on_coredump_button_pressed, LV_EVENT_CLICKED, NULL);

    ui_Label2 = lv_label_create(ui_create);
    lv_obj_set_width(ui_Label2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label2, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label2, LV_SYMBOL_WARNING);

    ui_Dropdown1 = lv_dropdown_create(parent);
    lv_dropdown_clear_options(ui_Dropdown1);
    for (int i = 0; i < num_coredumps; i++) {
        lv_dropdown_add_option(ui_Dropdown1, coredumps[i].datetime, i);
    }
    if (num_coredumps > 0) {
        lv_dropdown_set_selected(ui_Dropdown1, 0);
        lv_label_set_text_fmt(ui_coredump_info_field, "%s\n%s Line #%d", coredumps[0].datetime, coredumps[0].file,
                              coredumps[0].line);
    }

    lv_obj_add_event_cb(ui_Dropdown1, coredump_dropdown_handler, LV_EVENT_ALL, NULL);
    lv_obj_set_width(ui_Dropdown1, lv_pct(80));
    lv_obj_set_height(ui_Dropdown1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Dropdown1, 0);
    lv_obj_set_y(ui_Dropdown1, -50);
    lv_obj_set_align(ui_Dropdown1, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Dropdown1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_set_style_bg_color(ui_Dropdown1, lv_color_hex(0x495060), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Dropdown1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void info_ui_show(lv_obj_t *root, on_reset_ui_event_cb_t reset_cb, zsw_coredump_sumary_t *coredumps, int num_coredumps)
{
    assert(root_page == NULL);

    reset_callback = reset_cb;
    // Create the root container
    root_page = lv_obj_create(root);
    // Remove the default border
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    // Make root container fill the screen
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, 0);
    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *tv = lv_tileview_create(root_page);
    lv_obj_set_size(tv, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(tv, LV_OPA_TRANSP, 0);
    // Remove scroolbar on tv
    lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

    num_cached_coredumps = num_coredumps;
    memcpy(cached_coredumps, coredumps, num_coredumps * sizeof(zsw_coredump_sumary_t));

    create_coredump_page_ui(lv_tileview_add_tile(tv, 0, 0, LV_DIR_BOTTOM), cached_coredumps, num_cached_coredumps);
    create_page_info_ui(lv_tileview_add_tile(tv, 0, 1, LV_DIR_VER));
    create_page_ble_ui(lv_tileview_add_tile(tv, 0, 2, LV_DIR_TOP));

    lv_obj_t *scroll_icon = lv_img_create(root_page);
    lv_img_set_src(scroll_icon, "S:scroll_icon.bin");
    lv_obj_align(scroll_icon, LV_ALIGN_BOTTOM_RIGHT, -17, -23);
}

void info_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}

void info_ui_set_uptime_sec(uint32_t uptime_seconds)
{
    int days;
    int hours;
    int minutes;
    int seconds;

    seconds_to_time_chunks(uptime_seconds, &days, &hours, &minutes, &seconds);
    lv_label_set_text_fmt(uptime_label, "Uptime: %02d:%02d:%02d:%02d", days, hours, minutes, seconds);
}

void info_ui_set_resets(uint32_t resets)
{
    lv_label_set_text_fmt(resets_label, "Resets:\t%d", resets);
}

void info_ui_set_total_uptime_sec(uint32_t uptime_seconds)
{
    int days;
    int hours;
    int minutes;
    int seconds;

    seconds_to_time_chunks(uptime_seconds, &days, &hours, &minutes, &seconds);
    lv_label_set_text_fmt(runtime_label, "Total uptime: %02d:%02d:%02d:%02d", days, hours, minutes, seconds);
}

void info_ui_set_wakeup_time_sec(uint64_t total_wake_time, uint32_t percent_used)
{
    int days;
    int hours;
    int minutes;
    int seconds;

    seconds_to_time_chunks(total_wake_time, &days, &hours, &minutes, &seconds);
    lv_label_set_text_fmt(wake_time_label, "Screen on: %02d:%02d:%02d:%02d (%d%%)", days, hours, minutes, seconds,
                          percent_used);
}

void info_ui_set_ref_off_time_sec(uint64_t total_off_time, uint32_t percent_off)
{
    int days;
    int hours;
    int minutes;
    int seconds;

    seconds_to_time_chunks(total_off_time, &days, &hours, &minutes, &seconds);
    lv_label_set_text_fmt(disp_pwr_off_label, "Stationary: %02d:%02d:%02d:%02d (%d%%)", days, hours, minutes, seconds,
                          percent_off);
}

void info_ui_set_time_to_inactive_sec(uint32_t time_left_seconds)
{
    lv_label_set_text_fmt(inactive_time_label, "Time to inactive: %ds", time_left_seconds);
}

void info_ui_set_mac_addr(char *mac_str)
{
    lv_label_set_text(mac_addr_label, mac_str);
}

void info_ui_set_gatt_status(bool cccd_enabled, uint16_t mtu)
{
    lv_label_set_text_fmt(ble_gatt_status_label, "Not. enabled: %d MTU: %d", cccd_enabled, mtu);
}

void info_app_ui_set_conn_mac(char *mac)
{
    lv_label_set_text(ble_remote_addr_label, mac);
}

void info_app_ui_set_conn_params(uint16_t interval, uint16_t latency, uint16_t timeout)
{
    lv_label_set_text_fmt(ble_remote_info_label, "Int: %dms, Tmo: %dms", interval, timeout);
}

void info_app_ui_set_conn_security_info(int info, int err)
{
    char msg[100];

    if (!err) {
        snprintf(msg, sizeof(msg) - 1, "Security level: %d", info);
    } else {
        snprintf(msg, sizeof(msg) - 1, "Security failed: level %u err %d", info, err);
    }
    lv_label_set_text(ble_security_status_label, msg);
}

static void seconds_to_time_chunks(uint32_t time_seconds, int *days, int *hours, int *minutes, int *seconds)
{
    int n = time_seconds;
    *days = n / (24 * 3600);

    n = n % (24 * 3600);
    *hours = n / 3600;

    n %= 3600;
    *minutes = n / 60 ;

    n %= 60;
    *seconds = n;
}