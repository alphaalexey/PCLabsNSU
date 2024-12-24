#pragma once

#include <vector>

#include "AlignedAllocator.hpp"

using matrix_value_type = float;

// Matrix type with aligned memory
using AlignedMatrix =
    std::vector<matrix_value_type, AlignedAllocator<matrix_value_type>>;

AlignedMatrix generateVector(size_t size);
AlignedMatrix generateMatrix(size_t size);
AlignedMatrix generateIdentityMatrix(size_t size);
