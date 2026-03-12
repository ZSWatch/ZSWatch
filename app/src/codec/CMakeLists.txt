# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

# ZSWatch CMakeLists.txt wrapping the upstream Opus submodule.
# Source file lists are derived from opus's own .mk files via OpusSources.cmake.

if(CONFIG_ZSW_OPUS_CODEC)

set(OPUS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ext_drivers/opus)

list(APPEND CMAKE_MODULE_PATH ${OPUS_DIR}/cmake)
include(OpusFunctions)

get_opus_sources(CELT_SOURCES ${OPUS_DIR}/celt_sources.mk celt_sources)
get_opus_sources(SILK_SOURCES ${OPUS_DIR}/silk_sources.mk silk_sources)
get_opus_sources(SILK_SOURCES_FIXED ${OPUS_DIR}/silk_sources.mk silk_sources_fixed)
get_opus_sources(OPUS_SOURCES ${OPUS_DIR}/opus_sources.mk opus_sources)

# Paths from .mk files are relative to opus root — make them absolute
list(TRANSFORM celt_sources PREPEND "${OPUS_DIR}/")
list(TRANSFORM silk_sources PREPEND "${OPUS_DIR}/")
list(TRANSFORM silk_sources_fixed PREPEND "${OPUS_DIR}/")
list(TRANSFORM opus_sources PREPEND "${OPUS_DIR}/")

zephyr_library_named(opus_codec)

set(opus_all_sources
    ${celt_sources}
    ${silk_sources}
    ${silk_sources_fixed}
    ${opus_sources}
)

zephyr_library_sources(${opus_all_sources})

# Include paths for the opus library itself
zephyr_library_include_directories(
    ${OPUS_DIR}
    ${OPUS_DIR}/include
    ${OPUS_DIR}/celt
    ${OPUS_DIR}/silk
    ${OPUS_DIR}/silk/fixed
    ${OPUS_DIR}/src
)

# Expose include/ to the rest of the app for opus.h
zephyr_include_directories(
    ${OPUS_DIR}/include
)

# Common compile definitions for fixed-point, no-float build
target_compile_definitions(opus_codec PRIVATE
    OPUS_BUILD
    USE_ALLOCA
    HAVE_ALLOCA_H
    FIXED_POINT
    DISABLE_FLOAT_API
    HAVE_LRINT
    HAVE_LRINTF
)

# Suppress warnings from third-party code
target_compile_options(opus_codec PRIVATE
    -Wno-stringop-overread
    -Wno-maybe-uninitialized
    -Wno-sign-compare
    -Wno-unused-variable
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-implicit-fallthrough
    -Wno-parentheses
)

# --- ARM-specific C intrinsics (no RVCT assembly) ---
if(CONFIG_ARM)
    zephyr_library_sources(
        ${OPUS_DIR}/celt/arm/armcpu.c
        ${OPUS_DIR}/celt/arm/arm_celt_map.c
    )
    target_compile_definitions(opus_codec PRIVATE
        OPUS_ARM_INLINE_ASM
        OPUS_ARM_INLINE_EDSP
        OPUS_ARM_INLINE_MEDIA
    )
    target_compile_options(opus_codec PRIVATE
        -fsingle-precision-constant
    )
endif()

# --- XIP Relocation for hardware builds ---
if(CONFIG_ZSW_XIP)
    zephyr_code_relocate(FILES ${opus_all_sources} LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(FILES ${opus_all_sources} LOCATION EXTFLASH_RODATA NOCOPY)
endif()

# --- ZSWatch audio codec wrapper ---
# Add wrapper to the opus_codec library so all opus symbols are resolved together
zephyr_library_sources(${CMAKE_CURRENT_SOURCE_DIR}/zsw_audio_codec.c)
zephyr_library_include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# Expose header to the rest of the app
target_link_libraries(app PRIVATE opus_codec)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR})

endif() # CONFIG_ZSW_OPUS_CODEC
