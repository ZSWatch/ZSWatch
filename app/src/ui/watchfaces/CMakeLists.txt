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

# Add all watchfaces in this folder.
message("Watchfaces")
FOREACH(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
    include_directories(${subdir})
    message(STATUS ${subdir})
ENDFOREACH()

target_sources(app PRIVATE ${app_sources})