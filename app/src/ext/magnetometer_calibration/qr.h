#ifndef COMPONENTS_GEOMETRY_QR_H_
#define COMPONENTS_GEOMETRY_QR_H_

#include "matrix.h"
#include "vector.h"

typedef struct {
    Matrix Q;
    Matrix R;
} QR_t;

QR_t qr_decomposition(Matrix M);
void qr_free(QR_t qr);

#endif  // COMPONENTS_GEOMETRY_QR_H_
