#include "core/MonSuperSysteme.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

namespace {
mss::Matrix makeMatrix(int n, int seed) {
    std::vector<std::vector<double>> rows(static_cast<std::size_t>(n),
                                          std::vector<double>(static_cast<std::size_t>(n)));
    for (int r = 0; r < n; ++r)
        for (int c = 0; c < n; ++c)
            rows[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)] =
                static_cast<double>((r * 7 + c * 11 + seed) % 10);
    return mss::Matrix(std::move(rows));
}
void verify(int n, mss::OperationName op) {
    mss::MonSuperSysteme system;
    const mss::Matrix a = makeMatrix(n, 1);
    const mss::Matrix b = makeMatrix(n, 3);
    const int addrA = system.loadMatrix(a), addrB = system.loadMatrix(b);
    const int out = system.loadOperation(op, {addrA, addrB}).at(0);
    // Dependance : l'impression doit attendre le resultat.
    system.loadOperation(mss::OperationName::Print, {out});
    system.run();
    EXPECT_EQ(system.pendingOperations(), 0U);
    EXPECT_GT(system.executionTrace().size(), 0U);
    const auto &got = system.resultAt(out);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double expected = a.at(static_cast<std::size_t>(i), static_cast<std::size_t>(j)) +
                              b.at(static_cast<std::size_t>(i), static_cast<std::size_t>(j));
            if (op == mss::OperationName::Multiply) {
                expected = 0;
                for (int k = 0; k < n; ++k)
                    expected += a.at(static_cast<std::size_t>(i), static_cast<std::size_t>(k)) *
                                b.at(static_cast<std::size_t>(k), static_cast<std::size_t>(j));
            }
            EXPECT_DOUBLE_EQ(got.at(static_cast<std::size_t>(i), static_cast<std::size_t>(j)), expected);
        }
    }
}
}
TEST(RecursiveOperationsTest, Adds2x2) { verify(2, mss::OperationName::Add); }
TEST(RecursiveOperationsTest, Adds4x4) { verify(4, mss::OperationName::Add); }
TEST(RecursiveOperationsTest, Adds8x8) { verify(8, mss::OperationName::Add); }
TEST(RecursiveOperationsTest, Adds16x16) { verify(16, mss::OperationName::Add); }
TEST(RecursiveOperationsTest, Multiplies2x2) { verify(2, mss::OperationName::Multiply); }
TEST(RecursiveOperationsTest, Multiplies4x4) { verify(4, mss::OperationName::Multiply); }
TEST(RecursiveOperationsTest, Multiplies8x8) { verify(8, mss::OperationName::Multiply); }
TEST(RecursiveOperationsTest, Multiplies16x16) { verify(16, mss::OperationName::Multiply); }
TEST(RecursiveOperationsTest, ReservedOutputsAreNotReadableUntilComputed) {
    mss::MonSuperSysteme system;
    auto a = system.loadMatrix(mss::Matrix({{1,2},{3,4}}));
    auto b = system.loadMatrix(mss::Matrix({{5,6},{7,8}}));
    auto output = system.loadOperation(mss::OperationName::Add, {a,b}).at(0);
    EXPECT_THROW(system.resultAt(output), std::invalid_argument);
    system.run();
    EXPECT_DOUBLE_EQ(system.resultAt(output).at(0,0),6);
}
