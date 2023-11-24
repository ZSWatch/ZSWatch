#pragma once

#define ZSW_BOOT_MODE_RTT_FLASH_LOADER  0x0A
#define ZSW_BOOT_MODE_FLASH_ERASE       0xFF

int zsw_flash_erase_external(void);