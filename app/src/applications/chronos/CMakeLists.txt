if(0)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
    add_subdirectory(assets)
    add_subdirectory(screens)

    # Export none critical files for relocation to save flash
    set(RELOCATE_FILES ${RELOCATE_FILES} ${app_sources} PARENT_SCOPE)
endif()