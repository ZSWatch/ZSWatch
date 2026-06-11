#ifndef COMPONENTS_GEOMETRY_ELLIPSOID_FIT_H_
#define COMPONENTS_GEOMETRY_ELLIPSOID_FIT_H_

#include "vector.h"

typedef struct {
    Vector coefA;
    Vector coefB;
} Ellipsoid_t;

Ellipsoid_t ellipsoid_fit(Vector x, Vector y, Vector z);
void ellipsoid_free(Ellipsoid_t elip);

#endif  // COMPONENTS_GEOMETRY_ELLIPSOID_FIT_H_
