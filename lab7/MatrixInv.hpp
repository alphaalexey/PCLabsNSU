#pragma once

#include "Matrix.hpp"

AlignedMatrix
inverseMatrix_matrixMultiply_slow_no_vectorize(const AlignedMatrix &A,
                                               size_t size, size_t precision);
AlignedMatrix
inverseMatrix_matrixMultiply_slow_vectorize(const AlignedMatrix &A, size_t size,
                                            size_t precision);
AlignedMatrix inverseMatrix_matrixMultiply_default_no_vectorize(
    const AlignedMatrix &A, size_t size, size_t precision);
AlignedMatrix
inverseMatrix_matrixMultiply_default_vectorize(const AlignedMatrix &A,
                                               size_t size, size_t precision);
AlignedMatrix
inverseMatrix_matrixMultiply_manual_vectorize(const AlignedMatrix &A,
                                              size_t size, size_t precision);
AlignedMatrix inverseMatrix_matrixMultiply_manual_vectorize_micro(
    const AlignedMatrix &A, size_t size, size_t precision);
AlignedMatrix inverseMatrix_matrixMultiply_manual_vectorize_macro(
    const AlignedMatrix &A, size_t size, size_t precision);
AlignedMatrix inverseMatrix_matrixMultiply_blas(const AlignedMatrix &A,
                                                size_t size, size_t precision);
