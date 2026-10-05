#pragma once
#include <stddef.h>
#include <stdint.h>
#define __weak __attribute__((weak))
uint32_t crc32_ieee(const uint8_t *data, size_t length);
uint32_t crc32_ieee_update(uint32_t crc, const uint8_t *data, size_t length);
