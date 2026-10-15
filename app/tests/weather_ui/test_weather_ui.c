#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define WEATHER_UI_NUM_FORECASTS 4
#define LV_OBJ_FLAG_HIDDEN 1
struct object { unsigned flags; char text[100]; };
typedef struct object lv_obj_t;
typedef struct { double temperature, apparent_temperature, wind_speed; const void *icon; char *text; unsigned color; } weather_ui_current_weather_data_t;
typedef struct { double temperature; int rain_percent; const void *icon; double low_temp, high_temp; char day[4]; char *text; unsigned color; } weather_ui_forecast_data_t;
typedef struct { lv_obj_t *ui_day, *ui_day_temp, *ui_day_icon, *ui_day_day; } lv_obj_forecasts_t;
static struct object objects[32];
static lv_obj_t *root_page=&objects[0], *ui_root_container=&objects[1], *ui_today_container=&objects[2], *ui_time=&objects[3], *ui_loading_spinner=&objects[4], *ui_status_label=&objects[5], *ui_today_temp=&objects[6], *ui_today_min_max_temp=&objects[7], *ui_today_rain=&objects[8], *ui_today_icon=&objects[9];
static lv_obj_forecasts_t ui_forecasts[WEATHER_UI_NUM_FORECASTS];
static bool lv_obj_has_flag(lv_obj_t *o,unsigned flag) {return (o->flags&flag)!=0;}
static void lv_obj_clear_flag(lv_obj_t *o,unsigned flag) {o->flags&=~flag;}
static void lv_obj_add_flag(lv_obj_t *o,unsigned flag) {o->flags|=flag;}
static void lv_label_set_text(lv_obj_t *o,const char *text) {snprintf(o->text,sizeof(o->text),"%s",text);}
static void lv_label_set_text_fmt(lv_obj_t *o,const char *fmt,...) {va_list args;va_start(args,fmt);vsnprintf(o->text,sizeof(o->text),fmt,args);va_end(args);}
static void lv_img_set_src(lv_obj_t *o,const void *image) {(void)o;(void)image;}
#include "weather_update.inc"
int main(void) {
    for (int i=0;i<WEATHER_UI_NUM_FORECASTS;i++) {ui_forecasts[i].ui_day_temp=&objects[10+i*3];ui_forecasts[i].ui_day_icon=&objects[11+i*3];ui_forecasts[i].ui_day_day=&objects[12+i*3];}
    ui_root_container->flags=ui_today_container->flags=ui_time->flags=LV_OBJ_FLAG_HIDDEN;
    weather_ui_set_error("Failed");assert(strcmp(ui_status_label->text,"Failed")==0);
    weather_ui_current_weather_data_t current={.temperature=12.5};
    weather_ui_forecast_data_t days[WEATHER_UI_NUM_FORECASTS]={0};
    for(int i=0;i<WEATHER_UI_NUM_FORECASTS;i++) {days[i].temperature=14+i;days[i].low_temp=7;days[i].high_temp=14+i;snprintf(days[i].day,sizeof(days[i].day),"Day");}
    weather_ui_set_weather_data(current,days,WEATHER_UI_NUM_FORECASTS);
    assert(ui_status_label->text[0]==0);assert(!lv_obj_has_flag(ui_root_container,LV_OBJ_FLAG_HIDDEN));assert(lv_obj_has_flag(ui_loading_spinner,LV_OBJ_FLAG_HIDDEN));assert(strstr(ui_today_temp->text,"12.5")!=NULL);
    weather_ui_set_error("Timeout");current.temperature=9.5;
    weather_ui_set_weather_data(current,days,WEATHER_UI_NUM_FORECASTS);
    assert(ui_status_label->text[0]==0);assert(strstr(ui_today_temp->text,"9.5")!=NULL);
    root_page=NULL;weather_ui_set_error("late");weather_ui_set_weather_data(current,days,WEATHER_UI_NUM_FORECASTS);
    assert(ui_status_label->text[0]==0);
    puts("PASS: actual Weather UI clears errors on first and subsequent successful replies");
}
