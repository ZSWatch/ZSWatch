if(CONFIG_APPLICATIONS_USE_LEA_ASSISTANT)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
    # Internal BT headers needed for PAST support
    target_include_directories(app PRIVATE ${ZEPHYR_BASE}/subsys/bluetooth)
endif()
