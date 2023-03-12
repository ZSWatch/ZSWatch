#include <applications/application_manager.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(APP_MANAGER, LOG_LEVEL_DBG);

#define MAX_APPS        10
#define INVALID_APP_ID  0xFF

static void draw_application_picker(void);
static void app_clicked(lv_event_t *e);
static void async_app_start(lv_timer_t *timer);
static void async_app_close(lv_timer_t *timer);

static application_t *apps[MAX_APPS];
static uint8_t num_apps;
static uint8_t current_app;
static lv_obj_t *root_obj;
static lv_group_t *group_obj;
static on_application_manager_cb_fn close_cb_func;
static lv_obj_t *grid;

static void delete_application_picker(void)
{
    if (grid != NULL) {
        lv_obj_del(grid);
        grid = NULL;
    }
}

static void row_focused(lv_event_t *e)
{
    lv_obj_t *row = lv_event_get_target(e);
    if (row && lv_obj_get_child_cnt(row) > 0) {
        lv_obj_t *title_label = lv_obj_get_user_data(row);
        if (title_label) {
            lv_obj_set_style_text_color(title_label, lv_color_black(), LV_PART_MAIN);
        }
    }
}

static void row_unfocused(lv_event_t *e)
{
    lv_obj_t *row = lv_event_get_target(e);
    // Need to check child count as during delete we get this callback called, but without children.
    // The image is deleted already.
    if (row && lv_obj_get_child_cnt(row) > 0) {
        lv_obj_t *title_label = lv_obj_get_user_data(row);
        if (title_label) {
            lv_obj_set_style_text_color(title_label, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN);
        }
    }
}

static void app_clicked(lv_event_t *e)
{
    int app_id = (int)lv_event_get_user_data(e);
    current_app = app_id;
    // This function may be called within a lvgl callback such
    // as a button click. If we create a new ui in this callback
    // which registers a button press callback then that callback
    // may get called, but we don't want that. So delay the opening
    // of the new application some time.
    lv_timer_t *timer = lv_timer_create(async_app_start, 500,  NULL);
    lv_timer_set_repeat_count(timer, 1);
}

static void async_app_start(lv_timer_t *timer)
{
    LOG_DBG("Start %d", current_app);
    delete_application_picker();
    apps[current_app]->start_func(root_obj, group_obj);
}

static void async_app_close(lv_timer_t *timer)
{
    LOG_DBG("Stop %d", current_app);
    apps[current_app]->stop_func();
    current_app = INVALID_APP_ID;
    draw_application_picker();
}

static void async_app_manager_close(lv_timer_t *timer)
{
    LOG_DBG("Close app manager");
    close_cb_func();
}

static void app_manager_close_button_pressed(lv_event_t *e)
{
    lv_timer_t *timer = lv_timer_create(async_app_manager_close, 500,  NULL);
    lv_timer_set_repeat_count(timer, 1);
}

static void scroll_event_cb(lv_event_t *e)
{
    lv_obj_t *cont = lv_event_get_target(e);
    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    lv_coord_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;
    lv_coord_t r = lv_obj_get_height(cont) * 5 / 10;

    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_cnt(cont);
    for (i = 0; i < child_cnt; i++) {
        lv_obj_t *child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        lv_coord_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;

        lv_coord_t diff_y = child_y_center - cont_y_center;
        diff_y = LV_ABS(diff_y);

        /* Get the x of diff_y on a circle. */
        lv_coord_t x;
        /* If diff_y is out of the circle use the last point of the circle (the radius) */
        if (diff_y >= r) {
            x = r;
        } else {
            /* Use Pythagoras theorem to get x from radius and y */
            uint32_t x_sqr = r * r - diff_y * diff_y;
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000);   /* Use lvgl's built in sqrt root function */
            x = r - res.i - 20; /* Added - 20 here to pull all a bit more to the left side */
        }

        /* Translate the item by the calculated X coordinate */
        lv_obj_set_style_translate_x(child, x, 0);
        lv_obj_set_style_translate_y(child, -13, 0);

        /* Uncomment if to use some opacity with larger translations */
        //lv_opa_t opa = lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);
        //lv_obj_set_style_opa(child, LV_OPA_COVER - opa, 0);
    }
}

