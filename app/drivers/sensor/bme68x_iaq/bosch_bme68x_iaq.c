#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/settings/settings.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

#include <string.h>

#include "bosch_bme68x_iaq.h"

LOG_MODULE_REGISTER(bosch_bsec, CONFIG_BME68X_IAQ_LOG_LEVEL);

#define DT_DRV_COMPAT 					bosch_bme680
#define SETTINGS_NAME_BSEC 				"bsec"
#define SETTINGS_KEY_STATE 				"state"
#define SETTINGS_BSEC_STATE 			SETTINGS_NAME_BSEC "/" SETTINGS_KEY_STATE
#define BSEC_TOTAL_HEAT_DUR				UINT16_C(140)
#define BSEC_INPUT_PRESENT(x, shift)	(x.process_data & (1 << (shift - 1)))

/* Temperature offset due to external heat sources. */
static const float temp_offset = (CONFIG_BME68X_IAQ_TEMPERATURE_OFFSET / 100.0f);

/* Define which sensor values to request.
 * The order is not important, but output_ready needs to be updated if different types
 * of sensor values are requested.
 */
static const bsec_sensor_configuration_t bsec_requested_virtual_sensors[4] = {
	{
		.sensor_id   = BSEC_OUTPUT_IAQ,
		.sample_rate = BSEC_SAMPLE_RATE,
	},
	{
		.sensor_id   = BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
		.sample_rate = BSEC_SAMPLE_RATE,
	},
	{
		.sensor_id   = BSEC_OUTPUT_RAW_PRESSURE,
		.sample_rate = BSEC_SAMPLE_RATE,
	},
	{
		.sensor_id   = BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
		.sample_rate = BSEC_SAMPLE_RATE,
	},
};

struct bme68x_iaq_config {
	const struct i2c_dt_spec i2c;
};

struct bme68x_iaq_data {
	/* Variable to store intermediate sample result */
	struct bme_sample_result {
		double temperature;
		double humidity;
		double pressure;
		uint16_t air_quality;
	} latest;

	/* Trigger and corresponding handler */
	sensor_trigger_handler_t trg_handler;
	const struct sensor_trigger *trigger;

	/* Internal BSEC thread metadata value. */
	struct k_thread thread;

	/* Buffer used to maintain the BSEC library state. */
	uint8_t state_buffer[BSEC_MAX_STATE_BLOB_SIZE];

	/* Size of the saved state */
	int32_t state_len;

	bsec_sensor_configuration_t required_sensor_settings[BSEC_MAX_PHYSICAL_SENSOR];
	uint8_t n_required_sensor_settings;

	/* some RAM space needed by bsec_get_state and bsec_set_state for (de-)serialization. */
	uint8_t work_buffer[BSEC_MAX_WORKBUFFER_SIZE];

	struct bme68x_dev dev;
};

static K_SEM_DEFINE(bsec_output_sem, 1, 1);
static K_THREAD_STACK_DEFINE(bsec_thread_stack, CONFIG_BME68X_IAQ_THREAD_STACK_SIZE);
static K_TIMER_DEFINE(bsec_save_state_timer, NULL, NULL);
static struct i2c_dt_spec bme688;

/** @brief			BSEC state loading function.
 *  @param p_key	Pointer NVM key
 *  @param len		Data length
 *  @param read_cb	Read callback
 *  @param p_cb_arg	Pointer to read callback arguments
 *  @param p_param	Pointer to parameter given to the settings_load_subtree_direct function.
 *  @return			0 when successful
*/
static int bsec_load_state(const char *p_key, size_t len,
				 settings_read_cb read_cb, void *p_cb_arg, void *p_param)
{
	ARG_UNUSED(p_key);
	struct bme68x_iaq_data *data = p_param;

	if (len > ARRAY_SIZE(data->state_buffer)) {
		return -EINVAL;
	}

	data->state_len = read_cb(p_cb_arg, data->state_buffer, len);

	if (data->state_len > 0) {
		return 0;
	}

	LOG_WRN("No settings data read");

	return -ENODATA;
}

