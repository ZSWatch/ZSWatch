if(CONFIG_APPLICATIONS_USE_CALENDAR)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
endif()
