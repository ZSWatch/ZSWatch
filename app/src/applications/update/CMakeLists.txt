if(CONFIG_MCUMGR)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
endif()