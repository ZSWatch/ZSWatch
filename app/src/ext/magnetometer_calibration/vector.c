#include "vector.h"

#include "assert.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

Vector vec_new(unsigned int size) {
    Vector res = malloc(sizeof(Vector_t));
    res->size = size;
    res->data = calloc(size, sizeof(double));
    return res;
}

Vector vec_from_array(const double *array, unsigned int size) {
    Vector res = malloc(sizeof(Vector_t));
    res->size = size;
    res->data = array;
    return res;
}

Vector vec_copy(Vector vecA) {
    Vector res = vec_new(vecA->size);
    for (int i = 0; i < vecA->size; i++) {
        VEC_ELEM(res, i) = VEC_ELEM(vecA, i);
    }
    return res;
}

Vector vec_copy_subvec(Vector vecA, unsigned int felem, unsigned int elems) {
    assert(vecA->size >= felem + elems);
    Vector res = vec_new(elems);
    for (int i = 0; i < elems; i++) {
        VEC_ELEM(res, i) = VEC_ELEM(vecA, felem + i);
    }
    return res;
}

void vec_replace(Vector vecA, Vector vecB) {
    free(vecA->data);
    vecA->size = vecB->size;
    vecA->data = vecB->data;
    free(vecB);
}

void vec_fill(Vector vecA, double val) {
    for (int i = 0; i < vecA->size; i++) {
        VEC_ELEM(vecA, i) = val;
    }
}

double vec_dot_product(Vector vecA, Vector vecB) {
    assert(vecA->size == vecB->size);
    double res = 0;
    for (int i = 0; i < vecA->size; i++) {
        res += VEC_ELEM(vecA, i) * VEC_ELEM(vecB, i);
    }
    return res;
}

Vector vec_cross_product(Vector vecA, Vector vecB) {
    assert(vecA->size == 3 && vecB->size == 3);
    Vector res = vec_new(3);

    VEC_ELEM(res, 0) = VEC_ELEM(vecA, 1) * VEC_ELEM(vecB, 2) - VEC_ELEM(vecA, 2) * VEC_ELEM(vecB, 1);
    VEC_ELEM(res, 1) = VEC_ELEM(vecA, 2) * VEC_ELEM(vecB, 0) - VEC_ELEM(vecA, 0) * VEC_ELEM(vecB, 2);
    VEC_ELEM(res, 2) = VEC_ELEM(vecA, 0) * VEC_ELEM(vecB, 1) - VEC_ELEM(vecA, 1) * VEC_ELEM(vecB, 0);

    return res;
}

void vec_multiply_scalar(Vector vecA, double val) {
    for (int i = 0; i < vecA->size; i++) {
        VEC_ELEM(vecA, i) *= val;
    }
}
void vec_add(Vector vecA, Vector vecB) {
    assert(vecA->size == vecB->size);
    for (int i = 0; i < vecA->size; i++) {
        VEC_ELEM(vecA, i) += VEC_ELEM(vecB, i);
    }
}
void vec_sub(Vector vecA, Vector vecB) {
    assert(vecA->size == vecB->size);
    for (int i = 0; i < vecA->size; i++) {
        VEC_ELEM(vecA, i) -= VEC_ELEM(vecB, i);
    }
}
double vec_norm_square(Vector vecA) {
    double res = 0;
    for (int i = 0; i < vecA->size; i++) {
        res += VEC_ELEM(vecA, i) * VEC_ELEM(vecA, i);
    }
    return res;
}
double vec_norm(Vector vecA) {
    return sqrt(vec_norm_square(vecA));
}
float vec_angle_between(Vector vecA, Vector vecB) {
    return acos((VEC_X(vecA) * VEC_X(vecB) + VEC_Y(vecA) * VEC_Y(vecB) + VEC_Z(vecA) * VEC_Z(vecB)) / sqrt(
            (VEC_X(vecA) * VEC_X(vecA) + VEC_Y(vecA) * VEC_Y(vecA) + VEC_Z(vecA) * VEC_Z(vecA)) *
            (VEC_X(vecB) * VEC_X(vecB) + VEC_Y(vecB) * VEC_Y(vecB) + VEC_Z(vecB) * VEC_Z(vecB))));
}

float vec_angle_between_2D(Vector vecA, Vector vecB) {
    return atan2(VEC_Y(vecA), VEC_X(vecA)) - atan2(VEC_Y(vecB), VEC_X(vecB));
}

bool vec_normalize(Vector vecA) {
    double norm = vec_norm(vecA);
    if (norm == 0) return false;
    else
        vec_multiply_scalar(vecA, 1/norm);
    return true;
}

void vec_negate(Vector vecA) {
    for (int i = 0; i < vecA->size; i++) {
        VEC_ELEM(vecA, i) = -VEC_ELEM(vecA, i);
    }
}

void vec_rotate_x(Vector vec, float rad) {
    double cosA = cos(rad);
    double sinA = sin(rad);

    double temp = VEC_ELEM(vec, 1);
    VEC_ELEM(vec, 1) = cosA * temp - sinA * VEC_ELEM(vec, 2);
    VEC_ELEM(vec, 2) = cosA * temp - sinA * VEC_ELEM(vec, 2);
}

void vec_rotate_y(Vector vec, float rad) {
    double cosA = cos(rad);
    double sinA = sin(rad);

    double temp = VEC_ELEM(vec, 0);

    VEC_ELEM(vec, 0) = cosA * temp + sinA * VEC_ELEM(vec, 2);
    VEC_ELEM(vec, 2) = -sinA * temp + cosA * VEC_ELEM(vec, 2);
}

void vec_rotate_z(Vector vec, float rad) {
    double cosA = cos(rad);
    double sinA = sin(rad);

    double temp = VEC_ELEM(vec, 0);

    VEC_ELEM(vec, 0) = cosA * temp - sinA * VEC_ELEM(vec, 1);
    VEC_ELEM(vec, 1) = sinA * temp + cosA * VEC_ELEM(vec, 1);
}

bool vec_contains(Vector vecA, double val, double tol) {
    for (int i = 0; i < vecA->size; i++) {
        if (fabs(VEC_ELEM(vecA, i)) > tol)
            return true;
    }
    return false;
}

bool vec_equal(Vector vecA, Vector vecB, double tol) {
    if (vecA->size != vecB->size) return false;
    for (int i = 0; i < vecA->size; i++) {
        if (fabs(VEC_ELEM(vecA, i) - VEC_ELEM(vecB, i)) > tol)
            return false;
    }
    return true;
}

bool vec_check_nan(Vector vecA) {
    for (int i = 0; i < vecA->size; i++) {
        if (VEC_ELEM(vecA, i) != VEC_ELEM(vecA, i))
            return true;
    }
    return false;
}

void vec_print(Vector vecA) {
    if (vecA == NULL) return;
    printf("[");
    for (int i = 0; i < vecA->size; i++) {
        printf("%.10f, ", VEC_ELEM(vecA, i));
    }
    printf("]\n");
}

void vec_free(Vector vecA) {
    if (vecA == NULL) return;
    if (vecA->data != NULL)
        free(vecA->data);
    free(vecA);
}
void vec_from_array_free(Vector vecA) {
    if (vecA != NULL)
        free(vecA);
}
