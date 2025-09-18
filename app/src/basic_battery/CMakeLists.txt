# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

FILE(GLOB battery_sources *.c)
target_sources(app PRIVATE ${battery_sources})
