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

TEST(MonSuperSystemeTest, FullCleanResetsRamTraceAndPendingOperations) {
    mss::MonSuperSysteme system;
    const int a = system.loadMatrix(mss::Matrix({{1.0, 2.0}, {3.0, 4.0}}));
    const int b = system.loadMatrix(mss::Matrix({{5.0, 6.0}, {7.0, 8.0}}));
    system.loadOperation(mss::OperationName::Add, {a, b});
    system.run();
    ASSERT_GT(system.getRam().size(), 0U);
    ASSERT_FALSE(system.executionTrace().empty());

    system.loadOperation(mss::OperationName::Multiply, {a, b});
    ASSERT_GT(system.pendingOperations(), 0U);
    system.fullClean();

    EXPECT_EQ(system.getRam().size(), 0U);
    EXPECT_EQ(system.occupiedMemory(), 0U);
    EXPECT_TRUE(system.executionTrace().empty());
    EXPECT_EQ(system.pendingOperations(), 0U);
    EXPECT_THROW(system.initialMatrix(), std::logic_error);
    system.run(); // Ne doit pas reexecuter les anciennes operations ni ajouter Look.
    EXPECT_EQ(system.getRam().size(), 0U);
    EXPECT_TRUE(system.executionTrace().empty());

    const int fresh = system.loadMatrix(mss::Matrix({{9.0, 8.0}, {7.0, 6.0}}));
    EXPECT_EQ(fresh, 0);
    EXPECT_DOUBLE_EQ(system.resultAt(fresh).at(0, 0), 9.0);
}

TEST(MonSuperSystemeTest, FullCleanIsIdempotent) {
    mss::MonSuperSysteme system;
    system.fullClean();
    system.fullClean();
    EXPECT_EQ(system.getRam().size(), 0U);
    EXPECT_TRUE(system.executionTrace().empty());
}
