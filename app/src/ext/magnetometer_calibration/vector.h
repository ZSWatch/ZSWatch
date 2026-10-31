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

#pragma once

#include <stdbool.h>

#define VEC_ELEM(VEC, ID) VEC->data[ID]
#define VEC_X(VEC) VEC->data[0]
#define VEC_Y(VEC) VEC->data[1]
#define VEC_Z(VEC) VEC->data[2]

#define VEC_TO_TABLE(VEC, STRUCT) for (int ITER = 0; ITER < 3; ITER++) {STRUCT[ITER] = VEC_ELEM(VEC, ITER);}
#define TABLE_TO_VEC(VEC, STRUCT) for (int ITER = 0; ITER < 3; ITER++) {VEC_ELEM(VEC, ITER) = STRUCT[ITER];}

typedef struct {
    unsigned int size;
    double *data;
} Vector_t, *Vector;

Vector vec_new(unsigned int size);
Vector vec_from_array(const double *array, unsigned int size);
Vector vec_copy(Vector vecA);
Vector vec_copy_subvec(Vector vecA, unsigned int felem, unsigned int elems);
void vec_replace(Vector vecA, Vector vecB);

void vec_fill(Vector vecA, double val);

double vec_dot_product(Vector vecA, Vector vecB);
Vector vec_cross_product(Vector vecA, Vector vecB);
void vec_multiply_scalar(Vector vecA, double val);
void vec_add(Vector vecA, Vector vecB);
void vec_sub(Vector vecA, Vector vecB);
double vec_norm_square(Vector vecA);
double vec_norm(Vector vecA);

float vec_angle_between(Vector vecA, Vector vecB);
float vec_angle_between_2D(Vector vecA, Vector vecB);

bool vec_normalize(Vector vecA);
void vec_negate(Vector vecA);
void vec_rotate_x(Vector vec, float rad);
void vec_rotate_y(Vector vec, float rad);
void vec_rotate_z(Vector vec, float rad);

bool vec_contains(Vector vecA, double val, double tol);
bool vec_equal(Vector vecA, Vector vecB, double tol);
bool vec_check_nan(Vector vecA);

void vec_print(Vector vecA);

void vec_free(Vector vecA);
void vec_from_array_free(Vector vecA);
