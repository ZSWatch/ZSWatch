#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "cJSON.h"
#include "ble_http.h"
#include "weather_ui.h"
#define ARG_UNUSED(x) (void)(x)
#define LOG_ERR(...) ((void)0)
#define LOG_INF(...) ((void)0)
#define K_NO_WAIT 0
#define ZSW_APP_STATE_UI_VISIBLE 1
struct k_work {int submitted;};
static struct k_work weather_http_work, weather_app_publish;
static struct {int current_state;} app = {1};
static struct {double temperature_c, humidity, wind, wind_direction; int weather_code; char report_text[64];} last_weather;
static long last_update_weather_time;
typedef struct {struct tm tm;} zsw_timeval_t;
struct weather_http_result {ble_http_status_code_t status; char *response;};
static struct {struct weather_http_result entries[2]; unsigned count;} weather_http_results;
static int allocations, ui_calls, errors, gps_stops;
static int fail_allocation;
static double shown_temperature;
static char shown_error[64];
static void *k_malloc(size_t n) {if(fail_allocation)return NULL; void *p=malloc(n);if(p)allocations++;return p;}
static void k_free(void *p) {if(p)allocations--;free(p);}
static int k_msgq_put(void *q,const struct weather_http_result *r,int wait) {(void)q;(void)wait;if(weather_http_results.count==2)return -1;weather_http_results.entries[weather_http_results.count++]=*r;return 0;}
static int k_msgq_get(void *q,struct weather_http_result *r,int wait) {(void)q;(void)wait;if(!weather_http_results.count)return -1;*r=weather_http_results.entries[0];weather_http_results.entries[0]=weather_http_results.entries[1];weather_http_results.count--;return 0;}
static void k_work_submit(struct k_work *w) {w->submitted++;}
static long k_uptime_get(void) {return 1234;}
static void zsw_clock_get_time(zsw_timeval_t *t) {memset(t,0,sizeof(*t));}
static int ble_comm_request_gps_status(int active) {assert(!active);gps_stops++;return 0;}
static int wmo_code_to_weather_code(int code) {return code;}
static const char *zsw_ui_utils_day_name(int day) {(void)day;return "Thu";}
static const void *zsw_ui_utils_icon_from_wmo_weather_code(int code,unsigned *color,char **text) {(void)code;*color=1;*text="Cloudy";return NULL;}
void weather_ui_set_weather_data(weather_ui_current_weather_data_t current,weather_ui_forecast_data_t days[WEATHER_UI_NUM_FORECASTS],int count) {(void)days;assert(count>0&&count<=WEATHER_UI_NUM_FORECASTS);assert(app.current_state==1);ui_calls++;shown_temperature=current.temperature;}
void weather_ui_set_error(char *error) {errors++;snprintf(shown_error,sizeof(shown_error),"%s",error);}
#include "weather_response.inc"
static const char valid[]="{\"current\":{\"temperature_2m\":12.5,\"weather_code\":3,\"wind_speed_10m\":2,\"apparent_temperature\":11},\"daily\":{\"weather_code\":[3,1,2,3],\"temperature_2m_max\":[14,15,16,17],\"temperature_2m_min\":[5,6,7,8],\"precipitation_probability_max\":[10,20,30,40]}}";
static void run_reply(const char *text) {char *borrowed=malloc(strlen(text)+1);strcpy(borrowed,text);int before=ui_calls;http_rsp_cb(BLE_HTTP_STATUS_OK,borrowed);memset(borrowed,0,strlen(borrowed));free(borrowed);assert(ui_calls==before);process_http_result(NULL);assert(allocations==0);}
int main(void) {
    run_reply(valid);assert(ui_calls==1&&shown_temperature==12.5&&gps_stops==1);
    run_reply("not json");assert(errors==1&&ui_calls==1);
    run_reply("{}");assert(errors==2&&ui_calls==1);
    cJSON *root=cJSON_Parse(valid);cJSON *daily=cJSON_GetObjectItem(root,"daily");
    cJSON_AddItemToArray(cJSON_GetObjectItem(daily,"weather_code"),cJSON_CreateNumber(1));
    char *bad=cJSON_PrintUnformatted(root);run_reply(bad);cJSON_free(bad);cJSON_Delete(root);assert(errors==3&&ui_calls==1);
    root=cJSON_Parse(valid);daily=cJSON_GetObjectItem(root,"daily");cJSON_ReplaceItemInArray(cJSON_GetObjectItem(daily,"temperature_2m_max"),0,cJSON_CreateNull());bad=cJSON_PrintUnformatted(root);run_reply(bad);cJSON_free(bad);cJSON_Delete(root);assert(errors==4&&ui_calls==1);
    http_rsp_cb(BLE_HTTP_STATUS_TIMEOUT,NULL);assert(errors==4);process_http_result(NULL);assert(errors==5&&strcmp(shown_error,"Timeout")==0);
    fail_allocation=1;run_reply(valid);fail_allocation=0;assert(errors==6);
    http_rsp_cb(BLE_HTTP_STATUS_OK,(char *)valid);http_rsp_cb(BLE_HTTP_STATUS_OK,(char *)valid);http_rsp_cb(BLE_HTTP_STATUS_OK,(char *)valid);assert(allocations==2);process_http_result(NULL);assert(allocations==0&&ui_calls==3);
    app.current_state=0;run_reply(valid);assert(ui_calls==3&&gps_stops==4);
    puts("PASS: owned/deferred response, valid forecast, malformed/missing/oversized/null data, timeout, allocation/queue failure and closed UI");
}
