#ifndef COMPONENTS_GEOMETRY_SENSOR_CALIBRATION_H_
#define COMPONENTS_GEOMETRY_SENSOR_CALIBRATION_H_

#include "stdbool.h"
#include "matrix.h"
#include "vector.h"

typedef struct {
    Vector offset;
    Matrix transform;
} Callibration_t;

Callibration_t calib_calibrate_sensor(Vector x, Vector y, Vector z);
bool calib_calibration_success(Callibration_t calib);
void calib_calibrate_multiple_points(Callibration_t calib, Vector x, Vector y, Vector z);
void calib_calibrate_point(Callibration_t calib, Vector point);
double square_distance_variance(Vector x, Vector y, Vector z);
void calib_free(Callibration_t calibA);

#endif  // COMPONENTS_GEOMETRY_SENSOR_CALIBRATION_H_
