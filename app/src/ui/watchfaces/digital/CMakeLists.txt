if(CONFIG_WATCHFACE_DIGITAL)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
endif()