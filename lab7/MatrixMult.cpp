#include "MatrixMult.hpp"

#include <cassert>
#include <utility>

// #ifdef __AVX2__
#include <immintrin.h>
// #else
// #warning AVX2 is not available. Code will not compile!
// #endif

#include <cblas.h>

AlignedMatrix matrixMultiply_slow_no_vectorize(const AlignedMatrix &A,
                                               const AlignedMatrix &B,
                                               size_t size_M, size_t size_K,
                                               size_t size_N) {
    AlignedMatrix C(size_M * size_N);

#pragma clang loop vectorize(disable) interleave(disable)
    for (size_t i = 0; i < size_M; i++) {
#pragma clang loop vectorize(disable) interleave(disable)
        for (size_t k = 0; k < size_N; k++) {
#pragma clang loop vectorize(disable) interleave(disable)
            for (size_t j = 0; j < size_K; j++) {
                C[i * size_N + k] += A[i * size_K + j] * B[j * size_N + k];
            }
        }
    }
    return C;
}

AlignedMatrix matrixMultiply_slow_vectorize(const AlignedMatrix &A,
                                            const AlignedMatrix &B,
                                            size_t size_M, size_t size_K,
                                            size_t size_N) {
    AlignedMatrix C(size_M * size_N);

#pragma clang loop vectorize(enable) interleave(enable)
    for (size_t i = 0; i < size_M; i++) {
#pragma clang loop vectorize(enable) interleave(enable)
        for (size_t k = 0; k < size_N; k++) {
#pragma clang loop vectorize(enable) interleave(enable)
            for (size_t j = 0; j < size_K; j++) {
                C[i * size_N + k] += A[i * size_K + j] * B[j * size_N + k];
            }
        }
    }
    return C;
}

AlignedMatrix matrixMultiply_default_no_vectorize(const AlignedMatrix &A,
                                                  const AlignedMatrix &B,
                                                  size_t size_M, size_t size_K,
                                                  size_t size_N) {
    AlignedMatrix C(size_M * size_N);

#pragma clang loop vectorize(disable) interleave(disable)
    for (size_t i = 0; i < size_M; i++) {
        const auto c = C.begin() + i * size_N;

#pragma clang loop vectorize(disable) interleave(disable)
        for (size_t j = 0; j < size_K; j++) {
            const auto a = A[i * size_K + j];
            const auto b = B.begin() + j * size_N;

#pragma clang loop vectorize(disable) interleave(disable)
            for (size_t k = 0; k < size_N; k++) {
                c[k] += a * b[k];
            }
        }
    }
    return C;
}

AlignedMatrix matrixMultiply_default_vectorize(const AlignedMatrix &A,
                                               const AlignedMatrix &B,
                                               size_t size_M, size_t size_K,
                                               size_t size_N) {
    AlignedMatrix C(size_M * size_N);

#pragma clang loop vectorize(enable) interleave(enable)
    for (size_t i = 0; i < size_M; i++) {
        const auto c = C.begin() + i * size_N;

#pragma clang loop vectorize(enable) interleave(enable)
        for (size_t j = 0; j < size_K; j++) {
            const auto a = A[i * size_K + j];
            const auto b = B.begin() + j * size_N;

#pragma clang loop vectorize(enable) interleave(enable)
            for (size_t k = 0; k < size_N; k++) {
                c[k] += a * b[k];
            }
        }
    }
    return C;
}

// TODO: fixme
AlignedMatrix matrixMultiply_manual_vectorize(const AlignedMatrix &A,
                                              const AlignedMatrix &B,
                                              size_t size_M, size_t size_K,
                                              size_t size_N) {
    const auto c_count = size_M * size_N;
    AlignedMatrix C(paddedSize(c_count * sizeof(matrix_value_type),
                               std::to_underlying(Alignment::AVX) * 2),
                    0);

#pragma clang loop vectorize(disable) interleave(disable)
    for (size_t i = 0; i < size_M; i++) {
        const auto c = C.begin() + i * size_N;

#pragma clang loop vectorize(disable) interleave(disable)
        for (size_t j = 0; j < size_K; j++) {
            const __m256 a = _mm256_set1_ps(A[i * size_K + j]);
            const auto b = B.begin() + j * size_N;

#pragma clang loop vectorize(disable) interleave(disable)
            for (size_t k = 0; k < size_N;
                 k += 32 / sizeof(matrix_value_type) * 2) {
                _mm256_storeu_ps(&c[k],
                                 _mm256_fmadd_ps(a, _mm256_loadu_ps(&b[k]),
                                                 _mm256_loadu_ps(&c[k])));
                _mm256_storeu_ps(&c[k + 8],
                                 _mm256_fmadd_ps(a, _mm256_loadu_ps(&b[k + 8]),
                                                 _mm256_loadu_ps(&c[k + 8])));
            }
        }
    }
    return C;
}

