#include "eigen.h"

#include "stdlib.h"
#include "assert.h"
#include "qr.h"

#define EIGEN_TOLERANCE 1.0E-6
#define EIGEN_SINGULARITY_TOLERANCE 1.0E-10
#define EIGEN_MAX_ITER 100

Vector eig_solve_eigenvalues(Matrix M) {
    Matrix X = mat_copy(M);
    int i = 0;
    do {
        QR_t qr = qr_decomposition(X);
        Matrix RQ = mat_multiply(qr.R, qr.Q);
        mat_replace(X, RQ);
        qr_free(qr);
        i++;
    } while (!mat_is_upper_triang(X, EIGEN_TOLERANCE) && (i < EIGEN_MAX_ITER));
    Vector res = mat_get_diag(X);
    mat_free(X);
    return res;
}

Vector linsolve_from_qr(QR_t qr, Vector b) {
    Vector rhs = mat_multiply_vec(qr.Q, b);
    Vector solution = mat_linsolve_upperr_triang(qr.R, rhs);
    vec_free(rhs);
    return solution;
}

Vector linsolve_qr(Matrix matA, Vector b) {
    assert(matA->rows == b->size);
    QR_t qr = qr_decomposition(matA);
    Vector solution = linsolve_from_qr(qr, b);
    qr_free(qr);
    return solution;
}

Vector eigen_backsolve(Matrix matA, double eigVal) {
    Vector current = vec_new(matA->rows);
    vec_fill(current, 1);
    Vector previous = NULL;
    double lambda = eigVal + ((double) rand() / (double) RAND_MAX) * 0.000001;
    Matrix matLambda = mat_new(matA->rows, matA->columns);
    mat_fill_diag(matLambda, lambda);
    Matrix matMinusLambda = mat_copy(matA);
    mat_sub(matMinusLambda, matLambda);
    mat_free(matLambda);

    int i = 0;
    do {
        if (i > 0) {
            vec_free(previous);
        }
        previous = current;
        current = linsolve_qr(matMinusLambda, previous);

        if (VEC_ELEM(current, 0) < 0) {
            vec_negate(current);
        }
        vec_normalize(current);
        i++;
    } while (!vec_equal(current, previous, EIGEN_TOLERANCE) && (i < EIGEN_MAX_ITER));
    mat_free(matMinusLambda);
    vec_free(previous);
    return current;
}
Matrix eigen_solve_eigenvectors(Matrix matA, Vector eigenvalues) {
    assert(eigenvalues->size = matA->rows);
    assert(eigenvalues->size = matA->columns);

    double eigVal;
    int eigCount = matA->columns;
    Matrix eigenvectors = mat_new(eigCount, eigCount);

    for (int i = 0; i < eigCount; i++) {
        eigVal = VEC_ELEM(eigenvalues, i);
        Vector eigenvector = eigen_backsolve(matA, eigVal);
        mat_paste_column(eigenvectors, eigenvector, i);
        vec_free(eigenvector);
    }

    return eigenvectors;
}
Eigen_t eig_solve(Matrix matA) {
    assert(matA->columns == matA->rows);

    Eigen_t res;
    res.eigenvalues = eig_solve_eigenvalues(matA);
    res.eigenvectors = eigen_solve_eigenvectors(matA, res.eigenvalues);
    return res;
}

Vector eig_eigvec_of_largest_eigval(Eigen_t eigA) {
    assert(eigA.eigenvalues->size > 0);

    double largestEigval = VEC_ELEM(eigA.eigenvalues, 0);
    unsigned int largestEigvalId = 0;
    for (int i = 1; i < eigA.eigenvalues->size; i++) {
        if (VEC_ELEM(eigA.eigenvalues, i) > largestEigval) {
            largestEigval = VEC_ELEM(eigA.eigenvalues, i);
            largestEigvalId = i;
        }
    }
    return mat_copy_column(eigA.eigenvectors, largestEigvalId);
}

void eig_free(Eigen_t eigA) {
    vec_free(eigA.eigenvalues);
    mat_free(eigA.eigenvectors);
}
