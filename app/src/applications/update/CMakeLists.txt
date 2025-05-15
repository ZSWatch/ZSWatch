if(NOT CONFIG_BOARD_NATIVE_POSIX)
FILE(GLOB app_sources *.c)
target_sources(app PRIVATE ${app_sources})
endif()