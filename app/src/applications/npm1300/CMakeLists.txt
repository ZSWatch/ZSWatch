if (CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED OR CONFIG_BOARD_NATIVE_POSIX)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
endif()