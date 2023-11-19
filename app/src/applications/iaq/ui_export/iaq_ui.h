#pragma once

#include "lvgl.h"

/** @brief          
 *  @param p_parent Pointer to UI parent
*/
void iaq_app_ui_show(lv_obj_t *p_parent);

/** @brief  
*/
void iaq_app_ui_remove(void);

/** @brief      Set the angle of the IAQ cursor.
 *  @param iaq  IAQ value
*/
void iaq_app_ui_home_set_iaq_cursor(float iaq);

/** @brief      Set the value for the IAQ label.
 *  @param iaq  IAQ value
*/
void iaq_app_ui_home_set_iaq_label(float iaq);

/** @brief      Set the IAQ status message.
 *  @param iaq  IAQ value
*/
void iaq_app_ui_home_set_iaq_status(float iaq);