#include "serializer/ConfigurationSerializer.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

namespace {

std::filesystem::path temporaryConfigurationPath(const std::string &contents) {
    const auto uniqueId = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto path = std::filesystem::temp_directory_path() /
        ("mss_configuration_serializer_" + std::to_string(uniqueId) + ".txt");
    std::ofstream file(path);
    file << contents;
    return path;
}

} // namespace

TEST(ConfigurationSerializerTest, RequiresAnOpenFileForLoadAndSave) {
    mss::ConfigurationSerializer serializer;
    EXPECT_THROW(serializer.load(), std::logic_error);
    EXPECT_THROW(serializer.save(mss::Configuration{}), std::logic_error);
}

TEST(ConfigurationSerializerTest, RejectsOpeningAnotherFileBeforeClosing) {
    const auto path = temporaryConfigurationPath("Rotate 4\n");
    mss::ConfigurationSerializer serializer;
    serializer.open(path.string());

    EXPECT_THROW(serializer.open(path.string()), std::logic_error);
    serializer.close();
    serializer.close();
    std::filesystem::remove(path);
}

TEST(ConfigurationSerializerTest, RejectsMalformedDuplicateUnknownAndNonpositiveEntries) {
    const std::string invalidContents[] = {
        "Rotate nope\n",
        "UnknownOperation 2\n",
        "Rotate 2\nRotate 3\n",
        "Rotate 0\n",
        "Rotate -1\n",
        "Rotate 2 extra\n",
    };

    for (const std::string &contents : invalidContents) {
        const auto path = temporaryConfigurationPath(contents);
        mss::ConfigurationSerializer serializer;
        serializer.open(path.string());
        EXPECT_THROW(serializer.load(), std::runtime_error) << contents;
        serializer.close();
        std::filesystem::remove(path);
    }
}

TEST(ConfigurationSerializerTest, RejectsSavingNonpositiveCosts) {
    const auto path = std::filesystem::temp_directory_path() /
        ("mss_configuration_serializer_save_" +
         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".txt");
    mss::Configuration configuration;
    configuration.set(mss::OperationName::Rotate, 0);

    mss::ConfigurationSerializer serializer;
    serializer.open(path.string(), std::ios::out);
    EXPECT_THROW(serializer.save(configuration), std::invalid_argument);
    serializer.close();
    std::filesystem::remove(path);
}

TEST(ConfigurationSerializerTest, RejectsOpeningAMissingFile) {
    const auto path = std::filesystem::temp_directory_path() /
        ("mss_missing_configuration_" +
         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".txt");
    mss::ConfigurationSerializer serializer;

    EXPECT_THROW(serializer.open(path.string()), std::runtime_error);
}
