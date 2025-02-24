

#include "../chronos_ui.h"
#include "ui/utils/zsw_ui_utils.h"

ZSW_LV_IMG_DECLARE(image_chronos_icon);

#define CANVAS_WIDTH 48
#define CANVAS_HEIGHT 48


static lv_color_t cbuf[LV_IMG_BUF_SIZE_INDEXED_1BIT(CANVAS_WIDTH, CANVAS_HEIGHT)];

static lv_obj_t *ui_navText;
static lv_obj_t *ui_navIconCanvas;
static lv_obj_t *ui_navIcon;
static lv_obj_t *ui_navDistance;
static lv_obj_t *ui_navDirection;

void navigation_init(lv_obj_t *page)
{

    add_app_title(page, "Navigation", ZSW_LV_IMG_USE(image_chronos_icon));

    lv_obj_t *ui_navPanel = lv_obj_create(page);
    lv_obj_set_width(ui_navPanel, 240);
    lv_obj_set_height(ui_navPanel, 240);
    lv_obj_set_align(ui_navPanel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_navPanel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_navPanel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_navPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_navPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_navPanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_navPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_navText = lv_label_create(ui_navPanel);
    lv_obj_set_width(ui_navText, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_navText, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_navText, LV_ALIGN_CENTER);
    lv_label_set_text(ui_navText, "Navigation");
    lv_obj_set_style_text_align(ui_navText, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_navText, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_navIconCanvas = lv_canvas_create(ui_navPanel);
    lv_canvas_set_buffer(ui_navIconCanvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_INDEXED_1BIT);
    lv_obj_set_width(ui_navIconCanvas, 48);
    lv_obj_set_height(ui_navIconCanvas, 48);
    lv_obj_set_x(ui_navIconCanvas, 0);
    lv_obj_set_y(ui_navIconCanvas, 68);
    lv_obj_set_align(ui_navIconCanvas, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_navIconCanvas, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_radius(ui_navIconCanvas, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_navIconCanvas, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_navIconCanvas, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_navIconCanvas, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_canvas_fill_bg(ui_navIconCanvas, lv_color_black(), LV_OPA_COVER);
    lv_canvas_set_palette(ui_navIconCanvas, 0, lv_color_hex(0x000000));
    lv_canvas_set_palette(ui_navIconCanvas, 1, lv_color_hex(0xFFFFFF));


    ui_navIcon = lv_img_create(ui_navPanel);
    lv_img_set_src(ui_navIcon, ZSW_LV_IMG_USE(image_chronos_icon));
    lv_obj_set_width(ui_navIcon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_navIcon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_navIcon, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_navIcon, LV_OBJ_FLAG_CLICKABLE);     /// Flags
    lv_obj_clear_flag(ui_navIcon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_navDistance = lv_label_create(ui_navPanel);
    lv_obj_set_width(ui_navDistance, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_navDistance, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_navDistance, LV_ALIGN_CENTER);
    lv_label_set_text(ui_navDistance, "Chronos");
    lv_obj_set_style_text_font(ui_navDistance, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_navDirection = lv_label_create(ui_navPanel);
    lv_obj_set_width(ui_navDirection, 180);
    lv_obj_set_height(ui_navDirection, 40);
    lv_obj_set_align(ui_navDirection, LV_ALIGN_CENTER);
    lv_label_set_text(ui_navDirection, "Start Navigation on Google Maps ");
    lv_obj_set_style_text_align(ui_navDirection, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_navDirection, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);


}

void navigateInfo(const char *text, const char *title, const char *directions)
{
    lv_label_set_text(ui_navText, text);
    lv_label_set_text(ui_navDirection, directions);
    lv_label_set_text(ui_navDistance, title);
}

void navIconState(bool show)
{
    if (show) {
        lv_obj_clear_flag(ui_navIconCanvas, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_navIcon, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(ui_navIconCanvas, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_navIcon, LV_OBJ_FLAG_HIDDEN);
    }
}

void setNavIconPx(uint16_t x, uint16_t y, bool on)
{
    if (on) {
        lv_canvas_set_px_color(ui_navIconCanvas, x, y, lv_color_make(255, 255, 255));
    } else {
        lv_canvas_set_px_color(ui_navIconCanvas, x, y, lv_color_make(0, 0, 0));
    }
}