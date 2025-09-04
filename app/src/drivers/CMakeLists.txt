target_sources(app PRIVATE zsw_display_control.c)
target_sources(app PRIVATE zsw_vibration_motor.c)
target_sources_ifdef(CONFIG_AUDIO_DMIC app PRIVATE zsw_microphone.c)