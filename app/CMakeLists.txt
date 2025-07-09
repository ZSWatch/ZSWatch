cmake_minimum_required(VERSION 3.20.0)

# Apply patches before build
function(apply_patches patch_dir target_dir)
    file(GLOB_RECURSE files RELATIVE ${CMAKE_SOURCE_DIR} "${patch_dir}/*.patch")
    foreach(file ${files})
        execute_process(
            COMMAND git apply --reverse --check ${CMAKE_CURRENT_SOURCE_DIR}/${file} --unsafe-paths
            WORKING_DIRECTORY ${target_dir}
            RESULT_VARIABLE patch_already_applied
            OUTPUT_QUIET
            ERROR_QUIET
        )
        if(patch_already_applied EQUAL 0)
            message("Patch already applied: ${file}")
        else()
            message("Applying patch: ${file}")
            execute_process(
                COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/${file} --unsafe-paths
                WORKING_DIRECTORY ${target_dir}
                RESULT_VARIABLE patch_apply_result
            )
            if(NOT patch_apply_result EQUAL 0)
                message(FATAL_ERROR "Failed to apply patch: ${file}")
            endif()
        endif()
    endforeach()
endfunction()

apply_patches("patches/zephyr" $ENV{ZEPHYR_BASE})
apply_patches("patches/ext_drivers" ${CMAKE_SOURCE_DIR}/src/ext_drivers)

# Get the target hardware
string(REGEX MATCH "zswatch@([0-9]+)/nrf5340.*" ZSWATCH_BOARD "${BOARD}")
if(ZSWATCH_BOARD)
    string(CONCAT VERSION_STRING "\"" ${CMAKE_MATCH_1} "\"")
    set(CONFIG_ZSWATCH_PCB_REV ${CMAKE_MATCH_1} CACHE INTERNAL "")
    set(CONFIG_BT_DIS_HW_REV_STR ${VERSION_STRING} CACHE INTERNAL "")

    message("PCB revision: " ${CONFIG_ZSWATCH_PCB_REV})
endif()

set(BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

macro(app_set_runner_args)
    if (CONFIG_BOARD_ZSWATCH_NRF5340_CPUAPP AND CONFIG_BOARD_REVISION STREQUAL "5") 
        board_runner_args(nrfjprog "--qspiini=${CMAKE_CURRENT_SOURCE_DIR}/qspi_mx25u51245.ini")
    elseif(CONFIG_BOARD_ZSWATCH_NRF5340_CPUAPP AND CONFIG_BOARD_REVISION STREQUAL "4")
        board_runner_args(nrfjprog "--qspiini=${CMAKE_CURRENT_SOURCE_DIR}/qspi_at25sl128a.ini")
    else ()
        message("Uploading firmware to external flash may fail. Default ini will be used.")
    endif()
endmacro()

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ZSWatchFW)

zephyr_compile_definitions(
  -DNRF53_ERRATA_159_ENABLE_WORKAROUND=0 # Not allowing 128MHz CPU with QPSI at same time. We take the risk of this errata that may cause currupt QPSI data.
)

add_subdirectory(drivers)
add_subdirectory(src/history)
add_subdirectory(src/applications)
add_subdirectory(src/ui/watchfaces)
add_subdirectory(src/sensors)
add_subdirectory(src/sensor_fusion)
add_subdirectory(src/events)
add_subdirectory(src/managers)
add_subdirectory(src/drivers)
add_subdirectory(src/ble)
add_subdirectory(src/images)

include_directories(src/)
include_directories(src/ui)
include_directories(src/applications)
include_directories(src/ui/watchfaces)

if (CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED)
    add_subdirectory(src/fuel_gauge)
else()
    add_subdirectory(src/basic_battery)
endif()

target_sources(app PRIVATE src/zsw_clock.c)
target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/zsw_cpu_freq.c)
target_sources(app PRIVATE src/zsw_retained_ram_storage.c)
target_sources(app PRIVATE src/zsw_coredump.c)

target_sources(app PRIVATE src/ui/notification/zsw_popup_notifcation.c)
target_sources(app PRIVATE src/ui/popup/zsw_popup_window.c)
target_sources(app PRIVATE src/ui/utils/zsw_ui_utils.c)

target_sources(app PRIVATE src/ui/watchfaces/zsw_watchface_dropdown_ui.c)

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
# Placing the mcumgr code in external flash causes QSPI assert. For now keeping it in iinternal flash.
if (CONFIG_MCUMGR)
    zephyr_code_relocate(LIBRARY subsys__mgmt__mcumgr__mgmt LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(LIBRARY subsys__mgmt__mcumgr__util LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(LIBRARY subsys__mgmt__mcumgr__smp LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(LIBRARY subsys__mgmt__mcumgr__transport LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(LIBRARY subsys__mgmt__mcumgr__grp__img_mgmt LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(LIBRARY subsys__mgmt__mcumgr__grp__os_mgmt LOCATION EXTFLASH_TEXT NOCOPY)
endif()

zephyr_code_relocate(LIBRARY modules__lvgl LOCATION EXTFLASH_TEXT NOCOPY)
zephyr_code_relocate(LIBRARY modules__lvgl LOCATION RAM_DATA)

zephyr_code_relocate(LIBRARY drivers__bluetooth LOCATION EXTFLASH_TEXT NOCOPY)
zephyr_code_relocate(LIBRARY drivers__bluetooth LOCATION RAM_DATA)

#zephyr_code_relocate(LIBRARY subsys__bluetooth__host LOCATION EXTFLASH_TEXT NOCOPY)
#zephyr_code_relocate(LIBRARY subsys__bluetooth__host LOCATION RAM_DATA)

zephyr_code_relocate(LIBRARY subsys__bluetooth__crypto LOCATION EXTFLASH_TEXT NOCOPY)
zephyr_code_relocate(LIBRARY subsys__bluetooth__crypto LOCATION RAM_DATA)

if (CONFIG_USB_DEVICE_STACK)
    zephyr_code_relocate(LIBRARY drivers__usb__device LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(LIBRARY drivers__usb__device LOCATION RAM_DATA)

    zephyr_code_relocate(LIBRARY drivers__usb__common__nrf_usbd_common LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(LIBRARY drivers__usb__common__nrf_usbd_common LOCATION RAM_DATA)
endif()

if (CONFIG_BME680)
    zephyr_code_relocate(LIBRARY drivers__sensor__bosch__bme680 LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(LIBRARY drivers__sensor__bosch__bme680 LOCATION RAM_DATA)
endif()

if (CONFIG_LIS2MDL)
    zephyr_code_relocate(LIBRARY drivers__sensor__st__lis2mdl LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(LIBRARY drivers__sensor__st__lis2mdl LOCATION RAM_DATA)
    zephyr_code_relocate(LIBRARY drivers__sensor__st__stmemsc LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(LIBRARY drivers__sensor__st__stmemsc LOCATION RAM_DATA)
endif()
