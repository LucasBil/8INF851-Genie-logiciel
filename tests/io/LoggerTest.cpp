#include "logging/Logger.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

TEST(LoggerTest, CreatesLogDirectoryAndAppendsTimestampedEntries) {
    const auto uniqueId = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto directory = std::filesystem::temp_directory_path() /
                           ("mss_logger_test_" + std::to_string(uniqueId));
    const auto filePath = directory / "nested" / "test.log";
    mss::Logger logger(filePath);

    ASSERT_TRUE(logger.info("configuration saved"));
    ASSERT_TRUE(logger.error("configuration load failed"));

    std::ifstream file(filePath);
    const std::string contents((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
    file.close();
    EXPECT_NE(contents.find("[INFO] configuration saved"), std::string::npos);
    EXPECT_NE(contents.find("[ERROR] configuration load failed"), std::string::npos);

    std::filesystem::remove_all(directory);
}
