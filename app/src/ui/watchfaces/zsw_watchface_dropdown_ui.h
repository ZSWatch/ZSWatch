#include "applications/watchface/watchface_app.h"

void zsw_watchface_dropdown_ui_add(lv_obj_t *root_page, watchface_app_evt_listener evt_cb, int brightness);
void zsw_watchface_dropdown_ui_set_music_info(char *track_name, char *artist);
void zsw_watchface_dropdown_ui_set_battery_info(uint8_t battery_percent, bool is_charging, int tte, int ttf);
void zsw_watchface_dropdown_ui_remove(void);
