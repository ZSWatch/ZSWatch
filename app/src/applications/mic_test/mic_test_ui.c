#include "mic_test_ui.h"
#include "circular_spectrum_ui.h"
#include "linear_spectrum_ui.h"
#include "ui/utils/zsw_ui_utils.h"
#include "assert.h"

static lv_obj_t *root_page = NULL;
static on_mic_test_ui_event_cb_t toggle_callback;
static lv_obj_t *toggle_button;
static lv_obj_t *button_label;
static lv_obj_t *status_label;
static lv_obj_t *mode_button;
static lv_obj_t *mode_label;
static bool is_recording = false;
static bool spectrum_ui_initialized = false;
static spectrum_mode_t current_mode = SPECTRUM_MODE_LINEAR;

// Forward declarations
static void show_demo_spectrum(void);
static void init_spectrum_mode(void);
static void cleanup_spectrum_mode(void);

static void toggle_button_event_cb(lv_event_t *e)
{
    if (toggle_callback) {
        toggle_callback();
    }
}

static void mode_button_event_cb(lv_event_t *e)
{
    // Switch between visualization modes
    current_mode = (current_mode + 1) % SPECTRUM_MODE_COUNT;
    
    // Update mode button text
    const char *mode_text = (current_mode == SPECTRUM_MODE_CIRCULAR) ? "○" : "|||";
    lv_label_set_text(mode_label, mode_text);
    
    // Cleanup old mode and init new mode
    cleanup_spectrum_mode();
    init_spectrum_mode();
    
    // Show demo in new mode
    show_demo_spectrum();
}

void mic_test_ui_show(lv_obj_t *root, on_mic_test_ui_event_cb_t toggle_cb)
{
    assert(root);
    toggle_callback = toggle_cb;

    // Create main container
    root_page = lv_obj_create(root);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));

    // Initialize spectrum analyzer FIRST (background layer)
    init_spectrum_mode();

    // Create overlay for controls with transparent background
    lv_obj_t *controls_overlay = lv_obj_create(root_page);
    lv_obj_set_style_border_width(controls_overlay, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(controls_overlay, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_size(controls_overlay, LV_PCT(100), LV_PCT(100));

    // Title label - smaller and at top
    lv_obj_t *title = lv_label_create(controls_overlay);
    lv_label_set_text(title, "Audio Spectrum");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(title, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Toggle button - smaller and centered
    toggle_button = lv_btn_create(controls_overlay);
    lv_obj_set_size(toggle_button, 80, 30);
    lv_obj_align(toggle_button, LV_ALIGN_CENTER, -25, 0);
    lv_obj_add_event_cb(toggle_button, toggle_button_event_cb, LV_EVENT_CLICKED, NULL);

    button_label = lv_label_create(toggle_button);
    lv_label_set_text(button_label, "Start");
    lv_obj_set_style_text_font(button_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_center(button_label);

    // Mode toggle button - small and to the right
    mode_button = lv_btn_create(controls_overlay);
    lv_obj_set_size(mode_button, 40, 30);
    lv_obj_align(mode_button, LV_ALIGN_CENTER, 35, 0);
    lv_obj_add_event_cb(mode_button, mode_button_event_cb, LV_EVENT_CLICKED, NULL);

    mode_label = lv_label_create(mode_button);
    lv_label_set_text(mode_label, "|||"); // Bars symbol for linear mode (default)
    lv_obj_set_style_text_font(mode_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_center(mode_label);

    // Status label - smaller and at bottom
    status_label = lv_label_create(controls_overlay);
    lv_label_set_text(status_label, "Ready");
    lv_obj_set_style_text_align(status_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(status_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(status_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -10);
}

void mic_test_ui_remove(void)
{
    cleanup_spectrum_mode();
    
    if (root_page) {
        lv_obj_del(root_page);
        root_page = NULL;
    }
}

void mic_test_ui_set_status(const char *status)
{
    if (status_label) {
        lv_label_set_text(status_label, status);
    }
}

void mic_test_ui_toggle_button_state(void)
{
    is_recording = !is_recording;
    if (button_label) {
        lv_label_set_text(button_label, is_recording ? "Stop" : "Start");
    }
}

void mic_test_ui_update_spectrum(const uint8_t *magnitudes, size_t num_bars)
{
    if (spectrum_ui_initialized) {
        if (current_mode == SPECTRUM_MODE_CIRCULAR && num_bars == SPECTRUM_NUM_BARS_CIRCULAR) {
            circular_spectrum_ui_update(magnitudes, num_bars);
        } else if (current_mode == SPECTRUM_MODE_LINEAR && num_bars == SPECTRUM_NUM_BARS_LINEAR) {
            linear_spectrum_ui_update(magnitudes, num_bars);
        }
    }
}

// Initialize the current spectrum mode
static void init_spectrum_mode(void)
{
    int ret = -1;
    
    if (current_mode == SPECTRUM_MODE_CIRCULAR) {
        printk("Initializing CIRCULAR spectrum mode\n");
        ret = circular_spectrum_ui_init(root_page, 120, 120, 40, 100);
    } else {
        printk("Initializing LINEAR spectrum mode\n");
        ret = linear_spectrum_ui_init(root_page, 10, 70, 220, 100);
    }
    
    if (ret == 0) {
        spectrum_ui_initialized = true;
        printk("Spectrum mode initialized successfully, showing demo\n");
        show_demo_spectrum();
    } else {
        printk("Failed to initialize spectrum mode: %d", ret);
    }
}

// Cleanup the current spectrum mode
static void cleanup_spectrum_mode(void)
{
    if (spectrum_ui_initialized) {
        if (current_mode == SPECTRUM_MODE_CIRCULAR) {
            circular_spectrum_ui_remove();
        } else {
            linear_spectrum_ui_remove();
        }
        spectrum_ui_initialized = false;
    }
}

// For testing - create some demo spectrum data
static void show_demo_spectrum(void)
{
    if (spectrum_ui_initialized) {
        if (current_mode == SPECTRUM_MODE_CIRCULAR) {
            uint8_t demo_magnitudes[24];
            for (int i = 0; i < 24; i++) {
                demo_magnitudes[i] = (i * 10) % 255; // Create gradient pattern
            }
            circular_spectrum_ui_update(demo_magnitudes, 24);
        } else {
            printk("Showing LINEAR demo spectrum\n");
            uint8_t demo_magnitudes[48];
            for (int i = 0; i < 48; i++) {
                demo_magnitudes[i] = 150 + (i * 2) % 100; // Create sawtooth pattern (150-250)
            }
            linear_spectrum_ui_update(demo_magnitudes, 48);
        }
    }
}
