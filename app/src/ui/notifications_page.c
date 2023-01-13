#include <notifications_page.h>
#include <lvgl.h>

static void close_button_pressed(lv_event_t *e);
static void not_button_pressed(lv_event_t *e);
static void scroll_event_cb(lv_event_t *e);
static void build_notification_entry(lv_obj_t *parent, not_mngr_notification_t *not, lv_group_t *input_group);

static on_notification_page_closed_cb_t closed_callback;
static on_notification_remove_cb_t not_removed_callback;

static lv_obj_t *main_page;
static lv_group_t *group;

void notifications_page_init(on_notification_page_closed_cb_t closed_cb, on_notification_remove_cb_t not_removed_cb)
{
    closed_callback = closed_cb;
    not_removed_callback = not_removed_cb;
}

void notifications_page_create(not_mngr_notification_t *notifications, uint8_t num_notifications,
                               lv_group_t *input_group)
{
    group = input_group;

    main_page = lv_obj_create(lv_scr_act());
    lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_size(main_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_border_side(main_page, LV_BORDER_SIDE_NONE, 0);
    lv_obj_center(main_page);

    lv_obj_set_flex_flow(main_page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_dir(main_page, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(main_page, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(main_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_event_cb(main_page, scroll_event_cb, LV_EVENT_SCROLL, NULL);

    for (int i = 0; i < num_notifications; i++) {
        build_notification_entry(main_page, &notifications[i], input_group);
    }

    lv_obj_t *float_btn = lv_btn_create(main_page);
    lv_obj_set_size(float_btn, 50, 50);
    lv_obj_add_flag(float_btn, LV_OBJ_FLAG_FLOATING);
    lv_obj_align(float_btn, LV_ALIGN_BOTTOM_RIGHT, 0, -lv_obj_get_style_pad_right(main_page, LV_PART_MAIN));
    lv_obj_add_event_cb(float_btn, close_button_pressed, LV_EVENT_PRESSED, main_page);
    lv_obj_set_style_radius(float_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_img_src(float_btn, LV_SYMBOL_CLOSE, 0);
    lv_obj_set_style_text_font(float_btn, lv_theme_get_font_large(float_btn), 0);

    lv_group_focus_obj(float_btn);

    /* Update the notifications position manually firt time */
    lv_event_send(main_page, LV_EVENT_SCROLL, NULL);

    /* Be sure the fist notification is in the middle */
    lv_obj_scroll_to_view(lv_obj_get_child(main_page, 0), LV_ANIM_OFF);
}

void notifications_page_close(void)
{
    lv_obj_del(main_page);
}

static void build_notification_entry(lv_obj_t *parent, not_mngr_notification_t *not, lv_group_t *input_group)
{
    lv_obj_t *title;
    lv_obj_t *cont;
    static lv_style_t outline_default;
    static lv_style_t outline_focused;

    // Border around selected menu row when focused
    lv_style_init(&outline_default);
    lv_style_set_border_color(&outline_default, lv_color_hex(0x001833));
    lv_style_set_border_width(&outline_default, lv_disp_dpx(lv_disp_get_next(NULL), 1));
    lv_style_set_border_opa(&outline_default, LV_OPA_50);
    lv_style_set_border_side(&outline_default, LV_BORDER_SIDE_BOTTOM);

    lv_style_init(&outline_focused);
    lv_style_set_border_color(&outline_focused, lv_palette_main(LV_PALETTE_LIGHT_BLUE));
    lv_style_set_border_width(&outline_focused, lv_disp_dpx(lv_disp_get_next(NULL), 3));
    lv_style_set_border_side(&outline_focused, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&outline_focused, 8);
    lv_style_set_bg_color(&outline_default, lv_color_hex(0xC1C1C1));

    cont = lv_obj_create(parent);
    lv_obj_set_layout(cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_size(cont, LV_PCT(95), LV_SIZE_CONTENT);
    lv_obj_add_style(cont, &outline_focused, LV_STATE_FOCUS_KEY);

    lv_obj_add_event_cb(cont, not_button_pressed, LV_EVENT_CLICKED, (void *)not->id);
    lv_group_add_obj(input_group, cont);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    title = lv_label_create(cont);
    lv_label_set_text(title, not->title);
    lv_obj_set_size(title, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_center(title);
    lv_obj_add_flag(title, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    title = lv_label_create(cont);
    lv_label_set_text(title, not->body);
    lv_obj_set_size(title, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_add_flag(title, LV_OBJ_FLAG_CLICK_FOCUSABLE);
}

static void not_button_pressed(lv_event_t *e)
{
    lv_obj_del(lv_event_get_target(e));
    not_removed_callback((uint32_t)lv_event_get_user_data(e));
}

static void close_button_pressed(lv_event_t *e)
{
    lv_obj_del(main_page);
    closed_callback();
}

static void scroll_event_cb(lv_event_t *e)
{
    lv_obj_t *cont = lv_event_get_target(e);
    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    lv_coord_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;
    lv_coord_t r = lv_obj_get_height(cont) * 7 / 10;
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
            x = r - res.i;
        }

        /* Translate the item by the calculated X coordinate */
        lv_obj_set_style_translate_x(child, x, 0);
        lv_obj_set_style_translate_y(child, -13, 0);

        /* Use some opacity with larger translations */
        lv_opa_t opa = lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_obj_set_style_opa(child, LV_OPA_COVER - opa, 0);
    }
}