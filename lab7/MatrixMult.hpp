#pragma once

#include "Matrix.hpp"

AlignedMatrix matrixMultiply_slow_no_vectorize(const AlignedMatrix &A,
                                               const AlignedMatrix &B,
                                               size_t size_M, size_t size_K,
                                               size_t size_N);

AlignedMatrix matrixMultiply_slow_vectorize(const AlignedMatrix &A,
                                            const AlignedMatrix &B,
                                            size_t size_M, size_t size_K,
                                            size_t size_N);

AlignedMatrix matrixMultiply_default_no_vectorize(const AlignedMatrix &A,
                                                  const AlignedMatrix &B,
                                                  size_t size_M, size_t size_K,
                                                  size_t size_N);

AlignedMatrix matrixMultiply_default_vectorize(const AlignedMatrix &A,
                                               const AlignedMatrix &B,
                                               size_t size_M, size_t size_K,
                                               size_t size_N);

AlignedMatrix matrixMultiply_manual_vectorize(const AlignedMatrix &A,
                                              const AlignedMatrix &B,
                                              size_t size_M, size_t size_K,
                                              size_t size_N);

AlignedMatrix matrixMultiply_manual_vectorize_micro(const AlignedMatrix &A,
                                                    const AlignedMatrix &B,
                                                    size_t size_M,
                                                    size_t size_K,
                                                    size_t size_N);

AlignedMatrix matrixMultiply_manual_vectorize_macro(const AlignedMatrix &A,
                                                    const AlignedMatrix &B,
                                                    size_t size_M,
                                                    size_t size_K,
                                                    size_t size_N);

AlignedMatrix matrixMultiply_blas(const AlignedMatrix &A,
                                  const AlignedMatrix &B, size_t size_M,
                                  size_t size_K, size_t size_N);
