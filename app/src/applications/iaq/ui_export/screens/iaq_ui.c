#include "../iaq_ui.h"

static lv_obj_t *ui_Panel1;
static lv_obj_t *ui_Label1;
static lv_obj_t *ui_LabelAQI;
static lv_obj_t *ui_LabelQuality;
static lv_obj_t *ui_ImgSocket;
static lv_obj_t *ui_ImgScale;
static lv_obj_t *ui_ImgCursor;
static lv_obj_t *ui_root_page = NULL;

void iaq_app_ui_show(lv_obj_t *p_parent)
{
    assert(ui_root_page == NULL);

    ui_root_page = lv_obj_create(p_parent);
    lv_obj_set_style_border_width(ui_root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(ui_root_page, LV_PCT(100), LV_PCT(100));

    lv_obj_clear_flag(ui_root_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_root_page, lv_color_hex(0x0061FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Panel1 = lv_obj_create(ui_root_page);
    lv_obj_set_width(ui_Panel1, 75);
    lv_obj_set_height(ui_Panel1, 50);
    lv_obj_set_x(ui_Panel1, 0);
    lv_obj_set_y(ui_Panel1, 24);
    lv_obj_set_align(ui_Panel1, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Panel1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Panel1, lv_color_hex(0x7F7F00), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Panel1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_Panel1, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Panel1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Panel1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_Panel1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_Panel1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_Panel1, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_Panel1, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label1 = lv_label_create(ui_Panel1);
    lv_obj_set_width(ui_Label1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_Label1, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_Label1, "IAQ");
    lv_obj_set_style_text_color(ui_Label1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LabelAQI = lv_label_create(ui_Panel1);
    lv_obj_set_width(ui_LabelAQI, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LabelAQI, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_LabelAQI, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_LabelAQI, "-");
    lv_obj_set_style_text_color(ui_LabelAQI, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LabelQuality = lv_label_create(ui_root_page);
    lv_obj_set_width(ui_LabelQuality, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LabelQuality, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LabelQuality, 0);
    lv_obj_set_y(ui_LabelQuality, 64);
    lv_obj_set_align(ui_LabelQuality, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LabelQuality, "-");
    lv_obj_set_style_text_color(ui_LabelQuality, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LabelQuality, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ImgSocket = lv_img_create(ui_root_page);

#ifdef CONFIG_LV_Z_USE_FILESYSTEM
    lv_img_set_src(ui_ImgSocket, "S:iaq_socket.bin");
#else
    LV_IMG_DECLARE(iaq_img_socket_png);
    lv_img_set_src(ui_ImgSocket, &iaq_img_socket_png);
#endif

    lv_obj_set_width(ui_ImgSocket, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgSocket, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_ImgSocket, 0);
    lv_obj_set_y(ui_ImgSocket, -22);
    lv_obj_set_align(ui_ImgSocket, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_ImgSocket, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgSocket, LV_OBJ_FLAG_SCROLLABLE);

    ui_ImgScale = lv_img_create(ui_root_page);

#ifdef CONFIG_LV_Z_USE_FILESYSTEM
    lv_img_set_src(ui_ImgScale, "S:iaq_scale.bin");
#else
    LV_IMG_DECLARE(iaq_img_scale_png);
    lv_img_set_src(ui_ImgScale, &iaq_img_scale_png);
#endif

    lv_obj_set_width(ui_ImgScale, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgScale, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_ImgScale, 0);
    lv_obj_set_y(ui_ImgScale, -60);
    lv_obj_set_align(ui_ImgScale, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_ImgScale, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgScale, LV_OBJ_FLAG_SCROLLABLE);

    ui_ImgCursor = lv_img_create(ui_root_page);

    /* TODO: Fix strange rotation bug with cursor (cursor disapears) when using file system
    #ifdef CONFIG_LV_Z_USE_FILESYSTEM
        lv_img_set_src(ui_ImgCursor, "S:iaq_cursor.bin");
    #else
        LV_IMG_DECLARE(iaq_img_cursor_png);
        lv_img_set_src(ui_ImgCursor, &iaq_img_cursor_png);
    #endif
    */

    LV_IMG_DECLARE(iaq_img_cursor_png);
    lv_img_set_src(ui_ImgCursor, &iaq_img_cursor_png);

    lv_obj_set_width(ui_ImgCursor, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgCursor, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_ImgCursor, 0);
    lv_obj_set_y(ui_ImgCursor, -52);
    lv_obj_set_align(ui_ImgCursor, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_ImgCursor, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgCursor, LV_OBJ_FLAG_SCROLLABLE);
    lv_img_set_pivot(ui_ImgCursor, 7, 63);
    lv_img_set_angle(ui_ImgCursor, 0);
}

void iaq_app_ui_remove(void)
{
    lv_obj_del(ui_root_page);
    ui_root_page = NULL;
}

void iaq_app_ui_home_set_iaq_cursor(float iaq)
{
    int16_t angle;

    // Convert the IAQ value into an angle. Use the table from
    //  https://community.bosch-sensortec.com/t5/MEMS-sensors-forum/BME680-688-IAQ-meaning/td-p/45196
    // as reference.
    // 0        -> -900
    // >351     -> +900
    //  y = mx + b
    //      b = -900
    //      m = 5.12 -> 5
    // Use 5 to store everything in an int16_t. The error is 0.45° With IAQ 351
    angle = 5 * (int16_t)iaq - 900;

    lv_img_set_angle(ui_ImgCursor, angle);
}

void iaq_app_ui_home_set_iaq_label(float iaq)
{
    lv_label_set_text_fmt(ui_LabelAQI, "%g", iaq);
}

void iaq_app_ui_home_set_iaq_status(float iaq)
{
    const char *text;
    lv_color_t color;

    if (iaq < 50) {
        text = "Excellent";
        color = lv_color_hex(0x00FF00);
    } else if (iaq < 100) {
        text = "Good";
        color = lv_color_hex(0x4CA200);
    } else if (iaq < 150) {
        text = "Lightly polluted";
        color = lv_color_hex(0xECF500);
    } else if (iaq < 200) {
        text = "Moderately polluted";
        color = lv_color_hex(0xE59900);
    } else if (iaq < 250) {
        text = "Heavily polluted";
        color = lv_color_hex(0xFF0000);
    } else if (iaq < 350) {
        text = "Severely polluted";
        color = lv_color_hex(0xA8009D);
    } else {
        text = "Extremely polluted";
        color = lv_color_hex(0x994C00);
    }

    lv_label_set_text(ui_LabelQuality, text);
    lv_obj_set_style_bg_color(ui_Panel1, color, LV_PART_MAIN | LV_STATE_DEFAULT);
}