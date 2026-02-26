# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0
#
# Toolchain file for building ZSWatch LLEXT apps with the Zephyr SDK.
# The ZEPHYR_SDK_INSTALL_DIR environment variable must point to the SDK root.

if(DEFINED ENV{ZEPHYR_SDK_INSTALL_DIR})
    set(SDK_DIR $ENV{ZEPHYR_SDK_INSTALL_DIR})
    set(CMAKE_C_COMPILER   ${SDK_DIR}/arm-zephyr-eabi/bin/arm-zephyr-eabi-gcc)
    set(CMAKE_FIND_ROOT_PATH ${SDK_DIR}/arm-zephyr-eabi)
else()
    # Fall back to PATH lookup
    set(CMAKE_C_COMPILER   arm-zephyr-eabi-gcc)
endif()
