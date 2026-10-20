#include "core/Operation.hpp"

#include <gtest/gtest.h>

TEST(OperationTest, HoldsItsNameAndInputAddresses) {
    const mss::Operation operation{mss::OperationName::Add, {0, 2}};

    EXPECT_EQ(operation.type, mss::OperationName::Add);
    ASSERT_EQ(operation.inputLocations.size(), 2);
    EXPECT_EQ(operation.inputLocations[1], 2);
}
