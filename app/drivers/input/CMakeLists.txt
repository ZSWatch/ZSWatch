# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

add_subdirectory_ifdef(CONFIG_INPUT_CST816S cst816s)
zephyr_sources_ifdef(CONFIG_INPUT_NPM13XX input_npm13xx.c)
