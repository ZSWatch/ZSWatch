macro(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        SET(dirlist ${dirlist} ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
endmacro()

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# Add all folders (applications) in this folder.
message("Applications")
FOREACH(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
    include_directories(${subdir})
    message(STATUS ${subdir})
ENDFOREACH()

# Collect RELOCATE_FILES from all subdirectories and relocate them
if(RELOCATE_FILES)
  zephyr_code_relocate(FILES ${RELOCATE_FILES} LOCATION EXTFLASH_TEXT NOCOPY)
  zephyr_code_relocate(FILES ${RELOCATE_FILES} LOCATION EXTFLASH_RODATA NOCOPY)
  zephyr_code_relocate(FILES ${RELOCATE_FILES} LOCATION RAM_DATA)
endif()

set(RELOCATE_FILES ${RELOCATE_FILES} ${app_sources} PARENT_SCOPE)

target_sources(app PRIVATE ${app_sources})