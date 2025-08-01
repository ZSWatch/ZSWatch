if (CONFIG_ZSWATCH_PCB_REV GREATER_EQUAL 4 OR CONFIG_ARCH_POSIX)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})

    # Export none critical files for relocation to save flash
    set(RELOCATE_FILES ${RELOCATE_FILES} ${app_sources} PARENT_SCOPE)
endif()