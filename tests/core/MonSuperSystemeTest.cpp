#include "core/MonSuperSysteme.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

TEST(MonSuperSystemeTest, StoresTheInitialMatrixAtAddressZero) {
    mss::MonSuperSysteme system;
    system.loadInitialMatrix(mss::Matrix({{5.0}}));

    EXPECT_DOUBLE_EQ(system.initialMatrix().at(0, 0), 5.0);
}

TEST(MonSuperSystemeTest, RejectsMissingOrDuplicateInitialMatrix) {
    mss::MonSuperSysteme system;

    EXPECT_THROW(system.initialMatrix(), std::logic_error);
    system.loadInitialMatrix(mss::Matrix({{5.0}}));
    EXPECT_THROW(system.loadInitialMatrix(mss::Matrix({{6.0}})), std::logic_error);
}

TEST(MonSuperSystemeTest, ExposesItsCpuWithoutInventingOperationCosts) {
    mss::MonSuperSysteme system;
    mss::Operation operation{mss::OperationName::Add, {}};

    EXPECT_THROW(system.cpu().getOperationCost(operation), std::logic_error);
}
