#include "matrix.h"

#include "stdlib.h"
#include "stdio.h"
#include "assert.h"
#include "math.h"

Matrix mat_new(const unsigned int rows, const unsigned int columns) {
    Matrix res = malloc(sizeof(Matrix_t));
    res->rows = rows;
    res->columns = columns;
    res->data = calloc(rows*columns, sizeof(double));
    return res;
}
Matrix mat_new_I(const unsigned int rowsColumns) {
    Matrix res = mat_new(rowsColumns, rowsColumns);
    mat_fill_diag(res, 1);
    return res;
}
Matrix mat_copy(Matrix matA) {
    Matrix res = mat_new(matA->rows, matA->columns);
    mat_rewrite(res, matA);
    return res;
}
Matrix mat_copy_submat(Matrix matA, const unsigned int frow, const unsigned int fcolumn,
                                    const unsigned int rows, const unsigned int columns) {
    assert(matA->rows >= frow + rows);
    assert(matA->columns >= fcolumn + columns);

    Matrix res = mat_new(rows, columns);
    for (int i = 0; i < res->rows; i++) {
        for (int j = 0; j < res->columns; j++) {
            MAT_ELEM(res, i, j) = MAT_ELEM(matA, i + frow, j + fcolumn);
        }
    }
    return res;
}
Matrix mat_from_array(const double *array, const unsigned int rows, const unsigned int columns) {
    Matrix res = malloc(sizeof(Matrix_t));
    res->rows = columns;
    res->columns = columns;
    res->data = array;
    return res;
}
void mat_replace(Matrix matA, Matrix matB) {
    free(matA->data);
    matA->rows = matB->rows;
    matA->columns = matB->columns;
    matA->data = matB->data;
    free(matB);
}
Vector mat_copy_column(Matrix matA, const unsigned int column) {
    assert(matA->columns > column);

    Vector res = vec_new(matA->rows);
    for (int i = 0; i < matA->rows; i++) {
        VEC_ELEM(res, i) = MAT_ELEM(matA, i, column);
    }
    return res;
}
void mat_paste_column(Matrix matA, Vector vecB, const unsigned int column) {
    assert(matA->columns > column);

    for (int i = 0; i < matA->rows; i++) {
        MAT_ELEM(matA, i, column) = VEC_ELEM(vecB, i);
    }
}
Vector mat_get_diag(Matrix matA) {
    assert(matA->rows == matA->columns);

    Vector res = vec_new(matA->rows);
    for (int i = 0; i < matA->rows; i++) {
        VEC_ELEM(res, i) = MAT_ELEM(matA, i, i);
    }
    return res;
}
void mat_fill(Matrix matA, double val) {
    for (int i = 0; i < matA->rows * matA->columns; i++)  {
        MAT_ELEM_FLAT(matA, i) = val;
    }
}
void mat_fill_diag(Matrix matA, double val) {
    assert(matA->rows == matA->rows);
    for (int i = 0; i < matA->rows; i++)  {
        MAT_ELEM(matA, i, i) = val;
    }
}
void mat_rewrite(Matrix matA, Matrix matB) {
    assert(matA->rows == matB->rows);
    assert(matA->columns == matB->columns);
    for (int i = 0; i < matA->rows * matA->columns; i++)  {
        MAT_ELEM_FLAT(matA, i) = MAT_ELEM_FLAT(matB, i);
    }
}
Matrix mat_multiply(Matrix matA, Matrix matB) {
    assert(matA->columns == matB->rows);
    Matrix res = mat_new(matA->rows, matB->columns);
    for (int i = 0; i < res->rows; i++) {
        for (int j = 0; j < res->columns; j++) {
            for (int k = 0; k < matA->columns; k++) {
                MAT_ELEM(res, i, j) += MAT_ELEM(matA, i, k) * MAT_ELEM(matB, k, j);
            }
        }
    }
    return res;
}
void mat_multiply_vec3_into(Matrix matA, Vector vecB) {
    assert(vecB->size == matA->columns);
    assert(matA->rows == matA->columns);

    double tempX = VEC_X(vecB);
    double tempY = VEC_Y(vecB);
    VEC_X(vecB) = MAT_ELEM(matA, 0, 0) * tempX + MAT_ELEM(matA, 0, 1) * tempY + MAT_ELEM(matA, 0, 2) * VEC_Z(vecB);
    VEC_Y(vecB) = MAT_ELEM(matA, 1, 0) * tempX + MAT_ELEM(matA, 1, 1) * tempY + MAT_ELEM(matA, 1, 2) * VEC_Z(vecB);
    VEC_Z(vecB) = MAT_ELEM(matA, 2, 0) * tempX + MAT_ELEM(matA, 2, 1) * tempY + MAT_ELEM(matA, 2, 2) * VEC_Z(vecB);
}
Matrix mat_multiply_MMT(Matrix matA) {
    Matrix res = mat_new(matA->rows, matA->rows);
    for (int i = 0; i < res->rows; i++) {
        for (int j = 0; j < res->columns; j++) {
            for (int k = 0; k < matA->columns; k++) {
                MAT_ELEM(res, i, j) += MAT_ELEM(matA, i, k) * MAT_ELEM(matA, j, k);
            }
        }
    }
    return res;
}
Vector mat_multiply_vec(Matrix matA, Vector vecB) {
    assert(matA->columns == vecB->size);
    Vector res = vec_new(matA->rows);
    for (int i = 0; i < res->size; i++) {
        for (int k = 0; k < matA->columns; k++) {
            VEC_ELEM(res, i) += MAT_ELEM(matA, i, k) * VEC_ELEM(vecB, k);
        }
    }
    return res;
}
Vector vec_multiply_mat(Vector vecA, Matrix matB) {
    assert(vecA->size == matB->rows);
    Vector res = vec_new(matB->columns);
    for (int i = 0; i < res->size; i++) {
        for (int k = 0; k < matB->rows; k++) {
            VEC_ELEM(res, i) += VEC_ELEM(vecA, k) * MAT_ELEM(matB, k, i);
        }
    }
    return res;
}
void mat_add(Matrix matA, Matrix matB) {
    assert(matA->rows == matB->rows);
    assert(matA->columns == matB->columns);

    for (int i = 0; i < matA->rows * matA->columns; i++)  {
        MAT_ELEM_FLAT(matA, i) += MAT_ELEM_FLAT(matB, i);
    }
}

