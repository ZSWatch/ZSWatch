/**
 * @file lvgl_editor.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl_editor.h"

LV_FONT_DECLARE(zsw_font_12);
LV_FONT_DECLARE(zsw_font_14);
LV_FONT_DECLARE(zsw_font_20);
LV_FONT_DECLARE(zsw_font_28);
LV_FONT_DECLARE(zsw_font_48);
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lvgl_editor_init(const char * asset_path)
{
    // Initialize subjects and other assets including Editor-generated image arrays
    lvgl_editor_init_gen(asset_path);

    /*
     * The Editor generates its own Montserrat fonts from globals.xml. Use the
     * fonts of the watch UI theme instead, so the components look like the rest.
     */
    montserrat_10 = (lv_font_t *)&zsw_font_12;
    montserrat_12 = (lv_font_t *)&zsw_font_12;
    montserrat_14 = (lv_font_t *)&zsw_font_14;
    montserrat_14_full = (lv_font_t *)&zsw_font_14;
    montserrat_24 = (lv_font_t *)&zsw_font_20;
    montserrat_28 = (lv_font_t *)&zsw_font_28;
    montserrat_48 = (lv_font_t *)&zsw_font_48;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/