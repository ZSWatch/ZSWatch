#include <sensors_summary/sensors_summary_ui.h>
#include <lvgl.h>

static void event_set_reference_button(lv_event_t *e);

static lv_obj_t *root_page = NULL;
static on_ui_close_cb_t close_callback;
static on_reference_set_cb_t ref_set_callback;

static lv_obj_t *set_ref_btn;
static lv_obj_t *ref_btn_text;
static lv_obj_t *pressure_label;
static lv_obj_t *humidity_label;
static lv_obj_t *temp_label;
static lv_obj_t *rel_height_label;
static lv_obj_t *iaq_label;
static lv_obj_t *light_label;
static lv_obj_t *co2_label;

static void event_set_reference_button(lv_event_t *e)
{
    ref_set_callback();
}

static void create_ui(lv_obj_t *parent)
{
    set_ref_btn = lv_btn_create(parent);
    lv_obj_set_width(set_ref_btn, 75);
    lv_obj_set_height(set_ref_btn, 25);
    lv_obj_set_x(set_ref_btn, 0);
    lv_obj_set_y(set_ref_btn, 80);
    lv_obj_set_align(set_ref_btn, LV_ALIGN_CENTER);
    lv_obj_add_flag(set_ref_btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(set_ref_btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_text_font(set_ref_btn, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    ref_btn_text = lv_label_create(set_ref_btn);
    lv_obj_set_width(ref_btn_text, LV_SIZE_CONTENT);
    lv_obj_set_height(ref_btn_text, LV_SIZE_CONTENT);
    lv_obj_set_align(ref_btn_text, LV_ALIGN_CENTER);
    lv_label_set_text(ref_btn_text, "Set ref");
    lv_obj_set_style_text_font(ref_btn_text, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    pressure_label = lv_label_create(parent);
    lv_obj_set_width(pressure_label, LV_SIZE_CONTENT);
    lv_obj_set_height(pressure_label, LV_SIZE_CONTENT);
    lv_obj_set_x(pressure_label, 45);
    lv_obj_set_y(pressure_label, -85);
    lv_obj_set_align(pressure_label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(pressure_label, "Pressure: ");

    humidity_label = lv_label_create(parent);
    lv_obj_set_width(humidity_label, LV_SIZE_CONTENT);
    lv_obj_set_height(humidity_label, LV_SIZE_CONTENT);
    lv_obj_set_x(humidity_label, 30);
    lv_obj_set_y(humidity_label, -65);
    lv_obj_set_align(humidity_label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(humidity_label, "Humidity:");

    temp_label = lv_label_create(parent);
    lv_obj_set_width(temp_label, LV_SIZE_CONTENT);
    lv_obj_set_height(temp_label, LV_SIZE_CONTENT);
    lv_obj_set_x(temp_label, 15);
    lv_obj_set_y(temp_label, -45);
    lv_obj_set_align(temp_label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(temp_label, "Temp:");

    rel_height_label = lv_label_create(parent);
    lv_obj_set_width(rel_height_label, LV_SIZE_CONTENT);
    lv_obj_set_height(rel_height_label, LV_SIZE_CONTENT);
    lv_obj_set_x(rel_height_label, 15);
    lv_obj_set_y(rel_height_label, -25);
    lv_obj_set_align(rel_height_label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(rel_height_label, "Rel. height:");

    iaq_label = lv_label_create(parent);
    lv_obj_set_width(iaq_label, LV_SIZE_CONTENT);
    lv_obj_set_height(iaq_label, LV_SIZE_CONTENT);
    lv_obj_set_x(iaq_label, 5);
    lv_obj_set_y(iaq_label, -5);
    lv_obj_set_align(iaq_label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(iaq_label, "IAQ:");

    light_label = lv_label_create(parent);
    lv_obj_set_width(light_label, LV_SIZE_CONTENT);
    lv_obj_set_height(light_label, LV_SIZE_CONTENT);
    lv_obj_set_x(light_label, 5);
    lv_obj_set_y(light_label, 15);
    lv_obj_set_align(light_label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(light_label, "Light:");

    co2_label = lv_label_create(parent);
    lv_obj_set_width(co2_label, LV_SIZE_CONTENT);
    lv_obj_set_height(co2_label, LV_SIZE_CONTENT);
    lv_obj_set_x(co2_label, 15);
    lv_obj_set_y(co2_label, 35);
    lv_obj_set_align(co2_label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(co2_label, "CO2:");

    lv_obj_add_event_cb(set_ref_btn, event_set_reference_button, LV_EVENT_CLICKED, NULL);
}

void sensors_summary_ui_show(lv_obj_t *root, on_ui_close_cb_t close_cb, on_reference_set_cb_t ref_set_cb)
{
    assert(root_page == NULL);
    close_callback = close_cb;
    ref_set_callback = ref_set_cb;

    // Create the root container
    root_page = lv_obj_create(root);
    // Remove the default border
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    // Make root container fill the screen
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    create_ui(root_page);
}

void sensors_summary_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}

void sensors_summary_ui_set_pressure(float pressure)
{
    lv_label_set_text_fmt(pressure_label, "Pressure:\t%.0f Pa", pressure);
}

void sensors_summary_ui_set_humidity(float humidity)
{
    lv_label_set_text_fmt(humidity_label, "Humidity:\t%.2f %%", humidity);
}

void sensors_summary_ui_set_temp(float temp)
{
    lv_label_set_text_fmt(temp_label, "Temp:\t%.2f C", temp);
}

void sensors_summary_ui_set_rel_height(float rel_height)
{
    lv_label_set_text_fmt(rel_height_label, "Rel. height:\t%.2f m", rel_height);
}

void sensors_summary_ui_set_light(float light)
{
    lv_label_set_text_fmt(light_label, "Light:\t%.2f", light);
}

void sensors_summary_ui_set_iaq(float iaq)
{
    lv_label_set_text_fmt(iaq_label, "IAQ:\t%.2f", iaq);
}

void sensors_summary_ui_set_co2(float co2)
{
    lv_label_set_text_fmt(co2_label, "CO2:\t%.2f ppm", co2);
}