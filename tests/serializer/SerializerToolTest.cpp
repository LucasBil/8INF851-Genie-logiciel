#include "serializer/SerializerTool.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>

TEST(SerializerToolTest, SavesAndLoadsConfigurationsByAvailableFileIndex) {
    const auto uniqueId = std::chrono::steady_clock::now().time_since_epoch().count();
    const std::string name = "serializer_test_" + std::to_string(uniqueId);
    const std::string fileName = name + ".txt";

    mss::Configuration configuration;
    configuration.set(mss::OperationName::Add, 13);
    configuration.set(mss::OperationName::Rotate, 4);

    mss::SerializerTool serializer;
    serializer.saveConfiguration(configuration, name);

    const std::filesystem::path savedPath =
        std::filesystem::path("configurations") / (name + ".txt");
    std::ifstream savedFile(savedPath);
    const std::string text((std::istreambuf_iterator<char>(savedFile)),
                           std::istreambuf_iterator<char>());
    EXPECT_NE(text.find("Rotate 4"), std::string::npos);
    EXPECT_NE(text.find("Add 13"), std::string::npos);
    savedFile.close();

    std::istringstream listing(serializer.getConfigurationDirectoryContent());
    int configurationIndex = -1;
    char separator = '\0';
    std::string listedName;
    while (listing >> configurationIndex >> separator >> listedName) {
        if (separator == '.' && listedName == fileName) {
            const mss::Configuration loaded = serializer.loadConfiguration(configurationIndex);
            EXPECT_EQ(loaded.get(mss::OperationName::Add), 13);
            EXPECT_EQ(loaded.get(mss::OperationName::Rotate), 4);
            std::filesystem::remove(savedPath);
            return;
        }
    }

    FAIL() << "Saved configuration was not listed by FileExplorer";
}

TEST(SerializerToolTest, RejectsInvalidTextConfiguration) {
    const auto uniqueId = std::chrono::steady_clock::now().time_since_epoch().count();
    const std::filesystem::path path = std::filesystem::temp_directory_path() /
        ("mss_invalid_configuration_" + std::to_string(uniqueId) + ".txt");
    {
        std::ofstream invalidFile(path);
        invalidFile << "Rotate nope\n";
    }

    mss::ConfigurationSerializer serializer;
    serializer.open(path.string());
    EXPECT_THROW(serializer.load(), std::runtime_error);
    serializer.close();
    std::filesystem::remove(path);
}

TEST(SerializerToolTest, OverwritesAnExistingConfiguration) {
    const auto uniqueId = std::chrono::steady_clock::now().time_since_epoch().count();
    const std::string name = "serializer_overwrite_test_" + std::to_string(uniqueId);
    const std::filesystem::path path =
        std::filesystem::path("configurations") / (name + ".txt");

    mss::SerializerTool serializer;
    mss::Configuration first;
    first.set(mss::OperationName::Rotate, 3);
    serializer.saveConfiguration(first, name);

    mss::Configuration replacement;
    replacement.set(mss::OperationName::Rotate, 7);
    serializer.saveConfiguration(replacement, name);

    std::ifstream savedFile(path);
    const std::string text((std::istreambuf_iterator<char>(savedFile)),
                           std::istreambuf_iterator<char>());
    savedFile.close();
    EXPECT_EQ(text, "Rotate 7\n");
    std::filesystem::remove(path);
}

TEST(SerializerToolTest, UsesThePathListFromTheMostRecentDirectoryListing) {
    const auto uniqueId = std::chrono::steady_clock::now().time_since_epoch().count();
    const std::string listedName = "serializer_listed_" + std::to_string(uniqueId);
    const std::string laterName = "serializer_added_" + std::to_string(uniqueId);

    mss::Configuration listedConfiguration;
    listedConfiguration.set(mss::OperationName::Rotate, 3);
    mss::Configuration laterConfiguration;
    laterConfiguration.set(mss::OperationName::Rotate, 7);

    mss::SerializerTool serializer;
    serializer.saveConfiguration(listedConfiguration, listedName);
    const std::string listing = serializer.getConfigurationDirectoryContent();
    serializer.saveConfiguration(laterConfiguration, laterName);

    int listedIndex = -1;
    std::istringstream lines(listing);
    int index = -1;
    char separator = '\0';
    std::string fileName;
    while (lines >> index >> separator >> fileName) {
        if (separator == '.' && fileName == listedName + ".txt") {
            listedIndex = index;
            break;
        }
    }

    ASSERT_GE(listedIndex, 0);
    EXPECT_EQ(serializer.loadConfiguration(listedIndex).get(mss::OperationName::Rotate), 3);
    std::filesystem::remove(std::filesystem::path("configurations") /
                            (listedName + ".txt"));
    std::filesystem::remove(std::filesystem::path("configurations") /
                            (laterName + ".txt"));
}
