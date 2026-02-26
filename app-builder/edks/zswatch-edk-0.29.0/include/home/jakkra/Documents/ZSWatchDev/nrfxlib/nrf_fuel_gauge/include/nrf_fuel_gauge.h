/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file
 * @brief  nRF Fuel Gauge library API.
 */

#ifndef __NRF_FUEL_GAUGE_H__
#define __NRF_FUEL_GAUGE_H__

/**
 * @defgroup nrf_fuel_gauge nRF Fuel Gauge library
 * @{
 *
 * @brief Algorithm for battery state-of-charge estimation ("fuel gauge").
 *
 * @details The algorithm determines the predicted state-of-charge (SOC) based on the following
 * data: specific model of the battery type used in the application along with
 * measurements of battery voltage, current, and temperature.
 * The SOC is a value between 0% and 100%.
 *
 * @note As this library has two variants, make sure to use a battery model that matches the
 * configured variant: primary cell (non-rechargeable) or secondary cell (rechargeable).
 *
 * @note Current and voltage measurements do not have to be made at a certain interval.
 *       However, the two values should be measured as closely as possible in time.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

enum nrf_fuel_gauge_variant_type {
	/** Primary cell (non-rechargeable) battery variant. */
	NRF_FUEL_GAUGE_VARIANT_PRIMARY,
	/** Secondary cell (rechargeable) battery variant. */
	NRF_FUEL_GAUGE_VARIANT_SECONDARY,
};

/**
 * @brief Version number of nRF Fuel Gauge library in format "x.y.z".
 */
extern const char *nrf_fuel_gauge_version;

/**
 * @brief Build date of nRF Fuel Gauge library.
 */
extern const char *nrf_fuel_gauge_build_date;

/**
 * @brief Build variant of nRF Fuel Gauge library.
 */
extern const enum nrf_fuel_gauge_variant_type nrf_fuel_gauge_variant;

/**
 * @brief Size of the internal state memory used by nRF Fuel Gauge library.
 */
extern const size_t nrf_fuel_gauge_state_size;

/**
 * @brief Secondary cell (rechargeable) battery model parameters.
 *
 * @details Parameter details purposefully not documented.
 */
struct battery_model {
	float param_1[201];
	float temps[3];
	float param_2[122];
	float param_3[201];
	float param_4[201];
	float param_5[201];
	float param_6[201];
	float param_7[122];
	float param_8[122];
	float param_9[3];
	float param_10[6];
	float param_11[6];
	float param_12[3];
	char name[64];
};

/**
 * @brief Primary cell (non-rechargeable) battery model parameters.
 *
 * @details Parameter details purposefully not documented.
 */
struct battery_model_primary {
	float param_4[201];
	float param_3[201];
	float param_1[201];
	float param_5[201];
	float param_6[201];
	float param_2[88];
	float param_8[88];
	float param_7[88];
	float param_13[201];
	float param_14[201];
	float param_12;
	float param_15;
	float param_16;
	signed char param_17[20];
	float param_18[2];
	float param_19;
	char name[64];
};

/**
 * @brief Library state information. Useful for debugging.
 */
struct nrf_fuel_gauge_state_info {
	/** Predicted voltage. */
	float yhat;
	/** Battery resistance. */
	float r0;
	/** Truncated temperature. This state is used when the measured temperature falls outside
	 * the modeled range.
	 */
	float T_truncated;
	/** Corrected battery current. */
	float i_corrected;
	/** Raw state-of-charge value, not rounded off. */
	float soc_raw;
};

/**
 * @brief Optional static configuration parameters.
 *
 * @details Some parameter details purposefully not documented.
 */
struct nrf_fuel_gauge_config_parameters {
	float tau1;
	float tau2;
	float neta1;
	float neta2;
	float neta3;
	float beta1;
	float beta2;
	float gating;
	float volt_drop;
	uint8_t vk_buffer_length;
	/* Minimum duration of battery charge/discharge before TTE becomes available [s] */
	float tte_min_time;
	/* Reset threshold for TTE calculation. If short-term state-of-charge gradient differs
	 * from the long-term state-of-charge by this factor or more, the TTE state is reset.
	 */
	float tte_reset_threshold;
};

/**
 * @brief Optional runtime configuration parameters.
 *
 * @details Parameter details purposefully not documented.
 */