void mat_sub(Matrix matA, Matrix matB) {
    assert(matA->rows == matB->rows);
    assert(matA->columns == matB->columns);

    for (int i = 0; i < matA->rows * matA->columns; i++)  {
        MAT_ELEM_FLAT(matA, i) -= MAT_ELEM_FLAT(matB, i);
    }
}
Matrix mat_transpose(Matrix matA) {
    Matrix res = mat_new(matA->columns, matA->rows);
    for (int i = 0; i < res->rows; i++) {
        for (int j = 0; j < res->columns; j++) {
            MAT_ELEM(res, i, j) = MAT_ELEM(matA, j, i);
        }
    }
    return res;
}
void mat_orthogonalize(Matrix matA) {
    assert(matA->rows == matA->columns);
    for (int i = 1; i < matA->columns; i++) {
        Vector column = mat_copy_column(matA, i);
        for (int j = 1; j < i; j++) {
            Vector prevColumn =  mat_copy_column(matA, j);
            vec_multiply_scalar(prevColumn, vec_dot_product(prevColumn, column));
            vec_sub(column, prevColumn);
            vec_free(prevColumn);
        }
        vec_normalize(column);
        mat_paste_column(matA, column, i);
        vec_free(column);
    }
}
bool mat_is_upper_triang(Matrix matA, double tol) {
    assert(matA->rows == matA->columns);

    for (int i = 0; i < matA->rows; i++) {
        for (int j = 0; j < i; j++) {
            if (fabs(MAT_ELEM(matA, i, j)) > tol) {
                return false;
            }
        }
    }
    return true;
}

bool mat_check_nan(Matrix matA) {
    for (int i = 0; i < matA->rows*matA->columns; i++) {
        if (MAT_ELEM_FLAT(matA, i) != MAT_ELEM_FLAT(matA, i))
            return true;
    }
    return false;
}

Vector mat_linsolve_upperr_triang(Matrix R, Vector b) {
    assert(R->columns == b->size);
    assert(R->rows == R->columns);

    Vector solution = vec_new(b->size);
    float back_substitute;
    for (int i = b->size - 1; i >= 0; i--) {
        back_substitute = 0;
        for (int j = i+1; j <= b->size - 1; j++) {
            back_substitute += VEC_ELEM(solution, j) * MAT_ELEM(R, i, j);
        }
        VEC_ELEM(solution, i) =
            (VEC_ELEM(b, i) - back_substitute) / MAT_ELEM(R, i, i);
    }
    return solution;
}

