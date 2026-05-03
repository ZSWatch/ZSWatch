if(CONFIG_WATCHFACE_DIGITAL_MINIMAL)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
endif(CONFIG_WATCHFACE_DIGITAL_MINIMAL)
