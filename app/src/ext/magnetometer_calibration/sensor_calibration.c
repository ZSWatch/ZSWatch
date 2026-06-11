#include "sensor_calibration.h"

#include "stdio.h"
#include "math.h"
#include "assert.h"
#include "ellipsoid_fit.h"
#include "eigen.h"

Matrix calib_ellipsoid_matrix(Vector coefA) {
    Matrix res = mat_new(3, 3);
    MAT_ELEM(res, 0, 0) = VEC_ELEM(coefA, 0);
    MAT_ELEM(res, 0, 1) = VEC_ELEM(coefA, 5);
    MAT_ELEM(res, 0, 2) = VEC_ELEM(coefA, 4);

    MAT_ELEM(res, 1, 0) = VEC_ELEM(coefA, 5);
    MAT_ELEM(res, 1, 1) = VEC_ELEM(coefA, 1);
    MAT_ELEM(res, 1, 2) = VEC_ELEM(coefA, 3);

    MAT_ELEM(res, 2, 0) = VEC_ELEM(coefA, 4);
    MAT_ELEM(res, 2, 1) = VEC_ELEM(coefA, 3);
    MAT_ELEM(res, 2, 2) = VEC_ELEM(coefA, 2);
    return res;
}

Matrix calib_calibrate_rotation(Matrix ellipMat, Vector prq, double d) {
    Eigen_t eig = eig_solve(ellipMat);
    mat_orthogonalize(eig.eigenvectors);  // compensating inaccuracy
    Matrix rotation = mat_transpose(eig.eigenvectors);
    Matrix rotatedEllipMat = mat_multiply(rotation, ellipMat);
    Matrix diagonizedEllipMat = mat_multiply(rotatedEllipMat, eig.eigenvectors);
    mat_free(rotatedEllipMat);
    Vector rotPrq = vec_multiply_mat(prq, eig.eigenvectors);
    double a_ = MAT_ELEM(diagonizedEllipMat, 0, 0);
    double b_ = MAT_ELEM(diagonizedEllipMat, 1, 1);
    double c_ = MAT_ELEM(diagonizedEllipMat, 2, 2);
    mat_free(diagonizedEllipMat);
    double p_ = VEC_ELEM(rotPrq, 0);
    double q_ = VEC_ELEM(rotPrq, 1);
    double r_ = VEC_ELEM(rotPrq, 2);
    vec_free(rotPrq);
    Matrix gain = mat_new(3, 3);
    MAT_ELEM(gain , 0, 0) = 1/sqrt((p_*p_)/(a_*a_) + (q_*q_)/(a_*b_) + (r_*r_)/(a_*c_) - d/a_);
    MAT_ELEM(gain , 1, 1) = 1/sqrt((p_*p_)/(a_*b_) + (q_*q_)/(b_*b_) + (r_*r_)/(b_*c_) - d/b_);
    MAT_ELEM(gain , 2, 2) = 1/sqrt((p_*p_)/(a_*c_) + (q_*q_)/(b_*c_) + (r_*r_)/(c_*c_) - d/c_);
    Matrix gain_Rotation = mat_multiply(gain, rotation);
    Matrix res = mat_multiply(eig.eigenvectors, gain_Rotation);
    eig_free(eig);
    mat_free(gain_Rotation);
    mat_free(gain);
    mat_free(rotation);
    return res;
}

Vector calib_calibrate_offset(Matrix ellipMat, Vector prq) {
    mat_inv_3x3(ellipMat);
    Vector res = mat_multiply_vec(ellipMat, prq);
    vec_negate(res);
    return res;
}

Callibration_t calib_calibrate_sensor(Vector x, Vector y, Vector z) {
    Callibration_t res;
    res.offset = (Vector)0;
    res.transform = (Matrix)0;
    if (x->size < 6) return res;  //needs at least 6 samples to fit an ellipsoid, though in real use you want far more.
    Ellipsoid_t ellip = ellipsoid_fit(x, y, z);
    Matrix M = calib_ellipsoid_matrix(ellip.coefA);
    Vector prq = vec_copy_subvec(ellip.coefB, 0, 3);
    double d = ellip.coefB->data[3];
    ellipsoid_free(ellip);
    res.transform = calib_calibrate_rotation(M, prq, d);
    res.offset = calib_calibrate_offset(M, prq);
    mat_free(M);
    vec_free(prq);
    return res;
}

bool calib_calibration_success(Callibration_t calib) {
    return !(vec_check_nan(calib.offset) || mat_check_nan(calib.transform));
}

//ie
//offset != NULL
//transform != NULL




void calib_calibrate_multiple_points(Callibration_t calib, Vector x, Vector y, Vector z) {
    assert(x->size == y->size);
    assert(x->size == z->size);

    for (int i = 0; i < x->size; i++) {
        VEC_ELEM(x, i) = VEC_ELEM(x, i) - VEC_ELEM(calib.offset, 0);
        VEC_ELEM(y, i) = VEC_ELEM(y, i) - VEC_ELEM(calib.offset, 1);
        VEC_ELEM(z, i) = VEC_ELEM(z, i) - VEC_ELEM(calib.offset, 2);
    }
    double tempX, tempY;
    for (int i = 0; i < x->size; i++) {
        tempX = VEC_ELEM(x, i);
        tempY = VEC_ELEM(y, i);

        VEC_ELEM(x, i) = tempX * MAT_ELEM(calib.transform, 0, 0) + tempY * MAT_ELEM(calib.transform, 0, 1) +
                        VEC_ELEM(z, i) * MAT_ELEM(calib.transform, 0, 2);
        VEC_ELEM(y, i) = tempX * MAT_ELEM(calib.transform, 1, 0) + tempY * MAT_ELEM(calib.transform, 1, 1) +
                        VEC_ELEM(z, i) * MAT_ELEM(calib.transform, 1, 2);
        VEC_ELEM(z, i) = tempX * MAT_ELEM(calib.transform, 2, 0) + tempY * MAT_ELEM(calib.transform, 2, 1) +
                        VEC_ELEM(z, i) * MAT_ELEM(calib.transform, 2, 2);
    }
}

void calib_calibrate_point(Callibration_t calib, Vector point) {
    vec_sub(point, calib.offset);
    mat_multiply_vec3_into(calib.transform, point);
}

double square_distance_variance(Vector x, Vector y, Vector z) {
    // var = sum((d - mean(d))^2)/(n-1)
    assert(x->size == y->size);
    assert(x->size == z->size);
    assert(x->size > 1);
    double mean = 0;
    double res = 0;
    double temp;
    for (int i = 0; i < x->size; i++) {
        mean += sqrt(VEC_ELEM(x, i) * VEC_ELEM(x, i) + VEC_ELEM(y, i) * VEC_ELEM(y, i) + VEC_ELEM(z, i) * VEC_ELEM(z, i));
    }
    mean /= x->size;
    for (int i = 0; i < x->size; i++) {
        temp = sqrt(VEC_ELEM(x, i) * VEC_ELEM(x, i) + VEC_ELEM(y, i) * VEC_ELEM(y, i) + VEC_ELEM(z, i) * VEC_ELEM(z, i)) - mean;
        res += temp * temp;
    }
    res = res/(x->size-1);
    return res;
}

void calib_free(Callibration_t calibA) {
    vec_free(calibA.offset);
    mat_free(calibA.transform);
}
