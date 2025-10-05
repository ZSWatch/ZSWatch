# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

target_sources(app PRIVATE zsw_app_manager.c)
target_sources(app PRIVATE zsw_notification_manager.c)
target_sources(app PRIVATE zsw_phone_app_publisher.c)
target_sources(app PRIVATE zsw_power_manager.c)
target_sources_ifdef(CONFIG_AUDIO_DMIC app PRIVATE zsw_microphone_manager.c)
target_sources(app PRIVATE zsw_usb_manager.c)
target_sources(app PRIVATE zsw_xip_manager.c)
