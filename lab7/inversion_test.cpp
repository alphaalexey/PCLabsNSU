#include <gtest/gtest.h>
#include <iostream>
#include <new>
#include <random>

#include <Eigen/Dense>

#include "Matrix.hpp"
#include "MatrixInv.hpp"
#include "MatrixMult.hpp"

static constexpr size_t precision = 50'000;

class MatrixParametrizedTest : public ::testing::TestWithParam<std::size_t> {
protected:
    std::size_t N;
};

TEST_P(MatrixParametrizedTest, test1) {
    std::size_t N = GetParam();

    const auto a_size = paddedSize(N * sizeof(matrix_value_type),
                                   std::to_underlying(Alignment::AVX) * 2);
    const auto a_count = a_size / sizeof(matrix_value_type) +
                         ((a_size % sizeof(matrix_value_type)) != 0);
    const auto a = generateMatrix(N);
    const auto a_inv =
        inverseMatrix_matrixMultiply_manual_vectorize(a, N, precision);

    Eigen::MatrixXf mat_a = Eigen::Map<const Eigen::MatrixXf>(a.data(), N, N);
    Eigen::MatrixXf mat_inv =
        Eigen::Map<const Eigen::MatrixXf>(a_inv.data(), N, N);

    std::cout << "--------------------------------------------------"
              << std::endl;
    std::cout << mat_a << std::endl;
    std::cout << "--------------------------------------------------"
              << std::endl;
    std::cout << std::fixed << std::setprecision(2) << mat_a * mat_inv << std::endl;
    std::cout << "--------------------------------------------------"
              << std::endl;
    std::cout << mat_inv << std::endl;
    std::cout << "--------------------------------------------------"
              << std::endl;
    auto temp = mat_a.colPivHouseholderQr().inverse();
    std::cout << temp << std::endl;
    std::cout << "--------------------------------------------------"
              << std::endl;
    std::cout << (mat_inv - temp).norm() / temp.norm() << std::endl;

    EXPECT_TRUE((mat_a * mat_inv).isIdentity(1e-3));
}

INSTANTIATE_TEST_SUITE_P(MatrixInversionTests, MatrixParametrizedTest,
                         ::testing::Values(16));
