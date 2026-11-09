FILE(GLOB app_sources *.c)
target_sources(app PRIVATE ${app_sources})

add_subdirectory(assets_watchfaces)
add_subdirectory(fonts)

set(RELOCATE_FILES
    cardinal_point.c
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
    hour_minimal.c
    minute_minimal.c
    second_minimal.c
    foggy.c
    rainy.c
    snowy.c
    stormy.c
    sunny.c
    cloudy.c
    partly_cloudy.c
    unknown.c
)

zephyr_code_relocate(FILES ${RELOCATE_FILES} LOCATION EXTFLASH_RODATA NOCOPY)
zephyr_code_relocate(FILES ${RELOCATE_FILES} LOCATION RAM_DATA)