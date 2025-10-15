# This basic file is used to compile the runtime for the Editor preview.
# It is also intended for you to customize it for your own target/project.

# Only set project if this is the top-level CMakeLists.txt
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  cmake_minimum_required(VERSION 3.10)
  # can be customized
  project(LVGLProject)
  set(IS_TOP_LEVEL TRUE)
else()
  set(IS_TOP_LEVEL FALSE)
endif()

# This includes the generated list of .c files
include(${CMAKE_CURRENT_LIST_DIR}/file_list_gen.cmake)

# Create the UI sources as a library for Editor preview
if (LV_EDITOR_PREVIEW)
    add_library(lib-ui ${PROJECT_SOURCES})
    # Add any Editor-specific configuration here
endif()

# You may use this check to add configuration when compiling for the Editor preview,
# or for your target.
if (LV_EDITOR_PREVIEW)
    # things for the Preview
else ()
    # things for your target
    # For your target (Zephyr)

    # Create a Zephyr library for LVGL Editor code
    zephyr_library_named(lvgl_editor)
    zephyr_library_sources(${PROJECT_SOURCES})
    zephyr_library_sources(lvgl_editor.c)
    zephyr_library_compile_definitions(LV_LVGL_H_INCLUDE_SIMPLE)
    zephyr_library_compile_definitions(UI_SUBJECT_STRING_LENGTH=32)

    # Automatically extract include directories from source files
    # This is a bit of a hack, but lvgl_editor does not list the include dirs it needs
    # So we just assume headers are placed together with source files
    set(LVGL_EDITOR_INCLUDE_DIRS "")
    foreach(source_file ${PROJECT_SOURCES})
        get_filename_component(source_dir ${source_file} DIRECTORY)
        list(APPEND LVGL_EDITOR_INCLUDE_DIRS ${source_dir})
    endforeach()

    # Remove duplicates
    list(REMOVE_DUPLICATES LVGL_EDITOR_INCLUDE_DIRS)

    # Add include directories to the library
    zephyr_library_include_directories(${LVGL_EDITOR_INCLUDE_DIRS})

    # Make includes available to app target
    target_include_directories(app PUBLIC ${LVGL_EDITOR_INCLUDE_DIRS})

    # Link app to the LVGL Editor library, needed to bring in LVGL includes
    target_link_libraries(app PUBLIC lvgl_editor)

    # Relocate to external flash to save internal flash space
    # For now all source files and images are relocated,
    # we may want to be more selective in the future
    list(APPEND RELOCATE_FILES ${PROJECT_SOURCES})
    set(RELOCATE_FILES ${RELOCATE_FILES} PARENT_SCOPE)
endif ()