void micro_6x16(size_t size_K, const matrix_value_type *A, size_t lda,
                size_t step, const matrix_value_type *B, size_t ldb,
                matrix_value_type *C, size_t ldc) {
    __m256 c00 = _mm256_setzero_ps();
    __m256 c10 = _mm256_setzero_ps();
    __m256 c20 = _mm256_setzero_ps();
    __m256 c30 = _mm256_setzero_ps();
    __m256 c40 = _mm256_setzero_ps();
    __m256 c50 = _mm256_setzero_ps();
    __m256 c01 = _mm256_setzero_ps();
    __m256 c11 = _mm256_setzero_ps();
    __m256 c21 = _mm256_setzero_ps();
    __m256 c31 = _mm256_setzero_ps();
    __m256 c41 = _mm256_setzero_ps();
    __m256 c51 = _mm256_setzero_ps();
    const size_t offset0 = lda * 0;
    const size_t offset1 = lda * 1;
    const size_t offset2 = lda * 2;
    const size_t offset3 = lda * 3;
    const size_t offset4 = lda * 4;
    const size_t offset5 = lda * 5;
    __m256 b0, b1, a0, a1;
    for (size_t k = 0; k < size_K; k++) {
        b0 = _mm256_loadu_ps(B + 0);
        b1 = _mm256_loadu_ps(B + 8);
        a0 = _mm256_set1_ps(A[offset0]);
        a1 = _mm256_set1_ps(A[offset1]);
        c00 = _mm256_fmadd_ps(a0, b0, c00);
        c01 = _mm256_fmadd_ps(a0, b1, c01);
        c10 = _mm256_fmadd_ps(a1, b0, c10);
        c11 = _mm256_fmadd_ps(a1, b1, c11);
        a0 = _mm256_set1_ps(A[offset2]);
        a1 = _mm256_set1_ps(A[offset3]);
        c20 = _mm256_fmadd_ps(a0, b0, c20);
        c21 = _mm256_fmadd_ps(a0, b1, c21);
        c30 = _mm256_fmadd_ps(a1, b0, c30);
        c31 = _mm256_fmadd_ps(a1, b1, c31);
        a0 = _mm256_set1_ps(A[offset4]);
        a1 = _mm256_set1_ps(A[offset5]);
        c40 = _mm256_fmadd_ps(a0, b0, c40);
        c41 = _mm256_fmadd_ps(a0, b1, c41);
        c50 = _mm256_fmadd_ps(a1, b0, c50);
        c51 = _mm256_fmadd_ps(a1, b1, c51);
        B += ldb;
        A += step;
    }
    _mm256_storeu_ps(C + 0, _mm256_add_ps(c00, _mm256_loadu_ps(C + 0)));
    _mm256_storeu_ps(C + 8, _mm256_add_ps(c01, _mm256_loadu_ps(C + 8)));
    C += ldc;
    _mm256_storeu_ps(C + 0, _mm256_add_ps(c10, _mm256_loadu_ps(C + 0)));
    _mm256_storeu_ps(C + 8, _mm256_add_ps(c11, _mm256_loadu_ps(C + 8)));
    C += ldc;
    _mm256_storeu_ps(C + 0, _mm256_add_ps(c20, _mm256_loadu_ps(C + 0)));
    _mm256_storeu_ps(C + 8, _mm256_add_ps(c21, _mm256_loadu_ps(C + 8)));
    C += ldc;
    _mm256_storeu_ps(C + 0, _mm256_add_ps(c30, _mm256_loadu_ps(C + 0)));
    _mm256_storeu_ps(C + 8, _mm256_add_ps(c31, _mm256_loadu_ps(C + 8)));
    C += ldc;
    _mm256_storeu_ps(C + 0, _mm256_add_ps(c40, _mm256_loadu_ps(C + 0)));
    _mm256_storeu_ps(C + 8, _mm256_add_ps(c41, _mm256_loadu_ps(C + 8)));
    C += ldc;
    _mm256_storeu_ps(C + 0, _mm256_add_ps(c50, _mm256_loadu_ps(C + 0)));
    _mm256_storeu_ps(C + 8, _mm256_add_ps(c51, _mm256_loadu_ps(C + 8)));
}

