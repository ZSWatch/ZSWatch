

#include "../chronos_ui.h"
#include "ui/utils/zsw_ui_utils.h"

ZSW_LV_IMG_DECLARE(chronos_arrow_icon);


#if LV_USE_CANVAS == 1
#define CANVAS_WIDTH 48
#define CANVAS_HEIGHT 48
static lv_color_t cbuf[LV_IMG_BUF_SIZE_INDEXED_1BIT(CANVAS_WIDTH, CANVAS_HEIGHT)];
#endif


static lv_obj_t *ui_navText;
static lv_obj_t *ui_navIconCanvas;
static lv_obj_t *ui_navIcon;
static lv_obj_t *ui_navDistance;
static lv_obj_t *ui_navDirection;

void chronos_ui_navigation_init(lv_obj_t *page)
{

    chronos_ui_add_app_title(page, "Navigation", ZSW_LV_IMG_USE(chronos_arrow_icon));

    lv_obj_set_style_pad_top(page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(page, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

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
    lv_obj_set_style_text_font(ui_navText, CHRONOS_FONT_20, LV_PART_MAIN | LV_STATE_DEFAULT);

#if LV_USE_CANVAS == 1
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
#else
    ui_navIconCanvas = lv_obj_create(ui_navPanel);
    lv_obj_remove_style_all(ui_navIconCanvas);
    lv_obj_set_width(ui_navIconCanvas, 48);
    lv_obj_set_height(ui_navIconCanvas, 48);
#endif

    ui_navIcon = lv_img_create(ui_navPanel);
    lv_img_set_src(ui_navIcon, ZSW_LV_IMG_USE(chronos_arrow_icon));
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
    lv_obj_set_style_text_font(ui_navDistance, CHRONOS_FONT_30, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_navDirection = lv_label_create(ui_navPanel);
    lv_obj_set_width(ui_navDirection, 180);
    lv_obj_set_height(ui_navDirection, 40);
    lv_obj_set_align(ui_navDirection, LV_ALIGN_CENTER);
    lv_label_set_text(ui_navDirection, "Start Navigation on Google Maps ");
    lv_obj_set_style_text_align(ui_navDirection, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_navDirection, CHRONOS_FONT_16, LV_PART_MAIN | LV_STATE_DEFAULT);


}

void chronos_ui_set_nav_info(const char *text, const char *title, const char *directions)
{
    lv_label_set_text(ui_navText, text);
    lv_label_set_text(ui_navDirection, directions);
    lv_label_set_text(ui_navDistance, title);
}

void chronos_ui_set_nav_icon_state(bool show)

{
    if (show) {
        lv_obj_clear_flag(ui_navIconCanvas, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_navIcon, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(ui_navIconCanvas, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_navIcon, LV_OBJ_FLAG_HIDDEN);
    }
}

void chronos_ui_set_nav_icon_px(uint16_t x, uint16_t y, bool on)
{
#if LV_USE_CANVAS == 1
    if (on) {
        lv_canvas_set_px_color(ui_navIconCanvas, x, y, lv_color_make(255, 255, 255));
    } else {
        lv_canvas_set_px_color(ui_navIconCanvas, x, y, lv_color_make(0, 0, 0));
    }
#endif
}