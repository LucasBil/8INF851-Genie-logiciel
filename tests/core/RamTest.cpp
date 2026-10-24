#include "core/Ram.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

TEST(RamTest, StoresMatricesAtSequentialAddresses) {
    mss::Ram ram;

    EXPECT_EQ(ram.store(mss::Matrix({{1.0}})), 0);
    EXPECT_EQ(ram.store(mss::Matrix({{2.0}})), 1);
    EXPECT_EQ(ram.size(), 2);
    EXPECT_DOUBLE_EQ(ram.at(1).at(0, 0), 2.0);
}

TEST(RamTest, RejectsInvalidAddressesAndUnspecifiedCosts) {
    const mss::Ram ram;
    mss::Operation operation{mss::OperationName::Add, {}};

    EXPECT_THROW(ram.at(0), std::out_of_range);
    EXPECT_THROW(ram.getOperationCost(operation), std::logic_error);
}
