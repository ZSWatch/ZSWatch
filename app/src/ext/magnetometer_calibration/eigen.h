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

#include "matrix.h"
#include "vector.h"

typedef struct {
    Vector eigenvalues;
    Matrix eigenvectors;
} Eigen_t;

Eigen_t eig_solve(Matrix matA);
Vector eig_eigvec_of_largest_eigval(Eigen_t eigA);
void eig_free(Eigen_t eigA);


