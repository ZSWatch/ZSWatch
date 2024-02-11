FILE(GLOB sensor_fusion_sources *.c)
target_sources(app PRIVATE ${sensor_fusion_sources})

zephyr_include_directories(${PROJECT_SOURCE_DIR}/src/ext_drivers/fusion/Fusion)
zephyr_sources(${PROJECT_SOURCE_DIR}/src/ext_drivers/fusion/Fusion/FusionOffset.c)
zephyr_sources(${PROJECT_SOURCE_DIR}/src/ext_drivers/fusion/Fusion/FusionCompass.c)
zephyr_sources(${PROJECT_SOURCE_DIR}/src/ext_drivers/fusion/Fusion/FusionAhrs.c)