struct nrf_fuel_gauge_runtime_parameters {
	/* Set to NAN to ignore. */
	float a;
	/* Set to NAN to ignore. */
	float b;
	/* Set to NAN to ignore. */
	float c;
	/* Set to NAN to ignore. */
	float d;
	/* Primary cell: when true, do not let state-of-charge increase.
	 * Secondary cell: when true, do not let state-of-charge increase unless when charging.
	 */
	bool discard_positive_deltaz;
};

/**
 * @brief Library init parameter struct.
 */
struct nrf_fuel_gauge_init_parameters {
	/** Initial voltage in volts [V]. */
	float v0;
	/** Initial current in amperes [A]. */
	float i0;
	/** Initial temperature in centigrades [C]. */
	float t0;
	/** Pointer to static battery model. The model is not copied and must be kept valid. */
	union {
		/** Secondary cell battery model, when using secondary cell variant of library. */
		const struct battery_model *model;
		/** Primary cell battery model, when using primary cell variant of library. */
		const struct battery_model_primary *model_primary;
	};
	/** Optional configuration parameters. Set to NULL to use default values. */
	const struct nrf_fuel_gauge_config_parameters *opt_params;
	/** Optional state memory, previously retrieved using @ref nrf_fuel_gauge_state_get.
	 *  When used, the library resumes from this state and all other parameters are ignored.
	 *  Set to NULL when not used or when initializing the library for the first time.
	 */
	const void *state;
};

/**
 * @brief Charger state enum.
 *
 * @details The library uses this (optional) information to improve predictions.
 *
 * @note Time-to-full (TTF) prediction requires this information.
 */
enum nrf_fuel_gauge_charge_state {
	/** Charger not active. */
	NRF_FUEL_GAUGE_CHARGE_STATE_IDLE,
	/** Trickle charging. */
	NRF_FUEL_GAUGE_CHARGE_STATE_TRICKLE,
	/** Constant current charging at charger current limit. */
	NRF_FUEL_GAUGE_CHARGE_STATE_CC,
	/** Constant current charging, limited by vbus. */
	NRF_FUEL_GAUGE_CHARGE_STATE_CC_VBUS_LIMITED,
	/** Constant voltage charging. */
	NRF_FUEL_GAUGE_CHARGE_STATE_CV,
	/** Charging completed. */
	NRF_FUEL_GAUGE_CHARGE_STATE_COMPLETE,
};

/**
 * @brief State info type used for informing the library of external factors and events.
 *
 * @details The library uses this (optional) information to improve predictions.
 */
enum nrf_fuel_gauge_ext_state_info_type {
	/** VBUS/USB has been connected to supply external power */
	NRF_FUEL_GAUGE_EXT_STATE_INFO_VBUS_CONNECTED,
	/** VBUS/USB has been disconnected */
	NRF_FUEL_GAUGE_EXT_STATE_INFO_VBUS_DISCONNECTED,
	/** Change in charger state (as determined by charger device) */
	NRF_FUEL_GAUGE_EXT_STATE_INFO_CHARGE_STATE_CHANGE,
	/** Multiply current by this factor when charging */
	NRF_FUEL_GAUGE_EXT_STATE_INFO_CHARGE_CURRENT_CORRECTION,
	/** Multiply current by this factor when discharging */
	NRF_FUEL_GAUGE_EXT_STATE_INFO_DISCHARGE_CURRENT_CORRECTION,
	/** Specify the charge current limit used by the charger */
	NRF_FUEL_GAUGE_EXT_STATE_INFO_CHARGE_CURRENT_LIMIT,
	/** Specify the termination current used by the charger */
	NRF_FUEL_GAUGE_EXT_STATE_INFO_TERM_CURRENT,
};

/**
 * @brief State info data type used for additional information.
 */
union nrf_fuel_gauge_ext_state_info_data {
	/* Change in charge state */
	enum nrf_fuel_gauge_charge_state charge_state;
	/** Factor used for current measurement correction */
	float current_correction_factor;
	/** Battery charge current limit [A] */
	float charge_current_limit;
	/** Battery charge termination current [A] */
	float charge_term_current;
};

/**
 * @brief Initialize optional configuration parameters to default values.
 *
 * @param opt_params Pointer to the parameter structure.
 */
void nrf_fuel_gauge_opt_params_default_get(struct nrf_fuel_gauge_config_parameters *opt_params);

