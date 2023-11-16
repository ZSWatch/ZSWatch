cmake_minimum_required(VERSION 3.20.0)

# Apply patches before build
file(GLOB_RECURSE files RELATIVE ${CMAKE_SOURCE_DIR} "patches/*.patch")
foreach(file ${files})
    execute_process(COMMAND 
                    patch -p1 -d ../zephyr/ -i ${CMAKE_CURRENT_SOURCE_DIR}/${file} -r - --no-backup-if-mismatch)
endforeach()

string(REGEX MATCH "zswatch_nrf5340_cpuapp(_ns)?@([0-9]+)$" ZSWATCH_BOARD "${BOARD}")
if(ZSWATCH_BOARD)
    set(ZSWATCH_REV ${CMAKE_MATCH_2})
    add_definitions(-DZSWATCH_REV=${ZSWATCH_REV})
    # Rev 3 and later will handle this later in a different way.
    if(${ZSWATCH_REV} EQUAL 2)
        set(DFU_BUILD true)
        set(PM_STATIC_YML_FILE ${CMAKE_CURRENT_SOURCE_DIR}/partition.yml)
        set(OVERLAY_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/boards/dfu.conf)
        set(mcuboot_DTC_OVERLAY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/overlays/mcuboot.overlay)
    else()
        set(PM_STATIC_YML_FILE ${CMAKE_CURRENT_SOURCE_DIR}/partition_zs.yml)
    endif()
endif()

if("nrf5340dk_nrf5340_cpuapp" STREQUAL "${BOARD}")
    set(PM_STATIC_YML_FILE ${CMAKE_CURRENT_SOURCE_DIR}/partition_zs.yml)
endif()

set(BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ZSWatchFW)

add_subdirectory(drivers)
add_subdirectory(src/applications)
add_subdirectory(src/ext_drivers/bmi270)
add_subdirectory(src/ext_drivers/bmp581)

include_directories(src/)
include_directories(src/ui)
include_directories(src/applications)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/ram_retention_storage.c)
target_sources(app PRIVATE src/zsw_clock.c)
target_sources(app PRIVATE src/zsw_cpu_freq.c)

target_sources(app PRIVATE src/drivers/zsw_display_control.c)
target_sources(app PRIVATE src/drivers/zsw_vibration_motor.c)

target_sources(app PRIVATE src/managers/zsw_app_manager.c)
target_sources(app PRIVATE src/managers/zsw_notification_manager.c)
target_sources(app PRIVATE src/managers/zsw_battery_manager.c)
target_sources(app PRIVATE src/managers/zsw_power_manager.c)
target_sources(app PRIVATE src/managers/zsw_phone_app_publisher.c)

target_sources(app PRIVATE src/battery/battery.c)
target_sources(app PRIVATE src/battery/zsw_charger.c)

target_sources(app PRIVATE src/ble/ble_aoa.c)
target_sources(app PRIVATE src/ble/ble_comm.c)
target_sources(app PRIVATE src/ble/ble_transport.c)
target_sources(app PRIVATE src/ble/zsw_gatt_sensor_server.c)
target_sources_ifdef(CONFIG_BLE_USES_AMS app PRIVATE src/ble/ble_ams.c)

target_sources(app PRIVATE src/sensors/zsw_imu.c)
target_sources(app PRIVATE src/sensors/zsw_pressure_sensor.c)
target_sources(app PRIVATE src/sensors/zsw_magnetometer.c)

target_sources(app PRIVATE src/ui/notification/zsw_popup_notifcation.c)
target_sources(app PRIVATE src/ui/popup/zsw_popup_window.c)
target_sources(app PRIVATE src/ui/utils/zsw_ui_utils.c)

target_sources_ifdef(CONFIG_SPI_FLASH_LOADER app PRIVATE src/filesystem/zsw_rtt_flash_loader.c)
target_sources_ifdef(CONFIG_FILE_SYSTEM_LITTLEFS app PRIVATE src/filesystem/zsw_filesystem.c)
target_sources_ifdef(CONFIG_FILE_SYSTEM_LITTLEFS app PRIVATE src/filesystem/zsw_lvgl_spi_decoder.c)

if (DFU_BUILD)
    target_sources(app PRIVATE src/dfu.c)
endif()

FILE(GLOB img_sources src/images/*.c)
target_sources(app PRIVATE ${img_sources})

FILE(GLOB font_sources src/images/fonts/*.c)
target_sources(app PRIVATE ${font_sources})

zephyr_sources_ifdef(CONFIG_WATCHFACE_ANALOG src/ui/watchfaces/zsw_watchface_analog_ui.c)
zephyr_sources_ifdef(CONFIG_WATCHFACE_DIGITAL src/ui/watchfaces/zsw_watchface_digital_ui.c)
zephyr_sources_ifdef(CONFIG_WATCHFACE_MINIMAL src/ui/watchfaces/zsw_watchface_minimal_ui.c)

FILE(GLOB events_sources src/events/*.c)
target_sources(app PRIVATE ${events_sources})

add_compile_definitions(LV_LVGL_H_INCLUDE_SIMPLE)
