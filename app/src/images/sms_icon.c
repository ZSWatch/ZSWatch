#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_SMS_ICON
#define LV_ATTRIBUTE_IMG_SMS_ICON
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_SMS_ICON uint8_t sms_icon_map[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x3B,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x3B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x3B,0x01,0x00,0x00,
    0x00,0x00,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x00,0x00,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x9B,0x11,0x1D,0x6C,0x7D,0x7C,0x7D,0x7C,0x7D,0x7C,0x7D,0x7C,0x1D,0x6C,0x9B,0x11,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x1D,0x6C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x1D,0x6C,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,
    0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5D,0x7C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x5D,0x7C,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5D,0x7C,0xFF,0xFF,0x7D,0x7C,0x3F,0xBE,0x5F,0xC6,0x7D,0x7C,0xFF,0xFF,0x5D,0x7C,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5D,0x7C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x5D,0x7C,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,
    0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5D,0x7C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7D,0x53,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5D,0x7C,0x5F,0xC6,0x9C,0x32,0x3C,0x22,0x3C,0x22,0x3C,0x22,0xFC,0x19,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x3C,0x22,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x7B,0x01,
    0x00,0x00,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x00,0x00,0x00,0x00,0x3B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x7B,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x3B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x7B,0x01,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x5B,0x01,0x7B,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x2E,0x91,0xD5,0xF4,0xF4,0xD4,0x90,0x2D,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x98,0xFD,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFD,0x96,0x07,0x00,0x00,0x00,0x07,0xC0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xBF,0x07,0x00,0x00,0x98,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x95,0x00,0x2E,0xFD,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0x2C,0x92,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x8F,
    0xD5,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xD4,0xF3,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF2,0xF3,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF1,0xD5,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xD3,0x90,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x8D,0x2D,0xFD,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0x2B,
    0x00,0x96,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x93,0x00,0x00,0x07,0xBF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xBE,0x06,0x00,0x00,0x00,0x07,0x95,0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0x93,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x2C,0x8F,0xD4,0xF3,0xF3,0xD3,0x8E,0x2B,0x00,0x00,0x00,0x00,
}; // LVGL_9 compatible

const lv_img_dsc_t sms_icon = {
  .header.cf = LV_COLOR_FORMAT_NATIVE_WITH_ALPHA,
  .header.magic = LV_IMAGE_HEADER_MAGIC,
  
  .header.w = 16,
  .header.h = 16,
  .data_size = 256 * LV_COLOR_NATIVE_WITH_ALPHA_SIZE,
  .data = sms_icon_map,
};
