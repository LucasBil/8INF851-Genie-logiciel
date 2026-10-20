#include "core/Cpu.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

TEST(CpuTest, ReportsThatOperationCostsAreNotSpecified) {
    const mss::Cpu cpu;

    EXPECT_THROW(cpu.getOperationCost(mss::OperationName::Add), std::logic_error);
}
