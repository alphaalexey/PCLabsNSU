#include "MatrixInv.hpp"

#include <cmath>

#include "MatrixMult.hpp"

void multByNumber(AlignedMatrix &A, matrix_value_type number) {
    for (auto &i : A) {
        i *= number;
    }
}

matrix_value_type findMaxAbsSumByRows(const AlignedMatrix &A, size_t size) {
    matrix_value_type result = 0.f;

    for (size_t i = 0; i < size; i++) {
        matrix_value_type current_sum = 0.f;
        for (size_t j = 0; j < size; j++) {
            current_sum += std::fabs(A[i * size + j]);
        }
        result = std::max(result, current_sum);
    }

    return result;
}

void transposeMatrix(AlignedMatrix &A, size_t size_M, size_t size_N) {
    for (size_t i = 0; i < size_N; i++) {
        for (size_t j = i + 1; j < size_M; j++) {
            std::swap(A[i * size_M + j], A[j * size_N + i]);
        }
    }
}

#define FUNC_INV(func)                                                         \
    AlignedMatrix inverseMatrix_##func(const AlignedMatrix &A, size_t size,    \
                                       size_t precision) {                     \
        AlignedMatrix B = A;                                                   \
        transposeMatrix(B, size, size);                                        \
                                                                               \
        matrix_value_type A1 = findMaxAbsSumByRows(B, size);                   \
        matrix_value_type A2 = findMaxAbsSumByRows(A, size);                   \
        multByNumber(B, 1.f / (A1 * A2));                                      \
                                                                               \
        AlignedMatrix R = generateIdentityMatrix(size);                        \
        AlignedMatrix R_series = R;                                            \
        AlignedMatrix result = R;                                              \
                                                                               \
        AlignedMatrix BA = func(B, A, size, size, size);                       \
        for (size_t i = 0; i < size * size; i++) {                             \
            R[i] -= BA[i];                                                     \
        }                                                                      \
                                                                               \
        for (size_t i = 1; i < precision; i++) {                               \
            R_series = func(R_series, R, size, size, size);                    \
                                                                               \
            for (size_t j = 0; j < size * size; j++) {                         \
                result[j] += R_series[j];                                      \
            }                                                                  \
        }                                                                      \
                                                                               \
        result = func(result, B, size, size, size);                            \
        return result;                                                         \
    }

FUNC_INV(matrixMultiply_slow_no_vectorize)
FUNC_INV(matrixMultiply_slow_vectorize)
FUNC_INV(matrixMultiply_default_no_vectorize)
FUNC_INV(matrixMultiply_default_vectorize)
FUNC_INV(matrixMultiply_manual_vectorize)
FUNC_INV(matrixMultiply_manual_vectorize_micro)
FUNC_INV(matrixMultiply_manual_vectorize_macro)
FUNC_INV(matrixMultiply_blas)
