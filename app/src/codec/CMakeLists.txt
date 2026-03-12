# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

# ZSWatch CMakeLists.txt wrapping the upstream Opus submodule.
# Selects only the source files needed for fixed-point encoding.

if(CONFIG_ZSW_OPUS_CODEC)

set(OPUS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ext_drivers/opus)

zephyr_library_named(opus_codec)

# --- CELT sources (core) ---
set(opus_celt_sources
    ${OPUS_DIR}/celt/bands.c
    ${OPUS_DIR}/celt/celt.c
    ${OPUS_DIR}/celt/celt_encoder.c
    ${OPUS_DIR}/celt/celt_decoder.c
    ${OPUS_DIR}/celt/cwrs.c
    ${OPUS_DIR}/celt/entcode.c
    ${OPUS_DIR}/celt/entdec.c
    ${OPUS_DIR}/celt/entenc.c
    ${OPUS_DIR}/celt/kiss_fft.c
    ${OPUS_DIR}/celt/laplace.c
    ${OPUS_DIR}/celt/mathops.c
    ${OPUS_DIR}/celt/mdct.c
    ${OPUS_DIR}/celt/modes.c
    ${OPUS_DIR}/celt/pitch.c
    ${OPUS_DIR}/celt/celt_lpc.c
    ${OPUS_DIR}/celt/quant_bands.c
    ${OPUS_DIR}/celt/rate.c
    ${OPUS_DIR}/celt/vq.c
)

