if (CONFIG_RTC_ALARM)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
endif()