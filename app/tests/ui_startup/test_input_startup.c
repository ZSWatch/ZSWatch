#include <assert.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdio.h>
#define LOG_DBG(...) ((void)0)
#define INPUT_ABS_X 1
#define INPUT_ABS_Y 2
#define INPUT_BTN_TOUCH 3
#define INPUT_KEY_KP0 4
#define INPUT_KEY_KP1 5
#define INPUT_EV_DEVICE 6
struct input_event {int type,code,value;};
static atomic_int input_ready;
#define atomic_get(x) atomic_load(x)
static struct {int work; struct input_event event;} input_worker_item;
static unsigned activity_calls,work_calls;
static bool managers_ready;
static void zsw_power_manager_on_user_activity(void) {assert(managers_ready);activity_calls++;}
static void k_work_submit(int *work) {(void)work;assert(managers_ready);work_calls++;}
#include "input_callback.inc"
int main(void) {
    struct input_event event={.type=1,.code=INPUT_BTN_TOUCH,.value=1};
    on_input_subsys_callback(&event,NULL);event.code=INPUT_EV_DEVICE;on_input_subsys_callback(&event,NULL);
    assert(activity_calls==0&&work_calls==0);
    managers_ready=true;atomic_store(&input_ready,1);
    event.code=INPUT_BTN_TOUCH;on_input_subsys_callback(&event,NULL);assert(activity_calls==1&&work_calls==0);
    event.code=INPUT_EV_DEVICE;on_input_subsys_callback(&event,NULL);assert(activity_calls==2&&work_calls==1);
    assert(input_worker_item.event.code==INPUT_EV_DEVICE);
    puts("PASS: early input ignored; initialized touch activity and filtered/queued input preserved");
}
