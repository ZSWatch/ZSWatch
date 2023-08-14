#include <info/info_ui.h>
#include <lvgl.h>

static void seconds_to_time_chunks(uint32_t time_seconds, int *days, int *hours, int *minutes, int *seconds);

static lv_obj_t *root_page = NULL;
static on_reset_ui_event_cb_t reset_callback;

static lv_obj_t *uptime_label;
static lv_obj_t *resets_label;
static lv_obj_t *runtime_label;
static lv_obj_t *wake_time_label;
static lv_obj_t *disp_pwr_off_label;
static lv_obj_t *mac_addr_label;
static lv_obj_t *inactive_time_label;

static void reset_btn_pressed(lv_event_t *e)
{
    if (reset_callback) {
        reset_callback();
    }
}

static void create_ui(lv_obj_t *parent)
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

    mac_addr_label = lv_label_create(parent);
    lv_obj_set_width(mac_addr_label, LV_SIZE_CONTENT);
    lv_obj_set_height(mac_addr_label, LV_SIZE_CONTENT);
    lv_obj_set_x(mac_addr_label, 20);
    lv_obj_set_y(mac_addr_label, 50);
    lv_obj_set_align(mac_addr_label, LV_ALIGN_LEFT_MID);

    inactive_time_label = lv_label_create(parent);
    lv_obj_set_width(inactive_time_label, LV_SIZE_CONTENT);
    lv_obj_set_height(inactive_time_label, LV_SIZE_CONTENT);
    lv_obj_set_x(inactive_time_label, 30);
    lv_obj_set_y(inactive_time_label, 65);
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

void info_ui_show(lv_obj_t *root, on_reset_ui_event_cb_t reset_cb)
{
    assert(root_page == NULL);

    reset_callback = reset_cb;
    // Create the root container
    root_page = lv_obj_create(root);
    // Remove the default border
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    // Make root container fill the screen
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));

    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);

    create_ui(root_page);
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