# --- SILK sources ---
set(opus_silk_sources
    ${OPUS_DIR}/silk/CNG.c
    ${OPUS_DIR}/silk/code_signs.c
    ${OPUS_DIR}/silk/init_decoder.c
    ${OPUS_DIR}/silk/decode_core.c
    ${OPUS_DIR}/silk/decode_frame.c
    ${OPUS_DIR}/silk/decode_parameters.c
    ${OPUS_DIR}/silk/decode_indices.c
    ${OPUS_DIR}/silk/decode_pulses.c
    ${OPUS_DIR}/silk/decoder_set_fs.c
    ${OPUS_DIR}/silk/dec_API.c
    ${OPUS_DIR}/silk/enc_API.c
    ${OPUS_DIR}/silk/encode_indices.c
    ${OPUS_DIR}/silk/encode_pulses.c
    ${OPUS_DIR}/silk/gain_quant.c
    ${OPUS_DIR}/silk/interpolate.c
    ${OPUS_DIR}/silk/LP_variable_cutoff.c
    ${OPUS_DIR}/silk/NLSF_decode.c
    ${OPUS_DIR}/silk/NSQ.c
    ${OPUS_DIR}/silk/NSQ_del_dec.c
    ${OPUS_DIR}/silk/PLC.c
    ${OPUS_DIR}/silk/shell_coder.c
    ${OPUS_DIR}/silk/tables_gain.c
    ${OPUS_DIR}/silk/tables_LTP.c
    ${OPUS_DIR}/silk/tables_NLSF_CB_NB_MB.c
    ${OPUS_DIR}/silk/tables_NLSF_CB_WB.c
    ${OPUS_DIR}/silk/tables_other.c
    ${OPUS_DIR}/silk/tables_pitch_lag.c
    ${OPUS_DIR}/silk/tables_pulses_per_block.c
    ${OPUS_DIR}/silk/VAD.c
    ${OPUS_DIR}/silk/control_audio_bandwidth.c
    ${OPUS_DIR}/silk/quant_LTP_gains.c
    ${OPUS_DIR}/silk/VQ_WMat_EC.c
    ${OPUS_DIR}/silk/HP_variable_cutoff.c
    ${OPUS_DIR}/silk/NLSF_encode.c
    ${OPUS_DIR}/silk/NLSF_VQ.c
    ${OPUS_DIR}/silk/NLSF_unpack.c
    ${OPUS_DIR}/silk/NLSF_del_dec_quant.c
    ${OPUS_DIR}/silk/process_NLSFs.c
    ${OPUS_DIR}/silk/stereo_LR_to_MS.c
    ${OPUS_DIR}/silk/stereo_MS_to_LR.c
    ${OPUS_DIR}/silk/check_control_input.c
    ${OPUS_DIR}/silk/control_SNR.c
    ${OPUS_DIR}/silk/init_encoder.c
    ${OPUS_DIR}/silk/control_codec.c
    ${OPUS_DIR}/silk/A2NLSF.c
    ${OPUS_DIR}/silk/ana_filt_bank_1.c
    ${OPUS_DIR}/silk/biquad_alt.c
    ${OPUS_DIR}/silk/bwexpander_32.c
    ${OPUS_DIR}/silk/bwexpander.c
    ${OPUS_DIR}/silk/debug.c
    ${OPUS_DIR}/silk/decode_pitch.c
    ${OPUS_DIR}/silk/inner_prod_aligned.c
    ${OPUS_DIR}/silk/lin2log.c
    ${OPUS_DIR}/silk/log2lin.c
    ${OPUS_DIR}/silk/LPC_analysis_filter.c
    ${OPUS_DIR}/silk/LPC_inv_pred_gain.c
    ${OPUS_DIR}/silk/table_LSF_cos.c
    ${OPUS_DIR}/silk/NLSF2A.c
    ${OPUS_DIR}/silk/NLSF_stabilize.c
    ${OPUS_DIR}/silk/NLSF_VQ_weights_laroia.c
    ${OPUS_DIR}/silk/pitch_est_tables.c
    ${OPUS_DIR}/silk/resampler.c
    ${OPUS_DIR}/silk/resampler_down2_3.c
    ${OPUS_DIR}/silk/resampler_down2.c
    ${OPUS_DIR}/silk/resampler_private_AR2.c
    ${OPUS_DIR}/silk/resampler_private_down_FIR.c
    ${OPUS_DIR}/silk/resampler_private_IIR_FIR.c
    ${OPUS_DIR}/silk/resampler_private_up2_HQ.c
    ${OPUS_DIR}/silk/resampler_rom.c
    ${OPUS_DIR}/silk/sigm_Q15.c
    ${OPUS_DIR}/silk/sort.c
    ${OPUS_DIR}/silk/sum_sqr_shift.c
    ${OPUS_DIR}/silk/stereo_decode_pred.c
    ${OPUS_DIR}/silk/stereo_encode_pred.c
    ${OPUS_DIR}/silk/stereo_find_predictor.c
    ${OPUS_DIR}/silk/stereo_quant_pred.c
    ${OPUS_DIR}/silk/LPC_fit.c
)

# --- SILK fixed-point sources ---
set(opus_silk_fixed_sources
    ${OPUS_DIR}/silk/fixed/LTP_analysis_filter_FIX.c
    ${OPUS_DIR}/silk/fixed/LTP_scale_ctrl_FIX.c
    ${OPUS_DIR}/silk/fixed/corrMatrix_FIX.c
    ${OPUS_DIR}/silk/fixed/encode_frame_FIX.c
    ${OPUS_DIR}/silk/fixed/find_LPC_FIX.c
    ${OPUS_DIR}/silk/fixed/find_LTP_FIX.c
    ${OPUS_DIR}/silk/fixed/find_pitch_lags_FIX.c
    ${OPUS_DIR}/silk/fixed/find_pred_coefs_FIX.c
    ${OPUS_DIR}/silk/fixed/noise_shape_analysis_FIX.c
    ${OPUS_DIR}/silk/fixed/process_gains_FIX.c
    ${OPUS_DIR}/silk/fixed/regularize_correlations_FIX.c
    ${OPUS_DIR}/silk/fixed/residual_energy16_FIX.c
    ${OPUS_DIR}/silk/fixed/residual_energy_FIX.c
    ${OPUS_DIR}/silk/fixed/warped_autocorrelation_FIX.c
    ${OPUS_DIR}/silk/fixed/apply_sine_window_FIX.c
    ${OPUS_DIR}/silk/fixed/autocorr_FIX.c
    ${OPUS_DIR}/silk/fixed/burg_modified_FIX.c
    ${OPUS_DIR}/silk/fixed/k2a_FIX.c
    ${OPUS_DIR}/silk/fixed/k2a_Q16_FIX.c
    ${OPUS_DIR}/silk/fixed/pitch_analysis_core_FIX.c
    ${OPUS_DIR}/silk/fixed/vector_ops_FIX.c
    ${OPUS_DIR}/silk/fixed/schur64_FIX.c
    ${OPUS_DIR}/silk/fixed/schur_FIX.c
)

