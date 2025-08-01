FILE(GLOB app_sources *.c)
target_sources(app PRIVATE ${app_sources})

add_subdirectory(assets_watchfaces)
add_subdirectory(fonts)
add_subdirectory(weather_icons)
add_subdirectory(icons)
add_subdirectory(gifs)

set(RELOCATE_FILES
    cardinal_point.c
    move.c
    stormy.c
    snowy.c
    rainy.c
    snowy.c
    foggy.c
    sunny.c
    partly_cloudy.c
    cloudy.c
    unknown.c
    assets_watchfaces/face_75_2_dial_3_59132_0.c
    assets_watchfaces/face_75_2_dial_19_89191_0.c
    assets_watchfaces/face_75_2_dial_35_138999_0.c
    ui_img_iaq_co2_text.c
    hour_minimal.c
    minute_minimal.c
    second_minimal.c
)

zephyr_code_relocate(FILES ${RELOCATE_FILES} LOCATION EXTFLASH_RODATA NOCOPY)
zephyr_code_relocate(FILES ${RELOCATE_FILES} LOCATION RAM_DATA)