/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include "ellipsoid_fit.h"

#include "assert.h"
#include "matrix.h"
#include "eigen.h"

#define ALPHA 16

static Matrix ellipsoid_generate_data_mat(Vector x, Vector y, Vector z) {
    assert(x->size == y->size);
    assert(x->size == z->size);
    Matrix res = mat_new(10, x->size);
    for (int i = 0; i < res->columns; i++) {
        MAT_ELEM(res, 0, i) = VEC_ELEM(x, i) * VEC_ELEM(x, i);
        MAT_ELEM(res, 1, i) = VEC_ELEM(y, i) * VEC_ELEM(y, i);
        MAT_ELEM(res, 2, i) = VEC_ELEM(z, i) * VEC_ELEM(z, i);

        MAT_ELEM(res, 3, i) = 2 * VEC_ELEM(y, i) * VEC_ELEM(z, i);
        MAT_ELEM(res, 4, i) = 2 * VEC_ELEM(x, i) * VEC_ELEM(z, i);
        MAT_ELEM(res, 5, i) = 2 * VEC_ELEM(x, i) * VEC_ELEM(y, i);

        MAT_ELEM(res, 6, i) = 2 * VEC_ELEM(x, i);
        MAT_ELEM(res, 7, i) = 2 * VEC_ELEM(y, i);
        MAT_ELEM(res, 8, i) = 2 * VEC_ELEM(z, i);

        MAT_ELEM(res, 9, i) = 1;
    }
    return res;
}

static const double C_inv[] = {(double)(ALPHA - 4)/(- ALPHA*ALPHA + 3*ALPHA), -(double)(ALPHA - 2)/(- ALPHA*ALPHA + 3*ALPHA),
                        -(double)(ALPHA - 2)/(- ALPHA*ALPHA + 3*ALPHA), 0, 0, 0,
                        -(double)(ALPHA - 2)/(- ALPHA*ALPHA + 3*ALPHA), (double)(ALPHA - 4)/(- ALPHA*ALPHA + 3*ALPHA),
                        -(double)(ALPHA - 2)/(- ALPHA*ALPHA + 3*ALPHA), 0, 0, 0,
                        -(double)(ALPHA - 2)/(- ALPHA*ALPHA + 3*ALPHA), -(double)(ALPHA - 2)/(- ALPHA*ALPHA + 3*ALPHA),
                        (double)(ALPHA - 4)/(- ALPHA*ALPHA + 3*ALPHA), 0, 0, 0,
                        0, 0, 0, -1.f/ALPHA, 0, 0,
                        0, 0, 0, 0, -1.f/ALPHA, 0,
                        0, 0, 0, 0, 0, -1.f/ALPHA};
static Matrix ellipsoid_generate_inverse_restrain_mat(void) {
    return mat_from_array(C_inv, 6, 6);
}

Ellipsoid_t ellipsoid_fit(Vector x, Vector y, Vector z) {
    Matrix dataM = ellipsoid_generate_data_mat(x, y, z);
    Matrix invC = ellipsoid_generate_inverse_restrain_mat();
    Matrix S = mat_multiply_MMT(dataM);
    mat_free(dataM);
    Matrix S11 = mat_copy_submat(S, 0, 0, 6, 6);
    Matrix S12 = mat_copy_submat(S, 0, 6, 6, 4);
    Matrix S21 = mat_copy_submat(S, 6, 0, 4, 6);
    Matrix invS22 = mat_copy_submat(S, 6, 6, 4, 4);
    mat_inv_4x4(invS22);
    mat_free(S);
    // M = C1^-1*(S11-S12*(S22^-1*S21));
    Matrix invS22_S21 = mat_multiply(invS22, S21);
    mat_free(S21);
    mat_free(invS22);
    Matrix S12_invS22_S21 = mat_multiply(S12, invS22_S21);
    mat_free(S12);
    mat_sub(S11, S12_invS22_S21);
    mat_free(S12_invS22_S21);
    Matrix M = mat_multiply(invC, S11);
    mat_free(S11);
    mat_from_array_free(invC);
    Eigen_t eig = eig_solve(M);
    mat_free(M);
    Ellipsoid_t res;
    res.coefA = eig_eigvec_of_largest_eigval(eig);
    eig_free(eig);
    res.coefB = mat_multiply_vec(invS22_S21, res.coefA);
    vec_negate(res.coefB);
    mat_free(invS22_S21);
    return res;
}

void ellipsoid_free(Ellipsoid_t elip) {
    vec_free(elip.coefA);
    vec_free(elip.coefB);
}
