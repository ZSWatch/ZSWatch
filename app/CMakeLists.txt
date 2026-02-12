# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

set(BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ZSWatchFW)

set(XIP_RELOCATE_SOURCES "")

zephyr_compile_definitions(
  -DNRF53_ERRATA_159_ENABLE_WORKAROUND=0 # Not allowing 128MHz CPU with QPSI at same time. We take the risk of this errata that may cause currupt QPSI data.
)

add_subdirectory(drivers)
add_subdirectory(src/history)
add_subdirectory(src/applications)
add_subdirectory(src/sensors)
add_subdirectory(src/sensor_fusion)
add_subdirectory(src/events)
add_subdirectory(src/managers)
add_subdirectory(src/drivers)
add_subdirectory(src/ble)
add_subdirectory(src/images)
add_subdirectory(src/ui)
add_subdirectory(lvgl_editor)

include_directories(src/)
include_directories(src/ui)
include_directories(src/applications)
include_directories(src/managers)
include_directories(src/events)
include_directories(src/drivers)
include_directories(src/sensors)
include_directories(src/sensor_fusion)
include_directories(src/ble)

if(CONFIG_AUDIO_DMIC)
set(kissfft_sources
    src/ext_drivers/kissfft/kiss_fft.c
    src/ext_drivers/kissfft/kiss_fftr.c
)

target_sources(app PRIVATE ${kissfft_sources})

set_source_files_properties(${kissfft_sources} PROPERTIES
    COMPILE_OPTIONS "-include;zephyr/kernel.h"
)

target_include_directories(app PRIVATE src/ext_drivers/kissfft)

target_compile_definitions(app PRIVATE
    kiss_fft_scalar=float
    KISS_FFT_MALLOC=k_malloc
    KISS_FFT_FREE=k_free
)

endif()

if (CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED)
    add_subdirectory(src/fuel_gauge)
else()
    add_subdirectory(src/basic_battery)
endif()

target_sources(app PRIVATE src/zsw_clock.c)
target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/zsw_wdt.c)
target_sources(app PRIVATE src/zsw_cpu_freq.c)
target_sources(app PRIVATE src/zsw_retained_ram_storage.c)
target_sources(app PRIVATE src/zsw_coredump.c)
target_sources_ifdef(CONFIG_SHELL app PRIVATE src/zsw_shell.c)

target_sources(app PRIVATE src/ui/notification/zsw_popup_notifcation.c)
target_sources(app PRIVATE src/ui/popup/zsw_popup_window.c)
target_sources(app PRIVATE src/ui/app_picker/app_picker_ui.c)
target_sources(app PRIVATE src/ui/utils/zsw_ui_utils.c)

target_sources_ifdef(CONFIG_SPI_FLASH_LOADER app PRIVATE src/filesystem/zsw_rtt_flash_loader.c)
target_sources_ifdef(CONFIG_FILE_SYSTEM_LITTLEFS app PRIVATE src/filesystem/zsw_filesystem.c)
target_sources_ifdef(CONFIG_FILE_SYSTEM app PRIVATE src/filesystem/zsw_custom_fs.c)

target_sources_ifdef(CONFIG_RTC_ALARM app PRIVATE src/zsw_alarm.c)

target_compile_definitions(app PRIVATE _POSIX_C_SOURCE=200809L)

FILE(GLOB events_sources src/events/*.c)
target_sources(app PRIVATE ${events_sources})

add_compile_definitions(LV_LVGL_H_INCLUDE_SIMPLE)

file(GLOB ext_fs_files "src/images/binaries/S/*.*")
list(LENGTH ext_fs_files num_ext_fs_files)
add_compile_definitions(NUM_RAW_FS_FILES=${num_ext_fs_files})
message("Number of external filesystem files:" ${num_ext_fs_files})

# TODO fix all of them. New since NCS 2.7
set_target_properties(app PROPERTIES COMPILE_FLAGS -Wno-double-promotion)
zephyr_compile_options(-Wno-double-promotion)

# XIP Code Relocation
if (CONFIG_MCUMGR)
    # Should work in XIP as we use update_app to enable FW update which forces XIP on during update.
    zephyr_code_relocate(LIBRARY mcumgr_mgmt LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(LIBRARY mcumgr_util LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(LIBRARY mcumgr_smp LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(LIBRARY mcumgr_grp_img_mgmt LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(LIBRARY mcumgr_grp_os_mgmt LOCATION EXTFLASH_TEXT NOCOPY)
endif()

# Relocate some LVGL sources to external flash.
# More can be added here if needed.
# Note code running from external flash is slower than internal flash.
# So choose wisely...
list(APPEND XIP_RELOCATE_SOURCES
    ../modules/lib/gui/lvgl/src/font/lv_font_montserrat_10.c
    ../modules/lib/gui/lvgl/src/font/lv_font_montserrat_12.c
    ../modules/lib/gui/lvgl/src/font/lv_font_montserrat_14.c
    ../modules/lib/gui/lvgl/src/font/lv_font_montserrat_16.c
    ../modules/lib/gui/lvgl/src/font/lv_font_montserrat_18.c
    ../modules/lib/gui/lvgl/src/font/lv_font_montserrat_20.c
    ../modules/lib/gui/lvgl/src/font/lv_font_montserrat_28.c
    ../modules/lib/gui/lvgl/src/widgets/chart/lv_chart.c
    ../modules/lib/gui/lvgl/src/widgets/scale/lv_scale.c
    ../modules/lib/gui/lvgl/src/widgets/dropdown/lv_dropdown.c
    ../modules/lib/gui/lvgl/src/widgets/buttonmatrix/lv_buttonmatrix.c
    ../modules/lib/gui/lvgl/src/widgets/slider/lv_slider.c
    ../modules/lib/gui/lvgl/src/widgets/menu/lv_menu.c
    ../modules/lib/gui/lvgl/src/widgets/imagebutton/lv_imagebutton.c
    ../modules/lib/gui/lvgl/src/widgets/led/lv_led.c
    ../modules/lib/gui/lvgl/src/widgets/checkbox/lv_checkbox.c
)

zephyr_code_relocate(FILES ${XIP_RELOCATE_SOURCES} LOCATION EXTFLASH_TEXT NOCOPY)
zephyr_code_relocate(FILES ${XIP_RELOCATE_SOURCES} LOCATION EXTFLASH_RODATA NOCOPY)
zephyr_code_relocate(FILES ${XIP_RELOCATE_SOURCES} LOCATION RAM_DATA)

if (CONFIG_USB_DEVICE_STACK)
    # USB can work in XIP as when plugging in VBUS (nPM1300 notifies us) we turn on XIP always on.
    # Also in update_app we turn on XIP always on before enabling USB.
    zephyr_code_relocate(LIBRARY drivers__usb__device LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(LIBRARY drivers__usb__device LOCATION RAM_DATA)

    zephyr_code_relocate(LIBRARY drivers__usb__common__nrf_usbd_common LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(LIBRARY drivers__usb__common__nrf_usbd_common LOCATION RAM_DATA)
endif()
