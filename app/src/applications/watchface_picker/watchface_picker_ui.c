#include <zephyr/kernel.h>
#include <zephyr/init.h>

#include "watchface_picker_ui.h"
#include "ui/utils/zsw_ui_utils.h"

static lv_obj_t *ui_faceSelect;
static on_watchface_selected_cb_t watchface_selected_cb;

void on_watchface_selected(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    int index = (int)lv_event_get_user_data(e);

    if (event_code == LV_EVENT_CLICKED) {
        printk("Selected watchface %d\n", index);
        watchface_selected_cb(index);
    }
}

void watchface_picker_ui_add_watchface(const lv_img_dsc_t *src, int index)
{
    lv_obj_t *ui_faceItem = lv_obj_create(ui_faceSelect);
    lv_obj_set_width( ui_faceItem, 160);
    lv_obj_set_height( ui_faceItem, 180);
    lv_obj_set_align( ui_faceItem, LV_ALIGN_CENTER );
    lv_obj_clear_flag( ui_faceItem, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_radius(ui_faceItem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_faceItem, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_faceItem, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_faceItem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(ui_faceItem, lv_color_hex(0x142ABC), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_outline_opa(ui_faceItem, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui_faceItem, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(ui_faceItem, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_faceItem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_faceItem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_faceItem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_faceItem, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_facePreview = lv_img_create(ui_faceItem);
    lv_img_set_src(ui_facePreview, src);
    lv_obj_set_width( ui_facePreview, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height( ui_facePreview, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_align( ui_facePreview, LV_ALIGN_TOP_MID );
    lv_obj_add_flag( ui_facePreview, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
    lv_obj_clear_flag( ui_facePreview, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

    lv_obj_t *ui_faceLabel = lv_label_create(ui_faceItem);
    lv_obj_set_width( ui_faceLabel, 160);
    lv_obj_set_height( ui_faceLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_align( ui_faceLabel, LV_ALIGN_BOTTOM_MID );
    lv_label_set_long_mode(ui_faceLabel, LV_LABEL_LONG_DOT);
    lv_label_set_text_fmt(ui_faceLabel, "#%02d", index);
    lv_obj_set_style_text_align(ui_faceLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_faceLabel, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_faceItem, on_watchface_selected, LV_EVENT_ALL, (void *)index);

}

void watchface_picker_ui_show(lv_obj_t *root, on_watchface_selected_cb_t select_cb)
{
    watchface_selected_cb = select_cb;
    ui_faceSelect = lv_obj_create(root);
    lv_obj_set_width( ui_faceSelect, 240);
    lv_obj_set_height( ui_faceSelect, 240);
    lv_obj_set_align( ui_faceSelect, LV_ALIGN_CENTER );
    lv_obj_set_flex_flow(ui_faceSelect, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_faceSelect, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag( ui_faceSelect, LV_OBJ_FLAG_SNAPPABLE );    /// Flags
    lv_obj_set_scrollbar_mode(ui_faceSelect, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(ui_faceSelect, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_faceSelect, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_faceSelect, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_faceSelect, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_faceSelect, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_faceSelect, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_faceSelect, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_faceSelect, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_faceSelect, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_faceSelect, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void watchface_picker_ui_remove(void)
{
    lv_obj_del(ui_faceSelect);
}
