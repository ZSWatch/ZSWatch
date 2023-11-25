#pragma once

typedef struct sensor_fusion {
    float roll;
    float pitch;
    float yaw;
    float x;
    float y;
    float z;
} sensor_fusion_t;

void sensor_fusion_fetch_all(sensor_fusion_t *readings);

void sensor_fusion_init(void);

void sensor_fusion_deinit(void);