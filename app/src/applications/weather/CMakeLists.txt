if (CONFIG_ZSWATCH_PCB_REV GREATER_EQUAL 4 OR CONFIG_BOARD_NATIVE_POSIX)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
endif()