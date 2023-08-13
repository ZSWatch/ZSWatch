#ifndef __ZSW_POPUP_NOTIFCATION_H
#define __ZSW_POPUP_NOTIFCATION_H

#include <notification_manager.h>

typedef void (*on_close_notif_cb_t)(uint32_t id);

void zsw_notification_popup_show(char *title, char *body, notification_src_t icon, uint32_t id,
                                 on_close_notif_cb_t close_cb,
                                 uint32_t close_after_seconds);

void zsw_notification_popup_remove(void);

bool zsw_notification_popup_is_shown(void);

#endif