/**
 * @brief Initialize the nRF Fuel Gauge library.
 *
 * @details The library can be initialized based on a previously stored state.
 *          This is useful when there are periods without RAM retention.
 *          To resume from a previously stored state, the battery model must be the same
 *          and be in the same memory location as when the library was first initialized.
 *
 * @note The battery model referenced in @ref nrf_fuel_gauge_init_parameters must be kept valid as long as the library
 *       is in use.
 *
 * @param parameters Pointer to the parameter structure.
 * @param v0 Adjusted battery voltage (optional, for logging purposes).
 *
 * @retval 0 Initialization successful.
 * @retval -22 Initialization failed due to invalid parameters.
 */
int nrf_fuel_gauge_init(const struct nrf_fuel_gauge_init_parameters *parameters, float *v0);

/**
 * @brief Process battery measurement values.
 *
 * @details State-of-charge for primary cell battery models may be rounded off.
 *          The raw value can always be obtained from the optional state info struct.
 *
 * @param v Measured battery voltage [V].
 * @param i Measured battery current [A].
 * @param T Measured battery temperature [C].
 * @param t_delta Time delta since previous processed measurement [s].
 * @param state Pointer to state info struct (optional, set to null to ignore).
 *
 * @retval Predicted state-of-charge [%].
 */
float nrf_fuel_gauge_process(float v, float i, float T, float t_delta,
			     struct nrf_fuel_gauge_state_info *state);

/**
 * @brief Get predicted "time-to-empty" discharge duration.
 *
 * @details Time-to-empty is calculated based on the discharge rate. It may be NAN.
 *
 * @note Primary cell variant will always return NAN.
 *
 * @retval Predicted time-to-empty [s].
 */
float nrf_fuel_gauge_tte_get(void);

/**
 * @brief Get predicted "time-to-full" charging duration.
 *
 * @details Time-to-full is calculated based on the charge rate. It may be NAN.
 *
 * @note Primary cell variant will always return NAN.
 *
 * @retval Predicted time-to-full [s].
 */
float nrf_fuel_gauge_ttf_get(void);

/**
 * @brief Put library into the idle state.
 *
 * @details Informing the library of idle states helps improve predictions when there are periods
 *          of low activity and no new battery measurements.
 *          If this function is not called,
 *          discharge current in low power states may be over-estimated.
 *          For example, consider a system that enters a state where only the low frequency
 *          oscillator is running to generate a wakeup sometime later. To minimize the power usage
 *          during this period, no battery measurements are made. The idle current is likely
 *          to be too low to accurately measure dynamically, but it is known ahead of time.
 *          Using this function with the expected average current will help the library
 *          more accurately predict the power drain during the idle, low-power period.
 *          When activity is resumed, use @ref nrf_fuel_gauge_process as normal.
 *
 * @note Make sure this function is called from the same context as @ref nrf_fuel_gauge_process.
 *
 * @param v Measured battery voltage in volts [V].
 * @param T Measured battery temperature in centigrades [C].
 * @param i_avg Expected average current in idle state in amperes [A].
 */
void nrf_fuel_gauge_idle_set(float v, float T, float i_avg);

/**
 * @brief Retrieve library state data.
 *
 * @details The state data can be stored in persistent storage during a period of no RAM
 *          retention, and later used to resume the library from the same state.
 *
 * @param state Memory to copy state into.
 * @param size Size of buffer. Must be at least @ref nrf_fuel_gauge_state_size.
 *
 * @retval 0 when successful, otherwise negative error code.
 */
int nrf_fuel_gauge_state_get(void *state, size_t size);

/**
 * @brief Optional function to inform library of external events and factors.
 *
 * @details Informing the library of certain external events and factors can help improve
 *          battery state predictions.
 *
 * @note @ref nrf_fuel_gauge_ttf_get relies on external charger state information.
 *
 * @param type Type of external event.
 * @param data Additional data for certain info types. Set to NULL when not relevant.
 *
 * @retval 0 when successful, otherwise negative error code.
 */
int nrf_fuel_gauge_ext_state_update(enum nrf_fuel_gauge_ext_state_info_type type,
	union nrf_fuel_gauge_ext_state_info_data *data);

/**
 * @brief Update runtime parameters.
 *
 * @details Parameter details purposefully not documented.
 *
 * @param params New parameter values.
 */
void nrf_fuel_gauge_param_adjust(const struct nrf_fuel_gauge_runtime_parameters *params);

/**
 * @brief Update optional parameters.
 *
 * @details Parameter details purposefully not documented.
 *
 * @param opt_params New parameter values.
 */
void nrf_fuel_gauge_opt_params_adjust(const struct nrf_fuel_gauge_config_parameters *opt_params);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __NRF_FUEL_GAUGE_H__ */