AlignedMatrix matrixMultiply_manual_vectorize_micro(const AlignedMatrix &A,
                                                    const AlignedMatrix &B,
                                                    size_t size_M,
                                                    size_t size_K,
                                                    size_t size_N) {
    assert(size_M % 6 == 0);
    assert(size_N % 16 == 0);

    const auto c_count = size_M * size_N;
    AlignedMatrix C(paddedSize(c_count * sizeof(matrix_value_type),
                               std::to_underlying(Alignment::AVX) * 12));

#pragma clang loop vectorize(disable) interleave(disable)
    for (size_t i = 0; i < size_M; i += 6) {
        const auto a = A.data() + i * size_K;
        const auto c = C.begin() + i * size_N;

#pragma clang loop vectorize(disable) interleave(disable)
        for (size_t k = 0; k < size_N; k += 16) {
            micro_6x16(size_K, a, size_K, 1, B.data() + k, size_N, &c[k],
                       size_N);
        }
    }
    return C;
}

struct buf_t {
    matrix_value_type *p;
    size_t n;

    buf_t(size_t size)
        : p((matrix_value_type *)_mm_malloc(size * 4, 64)), n(size) {}
    ~buf_t() { _mm_free(p); }
};

// TODO: need fix to work not only with matrix_value_type=float
void reorder_b_16(size_t size_K, const matrix_value_type *B, size_t ldb,
                  matrix_value_type *bufB) {
    for (size_t k = 0; k < size_K; k++, B += ldb, bufB += 16) {
        _mm256_storeu_ps(bufB, _mm256_loadu_ps(B));
        _mm256_storeu_ps(bufB + 8, _mm256_loadu_ps(B + 8));
    }
}

void macro_L1(size_t size_M, size_t size_N, size_t size_K,
              const matrix_value_type *A, size_t lda,
              const matrix_value_type *B, size_t ldb, matrix_value_type *bufB,
              matrix_value_type *C, size_t ldc) {
    for (size_t k = 0; k < size_N; k += 16) {
        reorder_b_16(size_K, B + k, ldb, bufB);
        for (size_t i = 0; i < size_M; i += 6)
            micro_6x16(size_K, A + i * lda, lda, 1, bufB, 16, C + i * ldc + k,
                       ldc);
    }
}

AlignedMatrix matrixMultiply_manual_vectorize_macro(const AlignedMatrix &A,
                                                    const AlignedMatrix &B,
                                                    size_t size_M,
                                                    size_t size_K,
                                                    size_t size_N) {
    assert(size_M % 6 == 0);
    assert(size_N % 16 == 0);

    const auto c_count = size_M * size_N;
    AlignedMatrix C(paddedSize(c_count * sizeof(matrix_value_type),
                               std::to_underlying(Alignment::AVX) * 12));

    const size_t L1 = 32 * 1024;
    size_t mK = std::min(L1 / 4 / 16, size_K);
    buf_t bufB(16 * mK);

#pragma clang loop vectorize(disable) interleave(disable)
    for (size_t k = 0; k < size_N; k += mK) {
        size_t dK = std::min(size_K, k + mK) - k;
        macro_L1(size_M, size_N, dK, A.data() + k, size_K,
                 B.data() + k * size_N, size_N, bufB.p, C.data(), size_N);
    }
    return C;
}

AlignedMatrix matrixMultiply_blas(const AlignedMatrix &A,
                                  const AlignedMatrix &B, size_t size_M,
                                  size_t size_K, size_t size_N) {
    AlignedMatrix C(size_M * size_N);

    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, size_M, size_N,
                size_K, 1.0, A.data(), size_K, B.data(), size_K, 0.0, C.data(),
                size_N);
    return C;
}
