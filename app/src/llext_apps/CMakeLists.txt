# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_ZSW_LLEXT_APPS)
    target_sources(app PRIVATE zsw_llext_exports.c)

    # ---- about_ext LLEXT app (with icon image via #include) ----
    add_llext_target(about_ext
        OUTPUT  ${PROJECT_BINARY_DIR}/llext/about_ext.llext
        SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/about_ext/about_ext_app.c
    )
    target_include_directories(about_ext_llext_lib PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/managers
    )

    # ---- battery_ext LLEXT app (chart + background zbus + icon) ----
    add_llext_target(battery_ext
        OUTPUT  ${PROJECT_BINARY_DIR}/llext/battery_ext.llext
        SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/battery_ext/battery_ext_app.c
    )
    target_include_directories(battery_ext_llext_lib PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/managers
    )

    # Generate C include file from about_ext for embedded testing
    generate_inc_file_for_target(app
        ${PROJECT_BINARY_DIR}/llext/about_ext.llext
        ${ZEPHYR_BINARY_DIR}/include/generated/about_ext.inc
    )

    # Copy built .llext files to LFS source directory for west upload_fs
    set(LFS_APPS_DIR ${CMAKE_SOURCE_DIR}/src/images/binaries/lvgl_lfs/apps)
    add_custom_command(TARGET about_ext POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${PROJECT_BINARY_DIR}/llext/about_ext.llext
            ${LFS_APPS_DIR}/about_ext/app.llext
        COMMENT "Copying about_ext.llext to LFS source"
    )
    add_custom_command(TARGET battery_ext POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${PROJECT_BINARY_DIR}/llext/battery_ext.llext
            ${LFS_APPS_DIR}/battery_ext/app.llext
        COMMENT "Copying battery_ext.llext to LFS source"
    )

    # Ensure LLEXT apps are built as part of the main firmware build
    add_dependencies(app about_ext battery_ext)
endif()
