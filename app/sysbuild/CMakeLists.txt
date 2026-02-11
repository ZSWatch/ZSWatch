# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

# Set BOARD_ROOT before find_package(Sysbuild) so that the board resolution
# picks up the custom board definitions in app/boards/.
# CMAKE_CURRENT_LIST_DIR points to app/sysbuild/, so .. resolves to app/.
set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)

find_package(Sysbuild REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(sysbuild LANGUAGES)