/** @brief			BSEC state saving function.
 *  @param p_dev	Pointer to device structure
*/
static void bsec_save_state(const struct device *p_dev)
{
	int ret;
	struct bme68x_iaq_data *data = p_dev->data;

	LOG_DBG("saving state to flash");

	ret = bsec_get_state(0, data->state_buffer, ARRAY_SIZE(data->state_buffer),
				data->work_buffer, ARRAY_SIZE(data->work_buffer),
				&data->state_len);

	__ASSERT(ret == BSEC_OK, "bsec_get_state failed.");
	__ASSERT(data->state_len <= sizeof(data->state_buffer), "state buffer too big to save.");

	ret = settings_save_one(SETTINGS_BSEC_STATE, data->state_buffer, data->state_len);

	__ASSERT(ret == 0, "storing state to flash failed.");
}

/** @brief				BME68X I2C read function.
 *  @param reg_addr		Register address
 *  @param p_buf		Pointer to write data
 *  @param len			Output length
 *  @param p_intf_ptr	Interface pointer
 *  @return 			0 when successful
*/
static int8_t bsec_i2c_write(uint8_t reg_addr, const uint8_t *p_buf, uint32_t len, void *p_intf_ptr)
{
	ARG_UNUSED(p_intf_ptr);
	uint8_t buf[len + 1];

	buf[0] = reg_addr;
	memcpy(&buf[1], p_buf, len);

	return i2c_write_dt(&bme688, buf, ARRAY_SIZE(buf));
}

/** @brief				BME68X I2C read function.
 *  @param reg_addr		Register address
 *  @param p_buf		Pointer to read data
 *  @param len			Output length
 *  @param p_intf_ptr	Interface pointer
 *  @return 			0 when successful
*/
static int8_t bsec_i2c_read(uint8_t reg_addr, uint8_t *p_buf, uint32_t len, void *p_intf_ptr)
{
	ARG_UNUSED(p_intf_ptr);

	return i2c_write_read_dt(&bme688, &reg_addr, 1, p_buf, len);
}

/** @brief				BME68X us delay function.
 *  @param period		Delay period
 *  @param p_intf_ptr	Interface pointer
*/
static void bsec_delay_us(uint32_t period, void *p_intf_ptr)
{
	ARG_UNUSED(p_intf_ptr);

	k_usleep(period);
}

/** @brief				Function to handle output of BSEC.
 *  @param p_dev		Pointer to device structure
 *  @param p_outputs	Pointer to output data
 *  @param n_outputs	Length of output data
*/
static void output_ready(const struct device *p_dev, const bsec_output_t *p_outputs, uint8_t n_outputs)
{
	struct bme68x_iaq_data *data = p_dev->data;

	k_sem_take(&bsec_output_sem, K_FOREVER);
	for (size_t i = 0; i < n_outputs; ++i) {
		switch (p_outputs[i].sensor_id) {
		case BSEC_OUTPUT_IAQ:
			data->latest.air_quality = (uint16_t)p_outputs[i].signal;
			LOG_DBG("IAQ: %d", data->latest.air_quality);
			break;
		case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE:
			data->latest.temperature = (double)p_outputs[i].signal;
			LOG_DBG("Temp: %.2f C", data->latest.temperature);
			break;
		case BSEC_OUTPUT_RAW_PRESSURE:
			data->latest.pressure = (double)p_outputs[i].signal;
			LOG_DBG("Press: %.2f Pa", data->latest.pressure);
			break;
		case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY:
			data->latest.humidity = (double)p_outputs[i].signal;
			LOG_DBG("Hum: %.2f %%", data->latest.humidity);
			break;
		default:
			LOG_WRN("unknown bsec output id: %d", p_outputs[i].sensor_id);
			break;
		}
	}

	k_sem_give(&bsec_output_sem);

	if (data->trg_handler != NULL) {
		data->trg_handler(p_dev, data->trigger);
	}
}

