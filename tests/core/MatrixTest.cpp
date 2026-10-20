#include "core/Matrix.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

TEST(MatrixTest, StoresValuesAndReportsItsDimensions) {
    const mss::Matrix matrix({{1.0, 2.0}, {3.0, 4.0}});

    EXPECT_EQ(matrix.rows(), 2);
    EXPECT_EQ(matrix.columns(), 2);
    EXPECT_DOUBLE_EQ(matrix.at(1, 0), 3.0);
}

TEST(MatrixTest, RejectsEmptyAndJaggedInput) {
    EXPECT_THROW(mss::Matrix({}), std::invalid_argument);
    EXPECT_THROW(mss::Matrix({{1.0}, {2.0, 3.0}}), std::invalid_argument);
}

TEST(MatrixTest, RejectsOutOfRangeCoordinates) {
    const mss::Matrix matrix({{1.0}});

    EXPECT_THROW(matrix.at(1, 0), std::out_of_range);
    EXPECT_THROW(matrix.at(0, 1), std::out_of_range);
}
