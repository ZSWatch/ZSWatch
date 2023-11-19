FILE(GLOB sensor_sources *.c)
target_sources(app PRIVATE ${sensor_sources})