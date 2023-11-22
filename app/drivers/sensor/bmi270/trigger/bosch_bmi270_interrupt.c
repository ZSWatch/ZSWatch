/* bosch_bmi270_interrupt.c - Driver for Bosch BMI270 IMU. */

/*
 * Copyright (c) 2023, Daniel Kampert
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>

#include "../bosch_bmi270.h"
#include "../private/bosch_bmi270_config.h"

LOG_MODULE_REGISTER(bosch_bmi270_trigger, CONFIG_BOSCH_BMI270_PLUS_LOG_LEVEL);

#ifdef CONFIG_BMI270_PLUS_TRIGGER_OWN_THREAD
static K_KERNEL_STACK_DEFINE(bmi2_thread_stack, CONFIG_BMI270_PLUS_THREAD_STACK_SIZE);
static struct k_thread bmi2_thread;
#endif

/** @brief          
 *  @param p_dev    
 *  @param enable   
*/
static inline void bmi2_enable_int(const struct device *p_dev, bool enable)
{
    const struct bmi270_config *config = p_dev->config;

    unsigned int flags = enable ? GPIO_INT_EDGE_RISING : GPIO_INT_DISABLE;

    gpio_pin_interrupt_configure_dt(&config->int_gpio, flags);
}

/** @brief          
 *  @param p_dev	
*/
static void bmi2_handle_int(const struct device *p_dev)
{
    struct bmi270_data *data = p_dev->data;

    bmi2_enable_int(p_dev, false);

#if defined(CONFIG_BMI270_PLUS_TRIGGER_OWN_THREAD)
    k_sem_give(&data->sem);
#elif defined(CONFIG_BMI270_PLUS_TRIGGER_GLOBAL_THREAD)
    k_work_submit(&data->work);
#endif
}

/** @brief          
 *  @param p_dev    
 *  @param p_cb     
 *  @param pins     
*/
static void bmi2_gpio_on_interrupt_callback(const struct device *p_dev, struct gpio_callback *p_cb, uint32_t pins)
{
    ARG_UNUSED(pins);
    ARG_UNUSED(p_dev);

    struct bmi270_data *data = CONTAINER_OF(p_cb, struct bmi270_data, gpio_handler);

    bmi2_handle_int(data->dev);
}

/** @brief          
 *  @param p_dev	
*/
static void bmi2_process_int(const struct device *p_dev)
{
    uint16_t status;

    struct bmi270_data *data = p_dev->data;
    struct bmi2_feat_sensor_data sensor_data;
    struct sensor_trigger* trigger = (struct sensor_trigger*)data->trig;

    memset(&sensor_data, 0, sizeof(sensor_data));

    if (bmi2_get_int_status(&status, &data->bmi2) != BMI2_OK) {
        LOG_ERR("Can not fetch status from IMU!");
        return;
    }

    LOG_DBG("Status: %u", status);

    if (status & BMI270_SIG_MOT_STATUS_MASK) {
        LOG_DBG("BMI270_SIG_MOT_STATUS_MASK");

        trigger->type = SENSOR_TRIG_SIG_MOTION;

        if (data->sig_motion) {
            data->sig_motion(p_dev, trigger);
        }
    }

    // TODO: For some reason step count ISR is nor firing, for now
    // just poll it whenever there is another event.
    if (true || status & BMI270_STEP_CNT_STATUS_MASK) {
        if (status & BMI270_STEP_CNT_STATUS_MASK) {
            LOG_DBG("BMI270_STEP_CNT_STATUS_MASK");
            
        }

        trigger->type = SENSOR_TRIG_STEP;

        if (data->step) {
            data->step(p_dev, trigger);
        }
    }

    if (status & BMI270_STEP_ACT_STATUS_MASK) {
        LOG_DBG("BMI270_STEP_ACT_STATUS_MASK");

        sensor_data.type = BMI2_STEP_ACTIVITY;
        bmi270_get_feature_data(&sensor_data, 1, &data->bmi2);

        trigger->type = SENSOR_TRIG_ACTIVITY;

        if (data->activity) {
            data->activity(p_dev, trigger);
        }
    }

    if (status & BMI270_WRIST_WAKE_UP_STATUS_MASK) {
        LOG_DBG("BMI270_WRIST_WAKE_UP_STATUS_MASK");

        trigger->type = SENSOR_TRIG_WRIST_WAKE;

        if (data->wake) {
            data->wake(p_dev, trigger);
        }
    }

    if (status & BMI270_WRIST_GEST_STATUS_MASK) {
        LOG_DBG("BMI270_WRIST_GEST_STATUS_MASK");

        sensor_data.type = BMI2_WRIST_GESTURE;
        bmi270_get_feature_data(&sensor_data, 1, &data->bmi2);

        trigger->type = SENSOR_TRIG_WRIST_GESTURE;

        if (data->gesture) {
            data->gesture(p_dev, trigger);
        }
    }

    if (status & BMI270_ANY_MOT_STATUS_MASK) {
        trigger->type = SENSOR_TRIG_MOTION;

        if (data->motion) {
            data->motion(p_dev, trigger);
        }
    }

    // After a NO_MOTION INT and the INT status is cleared, then
    // when any other INT is triggered, also NO_MOTION status bit is set once more.
    // To workaround that, only care about NO_MOTION
    // when the INT was only for NO_MOTION.
    if (status == BMI270_NO_MOT_STATUS_MASK) {
        trigger->type = SENSOR_TRIG_STATIONARY;

        if (data->stationary) {
            data->stationary(p_dev, trigger);
        }
    }

    // The global handler is called every time.
    if (data->global) {
        data->global(p_dev, trigger);
    }

    bmi2_enable_int(p_dev, true);
}

