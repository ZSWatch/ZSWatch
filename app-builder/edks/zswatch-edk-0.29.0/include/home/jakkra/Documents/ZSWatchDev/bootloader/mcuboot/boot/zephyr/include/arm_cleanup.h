
/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_ARM_CLEANUP_
#define H_ARM_CLEANUP_

/**
 * Cleanup interrupt priority and interupt enable registers.
 */
void cleanup_arm_interrupts(void);

#if defined(CONFIG_CPU_HAS_ARM_MPU) || defined(CONFIG_CPU_HAS_NXP_SYSMPU)
/**
 * Cleanup all ARM MPU region configuration
 */
void z_arm_clear_arm_mpu_config(void);
#endif

#endif
