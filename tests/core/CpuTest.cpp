#include "core/Cpu.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

TEST(CpuTest, ReportsThatOperationCostsAreNotSpecified) {
    const mss::Cpu cpu;
    mss::Operation operation{mss::OperationName::Add, {}};

    EXPECT_THROW(cpu.getOperationCost(operation), std::logic_error);
}
