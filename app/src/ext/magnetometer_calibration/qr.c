#include "qr.h"

#include "stdio.h"

QR_t qr_decomposition(Matrix M) {
    QR_t res;
    res.Q = mat_new(M->rows, M->columns);
    res.R = mat_new(M->rows, M->columns);

    Vector currentColumn;
    Vector currentUnitVector;
    double currentDotProduct;
    double norm;
    for (int i = 0; i < M->columns; i++) {
        currentColumn = mat_copy_column(M, i);
        for (int j = 0; j < i; j++) {
            currentUnitVector = mat_copy_column(res.Q, j);
            currentDotProduct = vec_dot_product(currentUnitVector, currentColumn);
            vec_multiply_scalar(currentUnitVector, currentDotProduct);
            vec_sub(currentColumn, currentUnitVector);
            vec_free(currentUnitVector);
            MAT_ELEM(res.R, j, i) = currentDotProduct;
        }
        norm = vec_norm(currentColumn);
        MAT_ELEM(res.R, i, i) = norm;
        vec_normalize(currentColumn);
        mat_paste_column(res.Q, currentColumn, i);
        vec_free(currentColumn);
    }
    return res;
}
void qr_free(QR_t qr) {
    mat_free(qr.Q);
    mat_free(qr.R);
}