bool mat_inv_4x4(Matrix matA) {
    double inv[16], det;

    double *m = matA->data;
    inv[0] = m[5]  * m[10] * m[15] -
             m[5]  * m[11] * m[14] -
             m[9]  * m[6]  * m[15] +
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] -
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] +
              m[4]  * m[11] * m[14] +
              m[8]  * m[6]  * m[15] -
              m[8]  * m[7]  * m[14] -
              m[12] * m[6]  * m[11] +
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] -
             m[4]  * m[11] * m[13] -
             m[8]  * m[5] * m[15] +
             m[8]  * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] +
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] -
               m[8]  * m[6] * m[13] -
               m[12] * m[5] * m[10] +
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] +
              m[1]  * m[11] * m[14] +
              m[9]  * m[2] * m[15] -
              m[9]  * m[3] * m[14] -
              m[13] * m[2] * m[11] +
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] -
             m[0]  * m[11] * m[14] -
             m[8]  * m[2] * m[15] +
             m[8]  * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] +
              m[0]  * m[11] * m[13] +
              m[8]  * m[1] * m[15] -
              m[8]  * m[3] * m[13] -
              m[12] * m[1] * m[11] +
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] -
              m[0]  * m[10] * m[13] -
              m[8]  * m[1] * m[14] +
              m[8]  * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
             m[1]  * m[7] * m[14] -
             m[5]  * m[2] * m[15] +
             m[5]  * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] +
              m[0]  * m[7] * m[14] +
              m[4]  * m[2] * m[15] -
              m[4]  * m[3] * m[14] -
              m[12] * m[2] * m[7] +
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] -
              m[0]  * m[7] * m[13] -
              m[4]  * m[1] * m[15] +
              m[4]  * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] +
               m[0]  * m[6] * m[13] +
               m[4]  * m[1] * m[14] -
               m[4]  * m[2] * m[13] -
               m[12] * m[1] * m[6] +
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
              m[1] * m[7] * m[10] +
              m[5] * m[2] * m[11] -
              m[5] * m[3] * m[10] -
              m[9] * m[2] * m[7] +
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
               m[0] * m[7] * m[9] +
               m[4] * m[1] * m[11] -
               m[4] * m[3] * m[9] -
               m[8] * m[1] * m[7] +
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (int i = 0; i < 16; i++)
        MAT_ELEM_FLAT(matA, i) = inv[i] * det;

    return true;
}

bool mat_inv_3x3(Matrix matA) {
    double inv[9], det;

    double *m = matA->data;
    inv[0] =  m[4] * m[8] - m[5] * m[7];
    inv[1] = -m[1] * m[8] + m[2] * m[7];
    inv[2] =  m[1] * m[5] - m[2] * m[4];
    inv[3] = -m[3] * m[8] + m[5] * m[6];
    inv[4] =  m[0] * m[8] - m[2] * m[6];
    inv[5] = -m[0] * m[5] + m[2] * m[3];
    inv[6] =  m[3] * m[7] - m[4] * m[6];
    inv[7] = -m[0] * m[7] + m[1] * m[6];
    inv[8] =  m[0] * m[4] - m[1] * m[3];

    det = m[0] * inv[0] + m[1] * inv[3] + m[2] * inv[6];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (int i = 0; i < 9; i++)
        MAT_ELEM_FLAT(matA, i) = inv[i] * det;

    return true;
}

Matrix mat_rotation_x(float rad) {
    Matrix res = mat_new(3, 3);
    float cosA = cos(rad);
    float sinA = sin(rad);

    MAT_ELEM(res, 0, 0) = 1.0f;
    MAT_ELEM(res, 0, 1) = 0.0f;
    MAT_ELEM(res, 0, 2) = 0.0f;

    MAT_ELEM(res, 1, 0) = 0.0f;
    MAT_ELEM(res, 1, 1) = cosA;
    MAT_ELEM(res, 1, 2) = -sinA;

    MAT_ELEM(res, 2, 0) = 0.0f;
    MAT_ELEM(res, 2, 1) = sinA;
    MAT_ELEM(res, 2, 2) = cosA;
    return res;
}

Matrix mat_rotation_y(float rad) {
    Matrix res = mat_new(3, 3);
    float cosA = cos(rad);
    float sinA = sin(rad);

    MAT_ELEM(res, 0, 0) = cosA;
    MAT_ELEM(res, 0, 1) = 0.0f;
    MAT_ELEM(res, 0, 2) = sinA;

    MAT_ELEM(res, 1, 0) = 0.0f;
    MAT_ELEM(res, 1, 1) = 1.0f;
    MAT_ELEM(res, 1, 2) = 0.0f;

    MAT_ELEM(res, 2, 0) = -sinA;
    MAT_ELEM(res, 2, 1) = 0.0f;
    MAT_ELEM(res, 2, 2) = cosA;
    return res;
}

Matrix mat_rotation_z(float rad) {
    Matrix res = mat_new(3, 3);
    float cosA = cos(rad);
    float sinA = sin(rad);

    MAT_ELEM(res, 0, 0) = cosA;
    MAT_ELEM(res, 0, 1) = -sinA;
    MAT_ELEM(res, 0, 2) = 0.0f;

    MAT_ELEM(res, 1, 0) = sinA;
    MAT_ELEM(res, 1, 1) = cosA;
    MAT_ELEM(res, 1, 2) = 0.0f;

    MAT_ELEM(res, 2, 0) = 0.0f;
    MAT_ELEM(res, 2, 1) = 0.0f;
    MAT_ELEM(res, 2, 2) = 1.0f;
    return res;
}

void mat_print(Matrix matA) {
    if (matA == NULL) return;
    printf("Size (%d, %d):\n", matA->rows, matA->columns);
    for (int i = 0; i < matA->rows; i++) {
        for (int j = 0; j < matA->columns; j++) {
            printf("%.3f, ", MAT_ELEM(matA, i, j));
        }
        printf("\n");
    }
}

void mat_free(Matrix matA) {
    if (matA == NULL) return;
    free(matA->data);
    free(matA);
    matA = NULL;
}

void mat_from_array_free(Matrix matA) {
    if (matA != NULL)
        free(matA);
}
