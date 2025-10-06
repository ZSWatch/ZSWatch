#ifndef ZSW_USB_MANAGER_H
#define ZSW_USB_MANAGER_H

#include <stdbool.h>
#include <errno.h>

int _zsw_usb_manager_enable(const char *requester);
int _zsw_usb_manager_disable(const char *requester);
#ifdef CONFIG_USB_DEVICE_STACK
bool zsw_usb_manager_is_enabled(void);
#define zsw_usb_manager_enable() _zsw_usb_manager_enable(__func__)
#define zsw_usb_manager_disable() _zsw_usb_manager_disable(__func__)
#else
#define zsw_usb_manager_enable() (0)
#define zsw_usb_manager_disable() (0)
#define zsw_usb_manager_is_enabled() (false)
#endif

#endif // ZSW_USB_MANAGER_H
