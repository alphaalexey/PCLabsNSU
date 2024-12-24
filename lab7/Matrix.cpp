#include "Matrix.hpp"

#include <random>

AlignedMatrix generateVector(size_t size) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<matrix_value_type> dist(0.f, 10.f);

    AlignedMatrix vec(size);
    for (size_t i = 0; i < size; i++) {
        vec[i] = dist(rng);
    }
    return vec;
}

AlignedMatrix generateMatrix(size_t size) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<matrix_value_type> dist(0.f, 10.f);

    AlignedMatrix matrix(size * size, 0.f);
    for (size_t i = 0; i < size; i++) {
        matrix_value_type rowSum = 0.0f;
        for (size_t j = 0; j < size; j++) {
            if (i != j) {
                matrix_value_type value = dist(rng);
                matrix[i * size + j] = value;
                rowSum += std::abs(value);
            }
        }
        // Задаём диагональный элемент, который доминирует над суммой остальных
        // элементов
        matrix[i * size + i] =
            rowSum + dist(rng) + 1.f; // +1 гарантирует положительную разницу
    }
    return matrix;
}

AlignedMatrix generateIdentityMatrix(size_t size) {
    AlignedMatrix matrix(size * size, 0.f);
    for (size_t i = 0; i < size; i++) {
        matrix[i * size + i] = 1.f;
    }
    return matrix;
}
