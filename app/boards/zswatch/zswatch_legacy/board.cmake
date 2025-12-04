# SPDX-License-Identifier: Apache-2.0

if(CONFIG_BOARD_ZSWATCH_LEGACY_NRF5340_CPUAPP_NS)
  set(TFM_PUBLIC_KEY_FORMAT "full")
endif()

if((CONFIG_BOARD_ZSWATCH_LEGACY_NRF5340_CPUAPP OR CONFIG_BOARD_ZSWATCH_LEGACY_NRF5340_CPUAPP_NS) AND CONFIG_BOARD_REVISION_5)
  message(STATUS "Using QSPI ini for MX25U51245G")
  board_runner_args(nrfjprog "--qspiini=${BOARD_DIR}/support/qspi_mx25u51245.ini")
  board_runner_args(jlink "--device=nrf5340_xxaa_app" "--speed=4000")
elseif(CONFIG_BOARD_ZSWATCH_LEGACY_NRF5340_CPUAPP OR CONFIG_BOARD_ZSWATCH_LEGACY_NRF5340_CPUAPP_NS)
  message(STATUS "Using QSPI ini for AT25SL128A")
  board_runner_args(nrfjprog "--qspiini=${BOARD_DIR}/support/qspi_at25sl128a.ini")
  board_runner_args(jlink "--device=nrf5340_xxaa_app" "--speed=4000")
endif()

if(CONFIG_TFM_FLASH_MERGED_BINARY)
  set_property(TARGET runners_yaml_props_target PROPERTY hex_file tfm_merged.hex)
endif()

if(CONFIG_BOARD_ZSWATCH_LEGACY_NRF5340_CPUNET)
  board_runner_args(jlink "--device=nrf5340_xxaa_net" "--speed=4000")
endif()

include(${ZEPHYR_BASE}/boards/common/nrfjprog.board.cmake)
include(${ZEPHYR_BASE}/boards/common/nrfutil.board.cmake)
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
