FILE(GLOB driver_sources *.c)
target_sources(app PRIVATE ${driver_sources})