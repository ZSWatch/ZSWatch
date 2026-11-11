/**
 * @file lvgl_editor.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl_editor.h"
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
}

/**********************
 *   STATIC FUNCTIONS
 **********************/