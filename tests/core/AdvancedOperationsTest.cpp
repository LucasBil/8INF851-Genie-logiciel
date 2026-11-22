#include "core/MonSuperSysteme.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include <sstream>
#include <string>

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

TEST(AdvancedOperationsTest, ExplicitCleanEmptiesRamAndPreservesTrace) {
    mss::MonSuperSysteme system;
    const int source = system.loadMatrix(pattern(4));
    const int result = system.loadOperation(mss::OperationName::Rotate, {source}).at(0);
    EXPECT_TRUE(system.loadOperation(mss::OperationName::Clean, {}).empty());
    EXPECT_EQ(system.pendingOperations(), 2U);
    system.run();
    EXPECT_EQ(system.pendingOperations(), 0U);
    EXPECT_EQ(system.occupiedMemory(), 0U);
    EXPECT_EQ(system.getRam().size(), 0U);
    EXPECT_THROW(system.resultAt(source), std::out_of_range);
    EXPECT_THROW(system.resultAt(result), std::out_of_range);
    ASSERT_FALSE(system.executionTrace().empty());
    EXPECT_EQ(system.executionTrace().back(), mss::OperationName::Clean);
}

TEST(AdvancedOperationsTest, CleanBeforeCalculationRunsLastAndEmptiesRam) {
    mss::MonSuperSysteme system;
    const int source = system.loadMatrix(pattern(4));
    system.loadOperation(mss::OperationName::Clean, {});
    system.loadOperation(mss::OperationName::Mirror, {source});
    system.run();
    EXPECT_EQ(system.pendingOperations(), 0U);
    EXPECT_EQ(system.getRam().size(), 0U);
    EXPECT_EQ(system.occupiedMemory(), 0U);
    ASSERT_GE(system.executionTrace().size(), 2U);
    EXPECT_EQ(system.executionTrace().back(), mss::OperationName::Clean);
    EXPECT_NE(std::find(system.executionTrace().begin(), system.executionTrace().end(),
                        mss::OperationName::Mirror), system.executionTrace().end());
}

TEST(AdvancedOperationsTest, MultipleCleanOperationsDoNotLeaveDanglingInstructions) {
    mss::MonSuperSysteme system;
    system.loadMatrix(pattern(2));
    system.loadOperation(mss::OperationName::Clean, {});
    system.loadOperation(mss::OperationName::Clean, {});
    system.run();
    EXPECT_EQ(system.getRam().size(), 0U);
    EXPECT_EQ(system.pendingOperations(), 0U);
    ASSERT_EQ(system.executionTrace().size(), 2U);
    EXPECT_EQ(system.executionTrace().front(), mss::OperationName::Clean);
    EXPECT_EQ(system.executionTrace().back(), mss::OperationName::Clean);
}

TEST(AdvancedOperationsTest, CleanAllowsFreshMatrixAndSecondExecution) {
    mss::MonSuperSysteme system;
    system.loadMatrix(pattern(2));
    system.loadOperation(mss::OperationName::Clean, {});
    system.run();
    system.loadInitialMatrix(mss::Matrix({{1, 2}, {3, 4}}));
    const int out = system.loadOperation(mss::OperationName::Mirror, {0}).at(0);
    system.run();
    EXPECT_DOUBLE_EQ(system.resultAt(out).at(0, 0), 2.0);
    EXPECT_EQ(system.occupiedMemory(), 2U);
}

TEST(AdvancedOperationsTest, CleanRejectsInputAndOutputArguments) {
    mss::MonSuperSysteme system;
    const int source = system.loadMatrix(pattern(2));
    EXPECT_THROW(system.loadOperation(mss::OperationName::Clean, {source}), std::invalid_argument);
    const int reserved = system.getRam().reserve().front();
    EXPECT_THROW(system.loadOperation(mss::OperationName::Clean, {}, {reserved}), std::invalid_argument);
}

