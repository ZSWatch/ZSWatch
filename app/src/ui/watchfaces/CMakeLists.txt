# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

macro(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        SET(dirlist ${dirlist} ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
endmacro()

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# Add all watchfaces in this folder.
message("Watchfaces")
FOREACH(subdir ${SUBDIRS})
  add_subdirectory(${subdir})
  include_directories(${subdir})
  message(STATUS ${subdir})
ENDFOREACH()

target_sources(app PRIVATE ${app_sources})
target_sources(app PRIVATE zsw_ui_notification_area.c)
target_sources(app PRIVATE zsw_watchface_dropdown_ui.c)
target_sources(app PRIVATE zsw_watchface_anim.c)

# Move only the code that creates a watchface (each face's *_show() and the dropdown)
# to external flash. It runs when the face opens, with XIP enabled by open_watchface().
# Per-second setters, animation callbacks and rodata stay internal: they can run
# around screen-off, when XIP is disabled.
if(CONFIG_ZSW_XIP)
  get_target_property(app_all_sources app SOURCES)
  list(FILTER app_all_sources INCLUDE REGEX "/watchfaces/.*/zsw_watchface_.*_ui\\.c$")
  list(APPEND app_all_sources ${CMAKE_CURRENT_SOURCE_DIR}/zsw_watchface_dropdown_ui.c)
  zephyr_code_relocate(FILES ${app_all_sources} FILTER "\\.text\\..*(_show|zsw_watchface_dropdown_ui_add)$"
                       LOCATION EXTFLASH_TEXT NOCOPY)
endif()
