FILE(GLOB app_sources *.c)
target_sources(app PRIVATE ${app_sources})

add_subdirectory(assets_watchfaces)
add_subdirectory(fonts)
add_subdirectory(weather_icons)
add_subdirectory(icons)