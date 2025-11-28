cmake_minimum_required(VERSION 3.20.0)

# Use the same board definitions as main app
set(BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../app)
list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../app)

# Add custom device tree bindings from main app
list(APPEND DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../app)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ProductionTest)

# Shared components from main app
target_sources(app PRIVATE
    ../app/src/drivers/zsw_display_control.c
)

target_sources(app PRIVATE
    ../app/src/drivers/zsw_vibration_motor.c
)

target_sources(app PRIVATE
    ../app/src/drivers/zsw_microphone.c
)

add_subdirectory(../app/drivers ${CMAKE_CURRENT_BINARY_DIR}/app_drivers)

if (CONFIG_AUDIO_DMIC)
    target_sources(app PRIVATE
        ../app/src/ext_drivers/kissfft/kiss_fft.c
        ../app/src/ext_drivers/kissfft/kiss_fftr.c
    )

    target_include_directories(app PRIVATE ../app/src/ext_drivers/kissfft)

    target_compile_definitions(app PRIVATE
        kiss_fft_scalar=float
        KISS_FFT_MALLOC=k_malloc
        KISS_FFT_FREE=k_free
    )
endif()

target_sources(app PRIVATE 
    src/main.c
    src/production_test_runner.c
    src/screens/button_test_screen.c
    src/screens/vibration_test_screen.c
    src/screens/brightness_test_screen.c
    src/screens/touch_test_screen.c
    src/screens/microphone_test_screen.c
    src/screens/sensor_scan_screen.c
    src/screens/result_screen.c
    
    # FFT spectrum analyzer for microphone test
    ../app/src/applications/mic/spectrum_analyzer.c
)

target_include_directories(app PRIVATE 
    ../app/src/
    ../app/src/drivers/
)

target_compile_definitions(app PRIVATE
    _POSIX_C_SOURCE=200809L
)
