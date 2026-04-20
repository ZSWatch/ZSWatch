# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

# Board definitions live in app/boards/
set(BOARD_ROOT ${APP_DIR}/../app)

if(PRODUCTION_TEST_FACTORY_BUILD)
    string(REGEX REPLACE "^/" "" board_qualifiers "${BOARD_QUALIFIERS}")
    string(REPLACE "/" "_" board_qualifiers "${board_qualifiers}")

    set(pm_static_yml_file
        "${APP_DIR}/../app/pm_static_${BOARD}_${board_qualifiers}_${BOARD_REVISION}.yml")

    if(NOT EXISTS "${pm_static_yml_file}")
        message(FATAL_ERROR
            "Missing static partition file for production_test factory build: ${pm_static_yml_file}")
    endif()

    set(PM_STATIC_YML_FILE "${pm_static_yml_file}" CACHE INTERNAL "")
endif()

# Apply Zephyr patches before build (same patches as the main app)
function(apply_patches patch_dir target_dir)
    set(full_patch_dir "${APP_DIR}/${patch_dir}")

    file(GLOB_RECURSE files RELATIVE ${full_patch_dir} "${full_patch_dir}/*.patch")

    foreach(file ${files})
        set(full_patch_path "${full_patch_dir}/${file}")

        execute_process(
            COMMAND git apply --reverse --check ${full_patch_path} --unsafe-paths
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
                COMMAND git apply ${full_patch_path} --unsafe-paths
                WORKING_DIRECTORY ${target_dir}
                RESULT_VARIABLE patch_apply_result
            )

            if(NOT patch_apply_result EQUAL 0)
                message(FATAL_ERROR "Failed to apply patch: ${file}")
            endif()
        endif()
    endforeach()
endfunction()

apply_patches("../app/patches/zephyr" $ENV{ZEPHYR_BASE})
