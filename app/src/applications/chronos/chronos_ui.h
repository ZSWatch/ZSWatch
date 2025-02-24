#pragma once


#include <lvgl.h>



void chronos_ui_init(lv_obj_t *root);

void chronos_ui_deinit();

void add_app_title(lv_obj_t *parent, const char *title, const void *src);


void notifications_init(lv_obj_t *page);
void add_notification(int id, const char *title, const char *time, const char *message);

void weather_init(lv_obj_t *page);
void add_weekly(const char *day, const void *src, int temp);


void navigation_init(lv_obj_t *page);
void navigateInfo(const char *text, const char *title, const char *directions);
void navIconState(bool show);
void setNavIconPx(uint16_t x, uint16_t y, bool on);