# --- Opus API sources ---
set(opus_api_sources
    ${OPUS_DIR}/src/opus.c
    ${OPUS_DIR}/src/opus_decoder.c
    ${OPUS_DIR}/src/opus_encoder.c
    ${OPUS_DIR}/src/extensions.c
    ${OPUS_DIR}/src/opus_multistream.c
    ${OPUS_DIR}/src/opus_multistream_encoder.c
    ${OPUS_DIR}/src/opus_multistream_decoder.c
    ${OPUS_DIR}/src/repacketizer.c
    ${OPUS_DIR}/src/opus_projection_encoder.c
    ${OPUS_DIR}/src/opus_projection_decoder.c
    ${OPUS_DIR}/src/mapping_matrix.c
)

# Combine all sources
set(opus_all_sources
    ${opus_celt_sources}
    ${opus_silk_sources}
    ${opus_silk_fixed_sources}
    ${opus_api_sources}
)

zephyr_library_sources(${opus_all_sources})

# Include paths for the opus library itself
zephyr_library_include_directories(
    ${OPUS_DIR}
    ${OPUS_DIR}/include
    ${OPUS_DIR}/celt
    ${OPUS_DIR}/silk
    ${OPUS_DIR}/silk/fixed
    ${OPUS_DIR}/src
)

# Expose include/ to the rest of the app for opus.h
zephyr_include_directories(
    ${OPUS_DIR}/include
)

# Common compile definitions for fixed-point, no-float build
target_compile_definitions(opus_codec PRIVATE
    OPUS_BUILD
    USE_ALLOCA
    HAVE_ALLOCA_H
    FIXED_POINT
    DISABLE_FLOAT_API
    HAVE_LRINT
    HAVE_LRINTF
)

# Suppress warnings from third-party code
target_compile_options(opus_codec PRIVATE
    -Wno-stringop-overread
    -Wno-maybe-uninitialized
    -Wno-sign-compare
    -Wno-unused-variable
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-implicit-fallthrough
    -Wno-parentheses
)

# --- ARM-specific C intrinsics (no RVCT assembly) ---
if(CONFIG_ARM)
    zephyr_library_sources(
        ${OPUS_DIR}/celt/arm/armcpu.c
        ${OPUS_DIR}/celt/arm/arm_celt_map.c
    )
    target_compile_definitions(opus_codec PRIVATE
        OPUS_ARM_INLINE_ASM
        OPUS_ARM_INLINE_EDSP
        OPUS_ARM_INLINE_MEDIA
    )
    target_compile_options(opus_codec PRIVATE
        -fsingle-precision-constant
    )
endif()

# --- XIP Relocation for hardware builds ---
if(CONFIG_ZSW_XIP)
    zephyr_code_relocate(FILES ${opus_all_sources} LOCATION EXTFLASH_TEXT NOCOPY)
    zephyr_code_relocate(FILES ${opus_all_sources} LOCATION EXTFLASH_RODATA NOCOPY)
endif()

# --- ZSWatch audio codec wrapper ---
# Add wrapper to the opus_codec library so all opus symbols are resolved together
zephyr_library_sources(${CMAKE_CURRENT_SOURCE_DIR}/zsw_audio_codec.c)
zephyr_library_include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# Expose header to the rest of the app
# Ensure app target can link against opus_codec symbols (needed for shell commands etc.)
target_link_libraries(app PRIVATE opus_codec)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR})

endif() # CONFIG_ZSW_OPUS_CODEC
