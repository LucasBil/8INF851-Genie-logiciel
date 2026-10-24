#include "core/Cpu.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

TEST(CpuTest, ReportsThatOperationCostsAreNotSpecified) {
    const mss::Cpu cpu;
    mss::Operation operation{mss::OperationName::Rotate, {}};

    EXPECT_THROW(cpu.getOperationCost(operation), std::logic_error);
}

TEST(CpuTest, PermutesMatrixColumns) {
    mss::Cpu cpu;
    mss::Matrix matrix({{1.0, 2.0}, {3.0, 4.0}});

    mss::Matrix& result = cpu.permuteColumn(matrix);

    EXPECT_EQ(&result, &matrix);
    EXPECT_DOUBLE_EQ(matrix.at(0, 0), 2.0);
    EXPECT_DOUBLE_EQ(matrix.at(0, 1), 1.0);
    EXPECT_DOUBLE_EQ(matrix.at(1, 0), 4.0);
    EXPECT_DOUBLE_EQ(matrix.at(1, 1), 3.0);
}

TEST(CpuTest, PermutesMatrixRows) {
    mss::Cpu cpu;
    mss::Matrix matrix({{1.0, 2.0}, {3.0, 4.0}});

    mss::Matrix& result = cpu.permuteLine(matrix);

    EXPECT_EQ(&result, &matrix);
    EXPECT_DOUBLE_EQ(matrix.at(0, 0), 3.0);
    EXPECT_DOUBLE_EQ(matrix.at(0, 1), 4.0);
    EXPECT_DOUBLE_EQ(matrix.at(1, 0), 1.0);
    EXPECT_DOUBLE_EQ(matrix.at(1, 1), 2.0);
}

TEST(CpuTest, RotatesMatrix90DegreesClockwise) {
    mss::Cpu cpu;
    mss::Matrix matrix({{1.0, 2.0}, {3.0, 4.0}});

    mss::Matrix& result = cpu.rotate(matrix);

    EXPECT_EQ(&result, &matrix);
    EXPECT_DOUBLE_EQ(matrix.at(0, 0), 3.0);
    EXPECT_DOUBLE_EQ(matrix.at(0, 1), 1.0);
    EXPECT_DOUBLE_EQ(matrix.at(1, 0), 4.0);
    EXPECT_DOUBLE_EQ(matrix.at(1, 1), 2.0);
}

TEST(CpuTest, CalculatesMatrixDeterminant) {
    mss::Cpu cpu;
    mss::Matrix matrix({{1.0, 2.0}, {3.0, 4.0}});

    mss::Matrix& result = cpu.calcDeterminant(matrix);

    EXPECT_EQ(&result, &matrix);
    ASSERT_EQ(matrix.rows(), 1);
    ASSERT_EQ(matrix.columns(), 1);
    EXPECT_DOUBLE_EQ(matrix.at(0, 0), -2.0);
}

TEST(CpuTest, RejectsNonTwoByTwoMatricesForMatrixCalculations) {
    mss::Cpu cpu;
    mss::Matrix columnPermutation({{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});
    mss::Matrix rowPermutation({{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});
    mss::Matrix rotation({{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});
    mss::Matrix determinant({{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});

    EXPECT_THROW(cpu.permuteColumn(columnPermutation), std::invalid_argument);
    EXPECT_THROW(cpu.permuteLine(rowPermutation), std::invalid_argument);
    EXPECT_THROW(cpu.rotate(rotation), std::invalid_argument);
    EXPECT_THROW(cpu.calcDeterminant(determinant), std::invalid_argument);
}