/** @brief				Convert raw bme68x output to valid input for BSEC.
 *  @param settings		Sensor settings
 *  @param p_data		Pointer to sensor data
 *  @param p_inputs		Pointer to BSEC input data
 *  @param timestamp_ns	
 *  @return				Number of inputs
*/
static size_t sensor_data_to_bsec_inputs(bsec_bme_settings_t settings,
					 const struct bme68x_data *p_data,
					 bsec_input_t *p_inputs, uint64_t timestamp_ns)
{
	size_t i = 0;

	if (BSEC_INPUT_PRESENT(settings, BSEC_INPUT_TEMPERATURE)) {
		/* append heatsource input */
		p_inputs[i].sensor_id = BSEC_INPUT_HEATSOURCE;
		p_inputs[i].signal = temp_offset;
		p_inputs[i].time_stamp = timestamp_ns;
		LOG_DBG("Temp offset: %.2f", p_inputs[i].signal);
		i++;

		/* append temperature input */
		p_inputs[i].sensor_id = BSEC_INPUT_TEMPERATURE;
		p_inputs[i].signal = p_data->temperature;

		if (IS_ENABLED(BME68X_DO_NOT_USE_FPU)) {
			/* in this config, temperature is output in centidegrees */
			p_inputs[i].signal /= 100.0f;
		}

		p_inputs[i].time_stamp = timestamp_ns;
		LOG_DBG("Temp: %.2f", p_inputs[i].signal);
		i++;
	}

	if (BSEC_INPUT_PRESENT(settings, BSEC_INPUT_HUMIDITY)) {
		p_inputs[i].sensor_id = BSEC_INPUT_HUMIDITY;
		p_inputs[i].signal =  p_data->humidity;

		if (IS_ENABLED(BME68X_DO_NOT_USE_FPU)) {
			/* in this config, humidity is output in millipercent */
			p_inputs[i].signal /= 1000.0f;
		}

		p_inputs[i].time_stamp = timestamp_ns;
		LOG_DBG("Hum: %.2f", p_inputs[i].signal);
		i++;
	}

	if (BSEC_INPUT_PRESENT(settings, BSEC_INPUT_PRESSURE)) {
		p_inputs[i].sensor_id = BSEC_INPUT_PRESSURE;
		p_inputs[i].signal =  p_data->pressure;
		p_inputs[i].time_stamp = timestamp_ns;
		LOG_DBG("Press: %.2f", p_inputs[i].signal);
		i++;
	}

	if (BSEC_INPUT_PRESENT(settings, BSEC_INPUT_GASRESISTOR)) {
		p_inputs[i].sensor_id = BSEC_INPUT_GASRESISTOR;
		p_inputs[i].signal =  p_data->gas_resistance;
		p_inputs[i].time_stamp = timestamp_ns;
		LOG_DBG("Gas: %.2f", p_inputs[i].signal);
		i++;
	}

	if (BSEC_INPUT_PRESENT(settings, BSEC_INPUT_PROFILE_PART)) {
		p_inputs[i].sensor_id = BSEC_INPUT_PROFILE_PART;
		if (settings.op_mode == BME68X_FORCED_MODE) {
			p_inputs[i].signal =  0;
		} else {
			p_inputs[i].signal =  p_data->gas_index;
		}
		p_inputs[i].time_stamp = timestamp_ns;
		LOG_DBG("Profile: %.2f", p_inputs[i].signal);
		i++;
	}

	return i;
}

