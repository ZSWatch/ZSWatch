#ifndef COMPONENTS_GEOMETRY_MATRIX_H_
#define COMPONENTS_GEOMETRY_MATRIX_H_

#include "vector.h"

#define MAT_ELEM(MAT, ROW, COLUMN) MAT->data[(COLUMN) + (ROW)*MAT->columns]
#define MAT_ELEM_FLAT(MAT, ID) MAT->data[(ID)]

#define MAT_TO_TABLE(MAT, STRUCT) for (int ITER = 0; ITER < 9; ITER++) {STRUCT[ITER] = MAT_ELEM_FLAT(MAT, ITER);}
#define TABLE_TO_MAT(MAT, STRUCT) for (int ITER = 0; ITER < 9; ITER++) {MAT_ELEM_FLAT(MAT, ITER) = STRUCT[ITER];}

typedef struct {
    unsigned int rows;
    unsigned int columns;
    double * data;
} Matrix_t, *Matrix;

Matrix mat_new(const unsigned int rows, const unsigned int columns);
Matrix mat_new_I(const unsigned int rowsColumns);
Matrix mat_copy(Matrix matA);
Matrix mat_copy_submat(Matrix matA, const unsigned int frow, const unsigned int fcolumn,
                                    const unsigned int rows, const unsigned int columns);
Matrix mat_from_array(const double *array, const unsigned int rows, const unsigned int columns);
void mat_replace(Matrix matA, Matrix matB);

Vector mat_copy_column(Matrix matA, const unsigned int column);
void mat_paste_column(Matrix matA, Vector vecB, const unsigned int column);
Vector mat_get_diag(Matrix matA);

void mat_fill(Matrix matA, double val);
void mat_fill_diag(Matrix matA, double val);
void mat_rewrite(Matrix matA, Matrix matB);

Matrix mat_multiply(Matrix matA, Matrix matB);
void mat_multiply_vec3_into(Matrix matA, Vector vecB);
Matrix mat_multiply_MMT(Matrix matA);
Vector mat_multiply_vec(Matrix matA, Vector vecB);
Vector vec_multiply_mat(Vector vecA, Matrix matB);
void mat_add(Matrix matA, Matrix matB);
void mat_sub(Matrix matA, Matrix matB);
Matrix mat_transpose(Matrix matA);
void mat_orthogonalize(Matrix matA);

bool mat_is_upper_triang(Matrix matA, double tol);
bool mat_check_nan(Matrix matA);

Vector mat_linsolve_upperr_triang(Matrix R, Vector b);
bool mat_inv_4x4(Matrix matA);
bool mat_inv_3x3(Matrix matA);

Matrix mat_rotation_x(float rad);
Matrix mat_rotation_y(float rad);
Matrix mat_rotation_z(float rad);

void mat_print(Matrix matA);

void mat_free(Matrix matA);
void mat_from_array_free(Matrix matA);
#endif  // COMPONENTS_GEOMETRY_MATRIX_H_
