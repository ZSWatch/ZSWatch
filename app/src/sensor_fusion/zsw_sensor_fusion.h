#pragma once

typedef struct sensor_fusion {
    float roll;
    float pitch;
    float yaw;
    float x;
    float y;
    float z;
} sensor_fusion_t;

int zsw_sensor_fusion_init(void);

void zsw_sensor_fusion_deinit(void);

int zsw_sensor_fusion_fetch_all(sensor_fusion_t *p_readings);

int zsw_sensor_fusion_get_heading(float *heading);

