#ifndef ZSW_UI_NAVIGATION_H
#define ZSW_UI_NAVIGATION_H

#include <zephyr/input/input.h>
#include <lvgl.h>
#include "applications/watchface/watchface_app.h"

typedef enum {
    ZSW_UI_STATE_INIT,
    ZSW_UI_STATE_WATCHFACE,
    ZSW_UI_STATE_APPLICATION_MANAGER,
} zsw_ui_state_t;

typedef struct {
    void (*on_notification_popup_request)(void);
    void (*on_app_manager_request)(const char *app_name);
    void (*on_app_manager_close)(void);
    void (*on_notification_closed)(uint32_t id);
} zsw_ui_navigation_callbacks_t;


int zsw_ui_navigation_init(const zsw_ui_navigation_callbacks_t *callbacks);

void zsw_ui_navigation_set_notification_mode(void);
void zsw_ui_navigation_clear_notification_mode(void);



#endif /* ZSW_UI_NAVIGATION_H */