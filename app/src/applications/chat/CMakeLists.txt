# Copyright (c) 2026 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_APPLICATIONS_USE_CHAT)
    target_sources(app PRIVATE chat_app.c)
endif()
