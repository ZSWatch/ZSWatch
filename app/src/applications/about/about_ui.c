#include "about_ui.h"
#include "ui/zsw_ui.h"

static lv_obj_t *root_page = NULL;

static lv_obj_t *ui_fw_version;
static lv_obj_t *ui_compile_time;
static lv_obj_t *ui_filesystem_stats;
static lv_obj_t *ui_num_applications;
static lv_obj_t *ui_hw_revision;
static lv_obj_t *ui_sdk_versions;

ZSW_LV_IMG_DECLARE(ZSWatch_logo_small);
ZSW_LV_IMG_DECLARE(zswatch_text);

void about_ui_show(lv_obj_t *root, int hw_version, char *fw_version, char *fw_version_info, char *sdk_version,
                   char *fs_stats, int num_apps)
{
    assert(root_page == NULL);

    root_page = lv_obj_create(root);
    lv_obj_remove_style_all(root_page);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_width(root_page, lv_pct(100));
    lv_obj_set_height(root_page, lv_pct(100));
    lv_obj_set_align(root_page, LV_ALIGN_CENTER);
    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_logo_img = lv_img_create(root_page);
#ifdef CONFIG_BOARD_NATIVE_POSIX
    lv_img_set_src(ui_logo_img, ZSW_LV_IMG_USE(zswatch_text));
#else
    lv_img_set_src(ui_logo_img, "S:zswatch_text.bin");
#endif
    lv_obj_set_width(ui_logo_img, LV_SIZE_CONTENT);   /// 180
    lv_obj_set_height(ui_logo_img, LV_SIZE_CONTENT);    /// 26
    lv_obj_set_x(ui_logo_img, 0);
    lv_obj_set_y(ui_logo_img, 40);
    lv_obj_set_align(ui_logo_img, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_logo_img, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_logo_img, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_fw_version = lv_label_create(root_page);
    lv_obj_set_width(ui_fw_version, 175);
    lv_obj_set_height(ui_fw_version, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_fw_version, 0);
    lv_obj_set_y(ui_fw_version, 75);
    lv_obj_set_align(ui_fw_version, LV_ALIGN_CENTER);
    lv_label_set_text(ui_fw_version, fw_version);
    lv_obj_set_style_text_align(ui_fw_version, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_compile_time = lv_label_create(root_page);
    lv_obj_set_width(ui_compile_time, 200);
    lv_obj_set_height(ui_compile_time, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_compile_time, 0);
    lv_obj_set_y(ui_compile_time, 95);
    lv_obj_set_align(ui_compile_time, LV_ALIGN_CENTER);
    lv_label_set_text_fmt(ui_compile_time, "Compiled at\n%s", fw_version_info);
    lv_obj_set_style_text_align(ui_compile_time, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_compile_time, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_filesystem_stats = lv_label_create(root_page);
    lv_obj_set_width(ui_filesystem_stats, 200);
    lv_obj_set_height(ui_filesystem_stats, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_filesystem_stats, 0);
    lv_obj_set_y(ui_filesystem_stats, 10);
    lv_obj_set_align(ui_filesystem_stats, LV_ALIGN_CENTER);
    lv_label_set_text(ui_filesystem_stats, fs_stats);
    lv_obj_set_style_text_align(ui_filesystem_stats, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_num_applications = lv_label_create(root_page);
    lv_obj_set_width(ui_num_applications, 175);
    lv_obj_set_height(ui_num_applications, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_num_applications, 0);
    lv_obj_set_y(ui_num_applications, 25);
    lv_obj_set_align(ui_num_applications, LV_ALIGN_CENTER);
    lv_label_set_text_fmt(ui_num_applications, "%d Applications", num_apps);
    lv_obj_set_style_text_align(ui_num_applications, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_hw_revision = lv_label_create(root_page);
    lv_obj_set_width(ui_hw_revision, 175);
    lv_obj_set_height(ui_hw_revision, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_hw_revision, 0);
    lv_obj_set_y(ui_hw_revision, -30);
    lv_obj_set_align(ui_hw_revision, LV_ALIGN_CENTER);
    lv_label_set_text_fmt(ui_hw_revision, "Hardware v%d", hw_version);
    lv_obj_set_style_text_align(ui_hw_revision, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_hw_revision, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *icon_img = lv_img_create(root_page);
#ifdef CONFIG_BOARD_NATIVE_POSIX
    lv_img_set_src(icon_img, ZSW_LV_IMG_USE(ZSWatch_logo_small));
#else
    lv_img_set_src(icon_img, "S:ZSWatch_logo_small.bin");
#endif
    lv_obj_set_width(icon_img, LV_SIZE_CONTENT);   /// 25
    lv_obj_set_height(icon_img, LV_SIZE_CONTENT);    /// 40
    lv_obj_set_x(icon_img, -55);
    lv_obj_set_y(icon_img, -30);
    lv_obj_set_align(icon_img, LV_ALIGN_CENTER);
    lv_obj_add_flag(icon_img, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(icon_img, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_sdk_versions = lv_label_create(root_page);
    lv_obj_set_width(ui_sdk_versions, 220);
    lv_obj_set_height(ui_sdk_versions, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_sdk_versions, 0);
    lv_obj_set_y(ui_sdk_versions, 60);
    lv_obj_set_align(ui_sdk_versions, LV_ALIGN_CENTER);
    lv_label_set_text(ui_sdk_versions, sdk_version);
    lv_obj_set_style_text_align(ui_sdk_versions, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void about_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}