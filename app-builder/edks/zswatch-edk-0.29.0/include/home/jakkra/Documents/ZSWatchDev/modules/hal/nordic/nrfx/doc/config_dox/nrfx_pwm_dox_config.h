/**
 *
 * @defgroup nrfx_pwm_config PWM peripheral driver configuration
 * @{
 * @ingroup nrfx_pwm
 */
/** @brief
 *
 *  Set to 1 to activate.
 *
 * @note This is an NRF_CONFIG macro.
 */
#define NRFX_PWM_ENABLED

/** @brief Interrupt priority
 *
 *  Following options are available:
 * - 0 - 0 (highest)
 * - 1 - 1
 * - 2 - 2
 * - 3 - 3
 * - 4 - 4
 * - 5 - 5
 * - 6 - 6
 * - 7 - 7
 *
 * @note This is an NRF_CONFIG macro.
 */
#define NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY

/** @brief Enables logging in the module.
 *
 *  Set to 1 to activate.
 *
 * @note This is an NRF_CONFIG macro.
 */
#define NRFX_PWM_CONFIG_LOG_ENABLED

/** @brief Default Severity level
 *
 *  Following options are available:
 * - 0 - Off
 * - 1 - Error
 * - 2 - Warning
 * - 3 - Info
 * - 4 - Debug
 *
 * @note This is an NRF_CONFIG macro.
 */
#define NRFX_PWM_CONFIG_LOG_LEVEL

/** @brief ANSI escape code prefix.
 *
 *  Following options are available:
 * - 0 - Default
 * - 1 - Black
 * - 2 - Red
 * - 3 - Green
 * - 4 - Yellow
 * - 5 - Blue
 * - 6 - Magenta
 * - 7 - Cyan
 * - 8 - White
 *
 * @note This is an NRF_CONFIG macro.
 */
#define NRFX_PWM_CONFIG_INFO_COLOR

/** @brief ANSI escape code prefix.
 *
 *  Following options are available:
 * - 0 - Default
 * - 1 - Black
 * - 2 - Red
 * - 3 - Green
 * - 4 - Yellow
 * - 5 - Blue
 * - 6 - Magenta
 * - 7 - Cyan
 * - 8 - White
 *
 * @note This is an NRF_CONFIG macro.
 */
#define NRFX_PWM_CONFIG_DEBUG_COLOR

/** @brief EGU instance used by the nRF52 Anomaly 109 workaround for PWM.
 *
 *  Following options are available:
 * - 0 - EGU0
 * - 1 - EGU1
 * - 2 - EGU2
 * - 3 - EGU3
 * - 4 - EGU4
 * - 5 - EGU5
 *
 * @note This is an NRF_CONFIG macro.
 */
#define NRFX_PWM_NRF52_ANOMALY_109_EGU_INSTANCE



/** @} */