/** @brief			Convert and apply bme68x settings chosen by BSEC and transmit them to the sensor.
 *  @param p_dev	Pointer to device structure
 *  @param settings	Sensor settings
 *  @return			
*/
static int apply_sensor_settings(const struct device *p_dev, bsec_bme_settings_t settings)
{
	int ret;
	struct bme68x_conf config = {0};
	struct bme68x_heatr_conf heater_config = {0};
	struct bme68x_iaq_data *data = p_dev->data;

	heater_config.enable = BME68X_ENABLE;
	heater_config.heatr_temp = settings.heater_temperature;
	heater_config.heatr_dur = settings.heater_duration;
	heater_config.heatr_temp_prof = settings.heater_temperature_profile;
	heater_config.heatr_dur_prof = settings.heater_duration_profile;
	heater_config.profile_len = settings.heater_profile_len;
	heater_config.shared_heatr_dur = 0;

	switch (settings.op_mode) {
	case BME68X_PARALLEL_MODE:
		/* this block is only executed for BME68X_PARALLEL_MODE */
		/* shared heating duration in milliseconds (converted from microseconds) */
		heater_config.shared_heatr_dur =
			BSEC_TOTAL_HEAT_DUR -
			(bme68x_get_meas_dur(settings.op_mode, &config, &data->dev)
			/ INT64_C(1000));

		__fallthrough;
	case BME68X_FORCED_MODE:
		/* this block is executed for any measurement mode */
		ret = bme68x_get_conf(&config, &data->dev);
		if (ret) {
			LOG_ERR("bme68x_get_conf err: %d", ret);
			return ret;
		}

		config.os_hum = settings.humidity_oversampling;
		config.os_temp = settings.temperature_oversampling;
		config.os_pres = settings.pressure_oversampling;

		ret = bme68x_set_conf(&config, &data->dev);
		if (ret) {
			LOG_ERR("bme68x_set_conf err: %d", ret);
			return ret;
		}

		bme68x_set_heatr_conf(settings.op_mode, &heater_config, &data->dev);

		__fallthrough;
	case BME68X_SLEEP_MODE:
		/* this block is executed for all modes */
		ret = bme68x_set_op_mode(settings.op_mode, &data->dev);
		if (ret) {
			LOG_ERR("bme68x_set_op_mode err: %d", ret);
			return ret;
		}
		break;
	default:
		LOG_ERR("unknown op mode: %d", settings.op_mode);
	}

	return 0;
}

/** @brief				
 *  @param p_dev		Pointer to device structure
 *  @param p_settings	Pointer to sensor settings
 *  @param timestamp_ns	
*/
static void fetch_and_process_output(const struct device *p_dev,
				     bsec_bme_settings_t *p_settings,
				     uint64_t timestamp_ns)
{
	uint8_t n_fields = 0;
	uint8_t n_outputs = 0;
	uint8_t n_inputs = 0;
	bsec_input_t inputs[BSEC_MAX_PHYSICAL_SENSOR] = {0};
	bsec_output_t outputs[ARRAY_SIZE(bsec_requested_virtual_sensors)] = {0};
	struct bme68x_data sensor_data[3] = {0};
	struct bme68x_iaq_data *data = p_dev->data;
	int ret = bme68x_get_data(p_settings->op_mode, sensor_data, &n_fields, &data->dev);

	if (ret) {
		LOG_DBG("bme68x_get_data err: %d", ret);
		return;
	}

	for (size_t i = 0; i < n_fields; ++i) {
		n_outputs = ARRAY_SIZE(bsec_requested_virtual_sensors);
		n_inputs = sensor_data_to_bsec_inputs(*p_settings,
							sensor_data + i,
							inputs, timestamp_ns);

		if (n_inputs == 0) {
			continue;
		}
		ret = bsec_do_steps(inputs, n_inputs, outputs, &n_outputs);
		if (ret != BSEC_OK) {
			LOG_ERR("bsec_do_steps err: %d", ret);
			continue;
		}
		output_ready(p_dev, outputs, n_outputs);
	}
}

