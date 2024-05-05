#include <lvgl.h>

typedef void(*on_watchface_selected_cb_t)(int watchface_index);

void watchface_picker_ui_show(lv_obj_t *root, on_watchface_selected_cb_t select_cb);
void watchface_picker_ui_add_watchface(const lv_img_dsc_t *src, int index);
void watchface_picker_ui_remove(void);