TEST(AdvancedOperationsTest, LookPrintsMatrixAndLeavesItUnchanged) {
    mss::MonSuperSysteme system;
    const int source = system.loadMatrix(mss::Matrix({{1, 2}, {3, 4}}));
    EXPECT_TRUE(system.loadOperation(mss::OperationName::Look, {source}).empty());
    testing::internal::CaptureStdout();
    system.run();
    const std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("RAM[0] : Matrice 2x2"), std::string::npos);
    EXPECT_NE(output.find("1 2\n3 4"), std::string::npos);
    EXPECT_DOUBLE_EQ(system.resultAt(source).at(1, 1), 4.0);
    EXPECT_EQ(system.occupiedMemory(), 1U);
    EXPECT_EQ(system.executionTrace().back(), mss::OperationName::Look);
}

TEST(AdvancedOperationsTest, LookCanInspectPendingAndOperationSlots) {
    mss::MonSuperSysteme system;
    const int source = system.loadMatrix(mss::Matrix({{1, 2}, {3, 4}}));
    const int pending = system.getRam().reserve().at(0);
    const int op = system.getRam().load(mss::Operation{mss::OperationName::Add, {static_cast<std::size_t>(source)}, {static_cast<std::size_t>(pending)}});
    std::ostringstream out;
    system.getRam().look(pending, out);
    system.getRam().look(op, out);
    EXPECT_NE(out.str().find("Pending"), std::string::npos);
    EXPECT_NE(out.str().find("Operation"), std::string::npos);
    EXPECT_THROW(system.getRam().look(-1, out), std::out_of_range);
    EXPECT_THROW(system.getRam().look(999, out), std::out_of_range);
}

TEST(AdvancedOperationsTest, LookRejectsOutputAndMissingInputs) {
    mss::MonSuperSysteme system;
    const int address = system.loadMatrix(mss::Matrix({{1, 2}, {3, 4}}));
    EXPECT_THROW(system.loadOperation(mss::OperationName::Look, {}), std::invalid_argument);
    const int reserved = system.getRam().reserve().front();
    EXPECT_THROW(system.loadOperation(mss::OperationName::Look, {address}, {reserved}), std::invalid_argument);
}

TEST(AdvancedOperationsTest, LookWaitsForFinalRecursiveResult) {
    mss::MonSuperSysteme system;
    const int source = system.loadMatrix(pattern(4));
    const int result = system.loadOperation(mss::OperationName::Rotate, {source}).at(0);
    system.loadOperation(mss::OperationName::Look, {result});
    testing::internal::CaptureStdout();
    system.run();
    const std::string printed = testing::internal::GetCapturedStdout();
    EXPECT_NE(printed.find("RAM[" + std::to_string(result) + "] : Matrice 4x4"), std::string::npos);
    EXPECT_EQ(printed.find("Pending"), std::string::npos);
    ASSERT_FALSE(system.executionTrace().empty());
    EXPECT_EQ(system.executionTrace().back(), mss::OperationName::Look);
    EXPECT_DOUBLE_EQ(system.resultAt(result).at(0, 0), pattern(4).at(3, 0));
}

TEST(AdvancedOperationsTest, LookRunsAfterCalculationsEvenWhenQueuedFirst) {
    mss::MonSuperSysteme system;
    const int source = system.loadMatrix(pattern(4));
    const int result = system.getRam().reserve().front();
    system.loadOperation(mss::OperationName::Look, {result});
    system.loadOperation(mss::OperationName::Mirror, {source}, {result});
    testing::internal::CaptureStdout();
    system.run();
    const std::string printed = testing::internal::GetCapturedStdout();
    EXPECT_NE(printed.find("Matrice 4x4"), std::string::npos);
    EXPECT_EQ(printed.find("Pending"), std::string::npos);
    ASSERT_FALSE(system.executionTrace().empty());
    EXPECT_EQ(system.executionTrace().back(), mss::OperationName::Look);
}

TEST(AdvancedOperationsTest, LookRunsBeforeCleanEvenWhenCleanQueuedFirst) {
    mss::MonSuperSysteme system;
    const int source = system.loadMatrix(pattern(4));
    const int result = system.loadOperation(mss::OperationName::Rotate, {source}).at(0);
    system.loadOperation(mss::OperationName::Clean, {});
    system.loadOperation(mss::OperationName::Look, {result});
    testing::internal::CaptureStdout();
    system.run();
    const std::string printed = testing::internal::GetCapturedStdout();
    EXPECT_NE(printed.find("Matrice 4x4"), std::string::npos);
    EXPECT_EQ(system.getRam().size(), 0U);
    ASSERT_GE(system.executionTrace().size(), 2U);
    EXPECT_EQ(system.executionTrace()[system.executionTrace().size()-2], mss::OperationName::Look);
    EXPECT_EQ(system.executionTrace().back(), mss::OperationName::Clean);
}