/** @brief			BSEC run worker. The worker manages all recurrings tasks for the sensor:
 * 						- Update device settings according to BSEC
 * 						- Fetch measurement values
 * 						- Update BSEC state
 * 						- Periodically save BSEC state to flash
 *  @param p_dev	Pointer to device structure
*/
static void bsec_run_worker(const struct device *p_dev)
{
	int ret;
	bsec_bme_settings_t sensor_settings = {0};

	while (true) {
		uint64_t timestamp_ns = k_ticks_to_ns_floor64(k_uptime_ticks());

		if (timestamp_ns < sensor_settings.next_call) {
			LOG_DBG("bsec_sensor_control not ready yet");
			k_sleep(K_NSEC(sensor_settings.next_call - timestamp_ns));
			continue;
		}

		memset(&sensor_settings, 0, sizeof(sensor_settings));
		ret = bsec_sensor_control((int64_t)timestamp_ns, &sensor_settings);
		if (ret != BSEC_OK) {
			LOG_ERR("bsec_sensor_control err: %d", ret);
			continue;
		}

		if (apply_sensor_settings(p_dev, sensor_settings)) {
			continue;
		}

		if (sensor_settings.trigger_measurement &&
		    sensor_settings.op_mode != BME68X_SLEEP_MODE) {
			fetch_and_process_output(p_dev, &sensor_settings, timestamp_ns);
		}

		if (k_timer_remaining_get(&bsec_save_state_timer) == 0) {
			bsec_save_state(p_dev);
			k_timer_start(&bsec_save_state_timer,
				      K_MINUTES(CONFIG_BME68X_IAQ_SAVE_INTERVAL_MINUTES),
				      K_NO_WAIT);
		}

		k_sleep(K_SECONDS(BSEC_SAMPLE_PERIOD_S));
	}
}

/** @brief			Initialize the BSEC driver.
 *  @param p_dev	Pointer to device structure
 *  @return			0 when successful
*/
static int bme68x_bsec_init(const struct device *p_dev)
{
	int err;
	struct bme68x_iaq_data *data = p_dev->data;
	const struct bme68x_iaq_config *config = p_dev->config;

	bme688 = config->i2c;

	err = settings_subsys_init();
	if (err) {
		LOG_ERR("settings_subsys_init, error: %d", err);
		return err;
	}

	err = settings_load_subtree_direct(SETTINGS_BSEC_STATE, bsec_load_state, data);
	if (err) {
		LOG_ERR("settings_load_subtree, error: %d", err);
		return err;
	}

	if (!device_is_ready(bme688.bus)) {
		LOG_ERR("I2C device not ready");
		return -ENODEV;
	}

	data->dev.intf = BME68X_I2C_INTF;
	data->dev.intf_ptr = NULL;
	data->dev.read = bsec_i2c_read;
	data->dev.write = bsec_i2c_write;
	data->dev.delay_us = bsec_delay_us;

	// TODO: Replace temperature with BME688 temperature
	data->dev.amb_temp = CONFIG_BME68X_IAQ_EXPECTED_AMBIENT_TEMP;

	err = bme68x_init(&data->dev);
	if (err) {
		LOG_ERR("Failed to init bme68x: %d", err);
		return err;
	}

	// TODO
	//struct bme68x_data measurement;
	//uint8_t n;
	//bme68x_set_op_mode(BME68X_FORCED_MODE, &data->dev);
	//k_msleep(1000);
	//bme68x_get_data(BME68X_FORCED_MODE, &measurement, &n, &data->dev);
	//LOG_DBG("Temperature: %.2f", measurement.temperature);
	//

	err = bsec_init();
	if (err != BSEC_OK) {
		LOG_ERR("Failed to init BSEC: %d", err);
		return err;
	}

	err = bsec_set_state(data->state_buffer, data->state_len,
			     data->work_buffer, ARRAY_SIZE(data->work_buffer));
	if (err != BSEC_OK && err != BSEC_E_CONFIG_EMPTY) {
		LOG_ERR("Failed to set BSEC state: %d", err);
	} else if (err == BSEC_OK) {
		LOG_DBG("Setting BSEC state successful.");
	}

	bsec_update_subscription(bsec_requested_virtual_sensors,
				 ARRAY_SIZE(bsec_requested_virtual_sensors),
				 data->required_sensor_settings, &data->n_required_sensor_settings);

	k_thread_create(&data->thread,
			bsec_thread_stack,
			CONFIG_BME68X_IAQ_THREAD_STACK_SIZE,
			(k_thread_entry_t)bsec_run_worker,
			(void*)p_dev, NULL, NULL, K_LOWEST_APPLICATION_THREAD_PRIO, 0, K_NO_WAIT);

	k_timer_start(&bsec_save_state_timer,
		      K_MINUTES(CONFIG_BME68X_IAQ_SAVE_INTERVAL_MINUTES),
		      K_NO_WAIT);

	LOG_DBG("BSEC driver started");

	return 0;
}

