if(CONFIG_APPLICATIONS_USE_LEA_ASSISTANT)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
endif()