static lv_obj_t *create_application_list_entry(lv_obj_t *grid, const lv_img_dsc_t *icon, const char *name, int app_id)
{
    lv_obj_t *cont = lv_obj_create(grid);
    lv_obj_center(cont);
    lv_obj_set_style_border_side(cont, LV_BORDER_SIDE_NONE, 0);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_size(cont, LV_SIZE_CONTENT, icon->header.h);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE); // Needed, otherwise indev will first focus on this cont before it's contents.

    lv_obj_add_event_cb(cont, row_focused, LV_EVENT_FOCUSED, (void *)app_id);
    lv_obj_add_event_cb(cont, row_unfocused, LV_EVENT_DEFOCUSED, (void *)app_id);
    lv_group_add_obj(group_obj, cont);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    lv_obj_t *img_icon = lv_img_create(cont);
    lv_img_set_src(img_icon, icon);
    lv_obj_set_size(img_icon, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(img_icon, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t *title = lv_label_create(cont);
    lv_label_set_text(title, name);
    lv_obj_set_size(title, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align_to(title, img_icon, LV_ALIGN_OUT_RIGHT_MID, 15, 0);
    lv_obj_set_style_text_color(title, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN);
    lv_obj_set_style_text_align(title, LV_ALIGN_OUT_LEFT_MID, LV_PART_MAIN);

    lv_obj_set_user_data(cont, title);

    return cont;
}

static void draw_application_picker(void)
{
    lv_obj_t *entry;
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_flex_flow(&style, LV_FLEX_FLOW_ROW_WRAP);
    lv_style_set_flex_main_place(&style, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_style_set_layout(&style, LV_LAYOUT_FLEX);

    lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

    assert(grid == NULL);
    grid = lv_obj_create(root_obj);
    lv_obj_add_style(grid, &style, 0);
    lv_obj_set_scrollbar_mode(root_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_side(grid, LV_BORDER_SIDE_NONE, 0);

    lv_obj_set_size(grid, LV_PCT(100), LV_PCT(100));
    lv_obj_center(grid);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_dir(grid, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(grid, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(grid, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_event_cb(grid, scroll_event_cb, LV_EVENT_SCROLL, NULL);

    for (int i = 0; i < num_apps; i++) {
        LOG_DBG("Apps[%d]: %s", i, apps[i]->name);
        entry = create_application_list_entry(grid, apps[i]->icon, apps[i]->name, i);
        lv_obj_add_event_cb(entry, app_clicked, LV_EVENT_RELEASED, (void *)i);
    }

    LV_IMG_DECLARE(close_icon);
    entry = create_application_list_entry(grid, &close_icon, "Close", num_apps);
    lv_obj_add_event_cb(entry, app_manager_close_button_pressed, LV_EVENT_RELEASED, NULL);

    lv_group_focus_obj(lv_obj_get_child(grid, 0));

    /* Update the notifications position manually firt time */
    lv_event_send(grid, LV_EVENT_SCROLL, NULL);

    /* Be sure the fist notification is in the middle */
    lv_obj_scroll_to_view(lv_obj_get_child(grid, 0), LV_ANIM_OFF);
}

void application_manager_show(on_application_manager_cb_fn close_cb, lv_obj_t *root, lv_group_t *group)
{
    close_cb_func = close_cb;
    root_obj = root;
    group_obj = group;
    draw_application_picker();
}

void application_manager_delete(void)
{
    if (current_app < num_apps) {
        LOG_DBG("Stop force %d", current_app);
        apps[current_app]->stop_func();
    }
    delete_application_picker();
}

void application_manager_add_application(application_t *app)
{
    __ASSERT_NO_MSG(num_apps < MAX_APPS);
    apps[num_apps] = app;
    num_apps++;
}

void application_manager_exit_app(void)
{
    lv_timer_t *timer = lv_timer_create(async_app_close, 500,  NULL);
    lv_timer_set_repeat_count(timer, 1);
}

void application_manager_app_close_request(application_t *app)
{
    LOG_DBG("application_manager_app_close_request");
    application_manager_exit_app();
}

static int application_manager_init(const struct device *arg)
{
    memset(apps, 0, sizeof(apps));
    num_apps = 0;
    current_app = INVALID_APP_ID;

    return 0;
}

SYS_INIT(application_manager_init, POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY);