/** @brief			Sensor API trigger function.
 *  @param p_dev	Pointer to device structure
 *  @param p_trig	Pointer to trigger configuration
 *  @param handler	Function handler for the sensor trigger
 *  @return			0 when successful
*/
static int bme68x_trigger_set(const struct device *p_dev,
		       const struct sensor_trigger *p_trig,
		       sensor_trigger_handler_t handler)
{
	struct bme68x_iaq_data *data = p_dev->data;

	if (p_trig->type != SENSOR_TRIG_TIMER) {
		LOG_ERR("Unsupported sensor trigger");
		return -ENOTSUP;
	}

	if ((p_trig->chan == SENSOR_CHAN_ALL)
	    || (p_trig->chan == SENSOR_CHAN_HUMIDITY)
	    || (p_trig->chan == SENSOR_CHAN_AMBIENT_TEMP)
	    || (p_trig->chan == SENSOR_CHAN_PRESS)
	    || (p_trig->chan == SENSOR_CHAN_VOC)) {
		data->trigger = p_trig;
		data->trg_handler = handler;
	} else {
		LOG_ERR("Unsupported sensor channel");
		return -ENOTSUP;
	}

	return 0;
}

/** @brief			Sensor API sample fetch function.
 *					NOTE: We don´t use this function, because the background thread is refreshing the
 *					values permanently. The function must stay in the code, because the device subsystem
 *					API is using it.
 *  @param p_dev	Pointer to device structure
 *  @param chan		Sensor channel
 *  @return			0 when successful
*/
static int bme68x_sample_fetch(const struct device *p_dev, enum sensor_channel chan)
{
	return 0;
}

/** @brief			Sensor API sample get function.
 *  @param p_dev	Pointer to device structure
 *  @param chan		Sensor channel
 *  @param p_val	Pointer to sensor data
 *  @return			0 when successful
*/
static int bme68x_channel_get(const struct device *p_dev,
			      enum sensor_channel chan,
			      struct sensor_value *p_val)
{
	struct bme68x_iaq_data *data = p_dev->data;
	int result = 0;

	k_sem_take(&bsec_output_sem, K_FOREVER);
	if (chan == SENSOR_CHAN_HUMIDITY) {
		sensor_value_from_double(p_val, data->latest.humidity);
	} else if (chan == SENSOR_CHAN_AMBIENT_TEMP) {
		sensor_value_from_double(p_val, data->latest.temperature);
	} else if (chan == SENSOR_CHAN_PRESS) {
		sensor_value_from_double(p_val, data->latest.pressure);
	} else if (chan == SENSOR_CHAN_VOC) {
		p_val->val1 = data->latest.air_quality;
		p_val->val2 = 0;
	} else {
		LOG_ERR("Unsupported sensor channel");
		result = -ENOTSUP;
	}
	k_sem_give(&bsec_output_sem);
	return result;
}

static const struct sensor_driver_api bme68x_driver_api = {
	.sample_fetch = &bme68x_sample_fetch,
	.channel_get = &bme68x_channel_get,
	.trigger_set = bme68x_trigger_set,
};

static struct bme68x_iaq_config config_0 =  {
	.i2c = I2C_DT_SPEC_INST_GET(0),
};
static struct bme68x_iaq_data data_0;

SENSOR_DEVICE_DT_INST_DEFINE(0, bme68x_bsec_init, NULL,
			      &data_0,
			      &config_0,
			      POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,
			      &bme68x_driver_api);