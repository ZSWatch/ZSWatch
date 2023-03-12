#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include <zephyr/drivers/sensor.h>

typedef enum accelerometer_evt_type_t {
    ACCELEROMETER_EVT_TYPE_XYZ,
    ACCELEROMETER_EVT_TYPE_DOOUBLE_TAP,
    ACCELEROMETER_EVT_TYPE_STEP,
    ACCELEROMETER_EVT_TYPE_TILT,
} accelerometer_evt_type_t;

typedef struct accelerometer_data_xyz_t {
    int16_t x;
    int16_t y;
    int16_t z;
} accelerometer_data_xyz_t;

typedef struct accelerometer_data_step_t {
    int16_t count;
} accelerometer_data_step_t;

typedef struct accelerometer_evt_t {
    accelerometer_evt_type_t type;
    union {
        accelerometer_data_xyz_t    xyz;
        accelerometer_data_step_t   step;
    } data;
} accelerometer_evt_t;

int accelerometer_init(void);

int accelerometer_fetch_xyz(int16_t *x, int16_t *y, int16_t *z);

int accelerometer_fetch_temperature(struct sensor_value *temperature);

int accelerometer_fetch_num_steps(int16_t *num_steps);

int accelerometer_reset_step_count(void);


#endif