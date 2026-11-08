#include "io/UserOutputController.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <chrono>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

TEST(UserOutputControllerTest, PrintsTraceAndFindsTheLowestCostConfiguration) {
    mss::Configuration firstConfiguration;
    firstConfiguration.set(mss::OperationName::Rotate, 5);
    firstConfiguration.set(mss::OperationName::Add, 10);
    mss::Configuration secondConfiguration;
    secondConfiguration.set(mss::OperationName::Rotate, 2);
    secondConfiguration.set(mss::OperationName::Add, 3);

    std::istringstream input("n");
    std::ostringstream output;
    mss::UserOutputController controller({firstConfiguration, secondConfiguration}, input, output);
    controller.printTrace({mss::OperationName::Rotate, mss::OperationName::Add});

    EXPECT_NE(output.str().find("1. Rotate"), std::string::npos);
    EXPECT_NE(output.str().find("Configuration 1 : 15"), std::string::npos);
    EXPECT_NE(output.str().find("Configuration 2 : 5"), std::string::npos);
    EXPECT_NE(output.str().find("Meilleure configuration : 2 (cout : 5)"), std::string::npos);
}

TEST(UserOutputControllerTest, SavesTraceWhenTheUserAccepts) {
    mss::Configuration configuration;
    configuration.set(mss::OperationName::Rotate, 1);
    const auto uniqueId = std::chrono::steady_clock::now().time_since_epoch().count();
    const std::filesystem::path filePath = std::filesystem::temp_directory_path() /
        ("mss_user_output_controller_trace_" + std::to_string(uniqueId) + ".txt");

    std::istringstream input("oui " + filePath.string());
    std::ostringstream output;
    mss::UserOutputController controller({configuration}, input, output);
    controller.printTrace({mss::OperationName::Rotate});

    std::ifstream savedFile(filePath);
    const std::string contents((std::istreambuf_iterator<char>(savedFile)),
                               std::istreambuf_iterator<char>());
    savedFile.close();
    std::filesystem::remove(filePath);

    EXPECT_NE(contents.find("1. Rotate"), std::string::npos);
    EXPECT_NE(output.str().find("Trace sauvegardee."), std::string::npos);
}

TEST(UserOutputControllerTest, RejectsPrintingWithoutAnActiveConfiguration) {
    std::istringstream input("n");
    std::ostringstream output;
    mss::UserOutputController emptyController({}, input, output);
    EXPECT_THROW(emptyController.printTrace({}, false), std::logic_error);

    mss::Configuration configuration;
    mss::UserOutputController controller({configuration}, input, output);
    EXPECT_THROW(controller.printTrace({mss::OperationName::Rotate}), std::out_of_range);
}

TEST(UserOutputControllerTest, RejectsInvalidSaveChoiceAndUnavailableInput) {
    mss::Configuration configuration;
    configuration.set(mss::OperationName::Rotate, 1);

    std::istringstream invalidChoice("maybe n");
    std::ostringstream firstOutput;
    mss::UserOutputController firstController({configuration}, invalidChoice, firstOutput);
    EXPECT_NO_THROW(firstController.printTrace({mss::OperationName::Rotate}));
    EXPECT_NE(firstOutput.str().find("Repondez par oui ou non."), std::string::npos);

    std::istringstream unavailableInput;
    std::ostringstream secondOutput;
    mss::UserOutputController secondController({configuration}, unavailableInput, secondOutput);
    EXPECT_THROW(secondController.printTrace({mss::OperationName::Rotate}), std::runtime_error);
}

TEST(UserOutputControllerTest, RejectsInvalidOperationsAndFailedTraceFiles) {
    mss::Configuration configuration;
    configuration.set(static_cast<mss::OperationName>(99), 1);
    std::istringstream input;
    std::ostringstream output;
    mss::UserOutputController controller({configuration}, input, output);
    EXPECT_THROW(controller.printTrace({static_cast<mss::OperationName>(99)}, false),
                 std::logic_error);

    const auto uniqueId = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto missingPath = std::filesystem::temp_directory_path() /
        ("mss_missing_directory_" + std::to_string(uniqueId)) / "trace.txt";
    std::istringstream saveInput(missingPath.string());
    std::ostringstream saveOutput;
    mss::UserOutputController saveController({configuration}, saveInput, saveOutput);
    EXPECT_THROW(saveController.outputTrace({mss::OperationName::Rotate}), std::runtime_error);
}