#ifdef CONFIG_BMI270_PLUS_TRIGGER_OWN_THREAD
/** @brief          
 *  @param p_arg1   
 *  @param p_arg2   
 *  @param p_arg3   
*/
static void bmi2_worker_thread(void *p_arg1, void *p_arg2, void *p_arg3)
{
    ARG_UNUSED(p_arg2);
    ARG_UNUSED(p_arg3);

    struct bmi270_data *data = p_arg1;

    while (true) {
        k_sem_take(&data->sem, K_FOREVER);
        LOG_DBG("Process interrupt from thread");
        bmi2_process_int(data->dev);
    }
}
#else
/** @brief          
 *  @param p_work   
*/
static void bmi2_worker(struct k_work *p_work)
{
    struct bmi270_data *data = CONTAINER_OF(p_work, struct bmi270_data, work);

    LOG_DBG("Process interrupt from worker");

    bmi2_process_int(data->dev);
}
#endif

int bmi2_init_interrupt(const struct device *p_dev)
{
    struct bmi2_int_pin_config int_cfg;
    struct bmi270_data *data = p_dev->data;
    const struct bmi270_config *config = p_dev->config;

    LOG_DBG("Initialize interrupts...");

    if (!gpio_is_ready_dt(&config->int_gpio)) {
        LOG_ERR("INT GPIO device not ready!");
        return -ENODEV;
    }

    if (gpio_pin_configure_dt(&config->int_gpio, GPIO_INPUT)) {
        return -EFAULT;
    }

    data->dev = p_dev;

#ifdef CONFIG_BMI270_PLUS_TRIGGER_OWN_THREAD
    k_sem_init(&data->sem, 0, K_SEM_MAX_LIMIT);

    k_thread_create(&bmi2_thread, bmi2_thread_stack, CONFIG_BMI270_PLUS_THREAD_STACK_SIZE,
                    bmi2_worker_thread, data, NULL, NULL, K_PRIO_COOP(CONFIG_BMI270_PLUS_THREAD_PRIORITY),
                    0, K_NO_WAIT);
#else
    data->work.handler = bmi2_worker;
#endif

    gpio_init_callback(&data->gpio_handler, bmi2_gpio_on_interrupt_callback, BIT(config->int_gpio.pin));

    if (gpio_add_callback(config->int_gpio.port, &data->gpio_handler)) {
        return -EFAULT;
    }

    if (bmi2_get_int_pin_config(&int_cfg, &data->bmi2) != BMI2_OK) {
        return -EFAULT;
    }

    //Uncomment when both int pins should get used
//  int_cfg.pin_type = BMI2_INT_BOTH;
/*
    int_cfg.pin_type = BMI2_INT1;
    int_cfg.pin_cfg[0].lvl = BMI2_INT_ACTIVE_HIGH;
    int_cfg.pin_cfg[0].od = BMI2_INT_PUSH_PULL;
    int_cfg.pin_cfg[0].output_en = BMI2_INT_OUTPUT_ENABLE;
*/

    int_cfg.pin_cfg[0].output_en = BMI2_INT_NONE;

    int_cfg.pin_type = BMI2_INT2;
    int_cfg.pin_cfg[1].lvl = BMI2_INT_ACTIVE_HIGH;
    int_cfg.pin_cfg[1].od = BMI2_INT_PUSH_PULL;
    int_cfg.pin_cfg[1].output_en = BMI2_INT_OUTPUT_ENABLE;

    if (bmi2_set_int_pin_config(&int_cfg, &data->bmi2) != BMI2_OK) {
        return -EFAULT;
    }

    LOG_DBG("Interrupts ready!");

    return 0;
}

int bmi270_trigger_set(const struct device *p_dev, const struct sensor_trigger *p_trig, sensor_trigger_handler_t handler)
{
    struct bmi270_data *data = p_dev->data;
    const struct bmi270_config *config = p_dev->config;

    if ((!config->int_gpio.port) || ((p_trig->chan != SENSOR_CHAN_GESTURE) && (p_trig->chan != SENSOR_CHAN_ALL))) {
        return -ENOTSUP;
    }

    data->trig = p_trig;

    // The global handler is used when all channels are selected.
    if (p_trig->chan == SENSOR_CHAN_ALL) {
        data->global = handler;
    }
    else {
        switch ((uint8_t)p_trig->type) {
            case SENSOR_TRIG_SIG_MOTION:
                data->sig_motion = handler;
                break;
            case SENSOR_TRIG_STEP:
                data->step = handler;
                break;
            case SENSOR_TRIG_ACTIVITY:
                data->activity = handler;
                break;
            case SENSOR_TRIG_WRIST_WAKE:
                data->wake = handler;
                break;
            case SENSOR_TRIG_WRIST_GESTURE:
                data->gesture = handler;
                break;
            case SENSOR_TRIG_STATIONARY:
                data->stationary = handler;
                break;
            case SENSOR_TRIG_MOTION:
                data->motion = handler;
                break;
            default:
                return -ENOTSUP;
        }
    }

    bmi2_enable_int(p_dev, false);

    if (handler != NULL) {
        bmi2_enable_int(p_dev, true);
    }

    LOG_DBG("Trigger for channel %u installed", p_trig->chan);

    return 0;  
}