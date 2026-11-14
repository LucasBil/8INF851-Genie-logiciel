#include "core/MonSuperSysteme.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <vector>

namespace {
mss::Matrix pattern(int n) {
    std::vector<std::vector<double>> data(static_cast<std::size_t>(n),
                                          std::vector<double>(static_cast<std::size_t>(n)));
    for (int r = 0; r < n; ++r)
        for (int c = 0; c < n; ++c)
            data[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)] = r * 17 + c + 1;
    return mss::Matrix(std::move(data));
}
void checkTransform(int n, mss::OperationName op) {
    mss::MonSuperSysteme system;
    const auto original = pattern(n);
    const int input = system.loadMatrix(original);
    const int output = system.loadOperation(op, {input}).at(0);
    system.run();
    const auto &result = system.resultAt(output);
    for (int r = 0; r < n; ++r)
        for (int c = 0; c < n; ++c) {
            const double expected = op == mss::OperationName::Rotate
                ? original.at(static_cast<std::size_t>(n - c - 1), static_cast<std::size_t>(r))
                : original.at(static_cast<std::size_t>(r), static_cast<std::size_t>(n - c - 1));
            EXPECT_DOUBLE_EQ(result.at(static_cast<std::size_t>(r), static_cast<std::size_t>(c)), expected);
        }
    EXPECT_EQ(system.occupiedMemory(), 2U);  // source + resultat uniquement
    EXPECT_EQ(system.pendingOperations(), 0U);
    EXPECT_DOUBLE_EQ(system.resultAt(input).at(0, 0), original.at(0, 0));
}
void checkDeterminant(int n, double expected) {
    mss::MonSuperSysteme system;
    std::vector<std::vector<double>> data(static_cast<std::size_t>(n),
                                          std::vector<double>(static_cast<std::size_t>(n), 0));
    for (int i = 0; i < n; ++i) data[static_cast<std::size_t>(i)][static_cast<std::size_t>(i)] = i + 2;
    const int input = system.loadMatrix(mss::Matrix(data));
    const int output = system.loadOperation(mss::OperationName::CalcDeterminant, {input}).at(0);
    system.run();
    EXPECT_EQ(system.resultAt(output).rows(), 1U);
    EXPECT_DOUBLE_EQ(system.resultAt(output).at(0, 0), expected);
    EXPECT_EQ(system.occupiedMemory(), 2U);
    EXPECT_EQ(system.pendingOperations(), 0U);
}
} // namespace

TEST(AdvancedOperationsTest, Rotate2x2) { checkTransform(2, mss::OperationName::Rotate); }
TEST(AdvancedOperationsTest, Rotate4x4) { checkTransform(4, mss::OperationName::Rotate); }
TEST(AdvancedOperationsTest, Rotate8x8) { checkTransform(8, mss::OperationName::Rotate); }
TEST(AdvancedOperationsTest, Mirror2x2) { checkTransform(2, mss::OperationName::Mirror); }
TEST(AdvancedOperationsTest, Mirror4x4) { checkTransform(4, mss::OperationName::Mirror); }
TEST(AdvancedOperationsTest, Mirror8x8) { checkTransform(8, mss::OperationName::Mirror); }
TEST(AdvancedOperationsTest, Determinant2x2) { checkDeterminant(2, 6); }
TEST(AdvancedOperationsTest, Determinant4x4) { checkDeterminant(4, 120); }
TEST(AdvancedOperationsTest, Determinant8x8) { checkDeterminant(8, 362880); }
TEST(AdvancedOperationsTest, DeterminantOffDiagonal4x4) {
    mss::MonSuperSysteme system;
    // Permutation des lignes 1 et 2 d'une matrice triangulaire => signe negatif.
    const int input = system.loadMatrix(mss::Matrix({{0, 3, 0, 0}, {2, 0, 0, 0},
                                                     {0, 0, 4, 0}, {0, 0, 0, 5}}));
    const int output = system.loadOperation(mss::OperationName::CalcDeterminant, {input}).at(0);
    system.run();
    EXPECT_DOUBLE_EQ(system.resultAt(output).at(0, 0), -120);
    EXPECT_EQ(system.occupiedMemory(), 2U);
}
TEST(AdvancedOperationsTest, ReusesFreedMemoryAndClearsExplicitly) {
    mss::MonSuperSysteme system;
    const int source = system.loadMatrix(pattern(4));
    const int first = system.loadOperation(mss::OperationName::Rotate, {source}).at(0);
    system.run();
    EXPECT_EQ(system.occupiedMemory(), 2U);
    const auto before = system.getRam().size();
    const int second = system.loadOperation(mss::OperationName::Mirror, {first}).at(0);
    system.run();
    EXPECT_EQ(system.occupiedMemory(), 3U);
    EXPECT_LE(system.getRam().size(), before + 1); // Peut reutiliser les trous.
    EXPECT_EQ(system.resultAt(second).rows(), 4U);
    system.clear();
    EXPECT_EQ(system.getRam().size(), 0U);
    EXPECT_EQ(system.occupiedMemory(), 0U);
    EXPECT_TRUE(system.executionTrace().empty());
}
TEST(AdvancedOperationsTest, PendingResultsCannotBeAccessed) {
    mss::MonSuperSysteme system;
    const int input = system.loadMatrix(pattern(4));
    const int result = system.loadOperation(mss::OperationName::Rotate, {input}).at(0);
    EXPECT_THROW(system.resultAt(result), std::invalid_argument);
    system.run();
    EXPECT_NO_THROW(system.resultAt(result));
}

TEST(AdvancedOperationsTest, ExplicitCleanIsQueuedTracedAndPreservesPublicResults) {
    mss::MonSuperSysteme system;
    const int source = system.loadMatrix(pattern(4));
    const int result = system.loadOperation(mss::OperationName::Rotate, {source}).at(0);
    const auto cleaned = system.loadOperation(mss::OperationName::Clean, {});
    EXPECT_TRUE(cleaned.empty());
    EXPECT_EQ(system.pendingOperations(), 2U);
    system.run();
    EXPECT_EQ(system.pendingOperations(), 0U);
    EXPECT_DOUBLE_EQ(system.resultAt(result).at(0, 0), 52.0);
    EXPECT_DOUBLE_EQ(system.resultAt(source).at(0, 0), 1.0);
    EXPECT_EQ(system.occupiedMemory(), 2U);
    ASSERT_FALSE(system.executionTrace().empty());
    EXPECT_EQ(system.executionTrace().back(), mss::OperationName::Clean);
}

TEST(AdvancedOperationsTest, CleanBeforeCalculationDoesNotDeletePendingOutputs) {
    mss::MonSuperSysteme system;
    const int source = system.loadMatrix(pattern(4));
    system.loadOperation(mss::OperationName::Clean, {});
    const int result = system.loadOperation(mss::OperationName::Mirror, {source}).at(0);
    system.run();
    EXPECT_DOUBLE_EQ(system.resultAt(result).at(0, 0), 4.0);
    EXPECT_EQ(system.occupiedMemory(), 2U);
    EXPECT_EQ(system.executionTrace().back(), mss::OperationName::Clean);
}

TEST(AdvancedOperationsTest, CleanRejectsInputAndOutputArguments) {
    mss::MonSuperSysteme system;
    const int source = system.loadMatrix(pattern(2));
    EXPECT_THROW(system.loadOperation(mss::OperationName::Clean, {source}), std::invalid_argument);
    const int reserved = system.getRam().reserve().front();
    EXPECT_THROW(system.loadOperation(mss::OperationName::Clean, {}, {reserved}), std::invalid_argument);
}
