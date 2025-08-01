FILE(GLOB font_sources *.c)
target_sources(app PRIVATE ${font_sources})

zephyr_code_relocate(FILES ${font_sources} LOCATION EXTFLASH_RODATA NOCOPY)
zephyr_code_relocate(FILES ${font_sources} LOCATION RAM_DATA)