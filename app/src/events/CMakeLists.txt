FILE(GLOB event_sources *.c)
target_sources(app PRIVATE ${event_sources})