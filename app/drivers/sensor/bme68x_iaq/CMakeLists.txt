if(CONFIG_EXTERNAL_USE_BOSCH_BSEC)
    zephyr_library_compile_definitions_ifdef(CONFIG_BME68X_IAQ_SAMPLE_RATE_ULTRA_LOW_POWER
    BSEC_SAMPLE_RATE=BSEC_SAMPLE_RATE_ULP
    BSEC_SAMPLE_PERIOD_S=300
    )

    zephyr_library_compile_definitions_ifdef(CONFIG_BME68X_IAQ_SAMPLE_RATE_LOW_POWER
    BSEC_SAMPLE_RATE=BSEC_SAMPLE_RATE_LP
    BSEC_SAMPLE_PERIOD_S=3
    )

    zephyr_library_compile_definitions_ifdef(CONFIG_BME68X_IAQ_SAMPLE_RATE_CONTINUOUS
    BSEC_SAMPLE_RATE=BSEC_SAMPLE_RATE_CONT
    BSEC_SAMPLE_PERIOD_S=1
    )

    zephyr_sources(bosch_bme68x_iaq.c)

    # Add the Bosch BSEC2 library to the project.
    zephyr_library_include_directories(${PROJECT_SOURCE_DIR}/src/ext_drivers/BSEC2/algo/normal_version/inc)
    set(LIB_DIR ${PROJECT_SOURCE_DIR}/src/ext_drivers/BSEC2/algo/normal_version/bin/gcc/Cortex_M4)
    add_library(libalgobsec STATIC IMPORTED PRIVATE)
    set_target_properties(libalgobsec PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libalgobsec.a)
    target_link_libraries(app PUBLIC libalgobsec)

    # Add the Bosch BME68X-Sensor-API to the project.
    zephyr_library_include_directories(${PROJECT_SOURCE_DIR}/src/ext_drivers/BME68x-Sensor-API)
    zephyr_sources(${PROJECT_SOURCE_DIR}/src/ext_drivers/BME68x-Sensor-API/bme68x.c)
endif()