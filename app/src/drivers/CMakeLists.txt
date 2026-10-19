target_sources(app PRIVATE zsw_display_control.c)
target_sources(app PRIVATE zsw_vibration_motor.c)
target_sources_ifdef(CONFIG_DT_HAS_MAXIM_MAX32664C_ENABLED app PRIVATE zsw_hr.c)
target_sources_ifdef(CONFIG_AUDIO_DMIC app PRIVATE zsw_microphone.c)

if(CONFIG_NORDIC_QSPI_NOR)
    target_sources(app PRIVATE zsw_qspi_reboot.c)
    # Let zsw_qspi_reboot.c leave 4-byte flash addressing before every reset.
    zephyr_ld_options(-Wl,--wrap=sys_arch_reboot)
endif()
