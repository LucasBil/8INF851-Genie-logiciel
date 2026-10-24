#include "core/Configuration.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

TEST(ConfigurationTest, StoresOneCostPerOperation) {
    mss::Configuration configuration;

    EXPECT_EQ(configuration.set(mss::OperationName::Rotate, 5), 5);
    EXPECT_EQ(configuration.get(mss::OperationName::Rotate), 5);

    EXPECT_EQ(configuration.set(mss::OperationName::Rotate, 8), 8);
    EXPECT_EQ(configuration.get(mss::OperationName::Rotate), 8);
}

TEST(ConfigurationTest, StoresCostsForDifferentOperationsIndependently) {
    mss::Configuration configuration;

    configuration.set(mss::OperationName::Rotate, 5);
    configuration.set(mss::OperationName::Add, 12);

    EXPECT_EQ(configuration.get(mss::OperationName::Rotate), 5);
    EXPECT_EQ(configuration.get(mss::OperationName::Add), 12);
}

TEST(ConfigurationTest, RejectsGettingAnOperationWithoutConfiguredCost) {
    const mss::Configuration configuration;

    EXPECT_THROW(configuration.get(mss::OperationName::Rotate), std::out_of_range);
}
