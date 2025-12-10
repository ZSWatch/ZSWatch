# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

# Apply patches before build
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

apply_patches("patches/zephyr" $ENV{ZEPHYR_BASE})
apply_patches("patches/ext_drivers" ${APP_DIR}/src/ext_drivers)