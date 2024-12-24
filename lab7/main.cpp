#include <iostream>

#include <benchmark/benchmark.h>

#include "Matrix.hpp"
#include "MatrixInv.hpp"
#include "MatrixMult.hpp"

static constexpr size_t precision = 5'000;

static constexpr size_t matrix_size = 6 * 16;
static constexpr size_t matrix_size_M = 6 * 16 * 10; // rows in A matrix
static constexpr size_t matrix_size_K =
    6 * 16; // columns in A matrix and rows in B matrix
static constexpr size_t matrix_size_N = 6 * 16 * 20; // columns in B matrix

#define BENCH_INV(func)                                                        \
    static void BM_matrixInverse_##func(benchmark::State &state) {             \
        const auto a_count =                                                   \
            paddedSize(matrix_size * sizeof(matrix_value_type),                \
                       std::to_underlying(Alignment::AVX));                    \
        const auto A = generateMatrix(a_count);                                \
                                                                               \
        for (auto _ : state) {                                                 \
            benchmark::DoNotOptimize(                                          \
                inverseMatrix_##func(A, matrix_size, precision));              \
        }                                                                      \
    }                                                                          \
                                                                               \
    BENCHMARK(BM_matrixInverse_##func)->Unit(benchmark::kMillisecond);

BENCH_INV(matrixMultiply_slow_no_vectorize)
BENCH_INV(matrixMultiply_slow_vectorize)
BENCH_INV(matrixMultiply_default_no_vectorize)
BENCH_INV(matrixMultiply_default_vectorize)
BENCH_INV(matrixMultiply_manual_vectorize)
BENCH_INV(matrixMultiply_manual_vectorize_micro)
BENCH_INV(matrixMultiply_manual_vectorize_macro)
BENCH_INV(matrixMultiply_blas)

#define BENCH(func)                                                            \
    static void BM_##func(benchmark::State &state) {                           \
        const auto a_count = matrix_size_M * matrix_size_K;                    \
        const auto A =                                                         \
            generateVector(paddedSize(a_count * sizeof(matrix_value_type),     \
                                      std::to_underlying(Alignment::AVX)));    \
        const auto b_count = matrix_size_K * matrix_size_N;                    \
        const auto B =                                                         \
            generateVector(paddedSize(b_count * sizeof(matrix_value_type),     \
                                      std::to_underlying(Alignment::AVX)));    \
                                                                               \
        for (auto _ : state) {                                                 \
            benchmark::DoNotOptimize(                                          \
                func(A, B, matrix_size_M, matrix_size_K, matrix_size_N));      \
        }                                                                      \
    }                                                                          \
                                                                               \
    BENCHMARK(BM_##func)->Unit(benchmark::kMillisecond);

BENCH(matrixMultiply_slow_no_vectorize)
BENCH(matrixMultiply_slow_vectorize)
BENCH(matrixMultiply_default_no_vectorize)
BENCH(matrixMultiply_default_vectorize)
BENCH(matrixMultiply_manual_vectorize)
BENCH(matrixMultiply_manual_vectorize_micro)
BENCH(matrixMultiply_manual_vectorize_macro)
BENCH(matrixMultiply_blas)

BENCHMARK_MAIN();