TEST(AdvancedOperationsTest, RunAutomaticallyLooksAtFinalResult) {
    mss::MonSuperSysteme system;
    const int source = system.loadMatrix(pattern(4));
    const int finalResult = system.loadOperation(mss::OperationName::Rotate, {source}).at(0);
    testing::internal::CaptureStdout();
    system.run();
    const std::string printed = testing::internal::GetCapturedStdout();
    EXPECT_NE(printed.find("RAM[" + std::to_string(finalResult) + "] : Matrice 4x4"), std::string::npos);
    EXPECT_EQ(system.executionTrace().back(), mss::OperationName::Look);
    EXPECT_EQ(system.pendingOperations(), 0U);
}

TEST(AdvancedOperationsTest, AutomaticLookFollowsLastRequestedCalculation) {
    mss::MonSuperSysteme system;
    const int a = system.loadMatrix(pattern(2));
    const int b = system.loadMatrix(pattern(2));
    const int first = system.loadOperation(mss::OperationName::Add, {a, b}).at(0);
    const int last = system.loadOperation(mss::OperationName::Multiply, {a, b}).at(0);
    testing::internal::CaptureStdout();
    system.run();
    const std::string printed = testing::internal::GetCapturedStdout();
    EXPECT_EQ(printed.find("RAM[" + std::to_string(first) + "] :"), std::string::npos);
    EXPECT_NE(printed.find("RAM[" + std::to_string(last) + "] :"), std::string::npos);
    EXPECT_EQ(std::count(system.executionTrace().begin(), system.executionTrace().end(),
                         mss::OperationName::Look), 1);
}

TEST(AdvancedOperationsTest, AutomaticLookRunsBeforeClean) {
    mss::MonSuperSysteme system;
    const int a = system.loadMatrix(pattern(4));
    const int result = system.loadOperation(mss::OperationName::Mirror, {a}).at(0);
    system.loadOperation(mss::OperationName::Clean, {});
    testing::internal::CaptureStdout();
    system.run();
    const std::string printed = testing::internal::GetCapturedStdout();
    EXPECT_NE(printed.find("RAM[" + std::to_string(result) + "] : Matrice 4x4"), std::string::npos);
    ASSERT_GE(system.executionTrace().size(), 2U);
    EXPECT_EQ(system.executionTrace()[system.executionTrace().size() - 2], mss::OperationName::Look);
    EXPECT_EQ(system.executionTrace().back(), mss::OperationName::Clean);
    EXPECT_EQ(system.getRam().size(), 0U);
}

TEST(AdvancedOperationsTest, AutomaticLookDoesNotDuplicateExplicitLook) {
    mss::MonSuperSysteme system;
    const int a = system.loadMatrix(pattern(2));
    const int result = system.loadOperation(mss::OperationName::Rotate, {a}).at(0);
    system.loadOperation(mss::OperationName::Look, {result});
    testing::internal::CaptureStdout();
    system.run();
    testing::internal::GetCapturedStdout();
    EXPECT_EQ(std::count(system.executionTrace().begin(), system.executionTrace().end(),
                         mss::OperationName::Look), 1);
}

TEST(AdvancedOperationsTest, AutomaticLookDisplaysAllSplitOutputs) {
    mss::MonSuperSysteme system;
    const int a = system.loadMatrix(pattern(2));
    const auto results = system.loadOperation(mss::OperationName::Split, {a});
    testing::internal::CaptureStdout();
    system.run();
    const std::string printed = testing::internal::GetCapturedStdout();
    for (int result : results)
        EXPECT_NE(printed.find("RAM[" + std::to_string(result) + "] : Matrice 1x1"), std::string::npos);
    EXPECT_EQ(std::count(system.executionTrace().begin(), system.executionTrace().end(),
                         mss::OperationName::Look), 4);
}
