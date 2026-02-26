# ZSWatch-specific compile/link flags for LLEXT apps
# Include this AFTER the Zephyr EDK cmake.cflags

# XIP flash is far from firmware .text — need indirect calls via GOT
list(APPEND LLEXT_CFLAGS "-mlong-calls")

# ZSWatch headers
list(APPEND LLEXT_CFLAGS "-I${CMAKE_CURRENT_LIST_DIR}/include/zswatch")
list(APPEND LLEXT_CFLAGS "-I${CMAKE_CURRENT_LIST_DIR}/include/zswatch/managers")
list(APPEND LLEXT_CFLAGS "-I${CMAKE_CURRENT_LIST_DIR}/include/zswatch/llext")

# Keep .text.iflash as a separate section for internal flash copy
set(LLEXT_EXTRA_LINK_FLAGS "-Wl,--unique=.text.iflash")
