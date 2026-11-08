#include "io/UserInputController.hpp"

#include "core/MonSuperSysteme.hpp"
#include "io/UserOutputController.hpp"
#include "serializer/SerializerTool.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <chrono>
#include <sstream>
#include <string>

TEST(UserInputControllerTest, LoadsInputAndReportsWhenTheCoreHasNoOperation) {
    mss::SerializerTool serializer;
    mss::Configuration configuration;
    configuration.set(mss::OperationName::Rotate, 4);
    const auto uniqueId = std::chrono::steady_clock::now().time_since_epoch().count();
    const std::string name = "user_input_test_" + std::to_string(uniqueId);
    serializer.saveConfiguration(configuration, name);

    std::istringstream listing(serializer.getConfigurationDirectoryContent());
    int configurationIndex = -1;
    char separator = '\0';
    std::string listedName;
    while (listing >> configurationIndex >> separator >> listedName) {
        if (separator == '.' && listedName == name + ".txt") {
            break;
        }
    }

    std::istringstream input("1 " + std::to_string(configurationIndex) +
                             " q 3 2 2 1 2 3 4 4");
    std::ostringstream terminal;
    mss::UserOutputController output({}, input, terminal);
    mss::MonSuperSysteme system;
    mss::UserInputController controller(serializer, output, system, input, terminal);

    controller.run();
    std::filesystem::remove(std::filesystem::path("configurations") / (name + ".txt"));

    EXPECT_DOUBLE_EQ(system.initialMatrix().at(0, 0), 1.0);
    EXPECT_NE(terminal.str().find("Aucune operation n'a ete executee par MSS"),
              std::string::npos);
}

TEST(UserInputControllerTest, RejectsInvalidMatrixDimensionsAndValues) {
    mss::SerializerTool serializer;
    mss::Configuration configuration;
    configuration.set(mss::OperationName::Rotate, 4);
    std::istringstream input(
        "2 3 1 1 1 1 1 1 1 1 1 1 1 active_config "
        "3 2 3 2 2 1 2 invalid 3 4 4");
    std::ostringstream terminal;
    mss::UserOutputController output({}, input, terminal);
    mss::MonSuperSysteme system;
    mss::UserInputController controller(serializer, output, system, input, terminal);

    controller.run();

    EXPECT_DOUBLE_EQ(system.initialMatrix().at(1, 1), 4.0);
    EXPECT_NE(terminal.str().find("Les matrices doivent etre carrees."), std::string::npos);
    EXPECT_NE(terminal.str().find("Entree invalide : un entier est requis."), std::string::npos);
}

TEST(UserInputControllerTest, RepeatsTheConfigurationListAfterAnInvalidIndex) {
    mss::SerializerTool serializer;
    mss::Configuration configuration;
    configuration.set(mss::OperationName::Rotate, 4);
    const auto uniqueId = std::chrono::steady_clock::now().time_since_epoch().count();
    const std::string name = "user_input_retry_test_" + std::to_string(uniqueId);
    serializer.saveConfiguration(configuration, name);

    std::istringstream listing(serializer.getConfigurationDirectoryContent());
    int configurationIndex = -1;
    char separator = '\0';
    std::string listedName;
    while (listing >> configurationIndex >> separator >> listedName) {
        if (separator == '.' && listedName == name + ".txt") {
            break;
        }
    }

    std::istringstream input("1 999 " + std::to_string(configurationIndex) + " q 4");
    std::ostringstream terminal;
    mss::UserOutputController output({}, input, terminal);
    mss::MonSuperSysteme system;
    mss::UserInputController controller(serializer, output, system, input, terminal);
    controller.run();

    std::filesystem::remove(std::filesystem::path("configurations") / (name + ".txt"));
    EXPECT_NE(terminal.str().find("Aucune configuration ne correspond a cet index."),
              std::string::npos);
    EXPECT_NE(terminal.str().find("Configuration chargee."), std::string::npos);
}

TEST(UserInputControllerTest, LoadsMultipleConfigurationsUntilTheUserExitsSelection) {
    mss::SerializerTool serializer;
    mss::Configuration first;
    first.set(mss::OperationName::Rotate, 4);
    mss::Configuration second;
    second.set(mss::OperationName::Rotate, 6);
    const auto uniqueId = std::chrono::steady_clock::now().time_since_epoch().count();
    const std::string firstName = "user_input_multi_a_" + std::to_string(uniqueId);
    const std::string secondName = "user_input_multi_b_" + std::to_string(uniqueId);
    serializer.saveConfiguration(first, firstName);
    serializer.saveConfiguration(second, secondName);

    std::istringstream listing(serializer.getConfigurationDirectoryContent());
    int firstIndex = -1;
    int secondIndex = -1;
    int index = -1;
    char separator = '\0';
    std::string fileName;
    while (listing >> index >> separator >> fileName) {
        if (separator == '.' && fileName == firstName + ".txt") {
            firstIndex = index;
        } else if (separator == '.' && fileName == secondName + ".txt") {
            secondIndex = index;
        }
    }

    ASSERT_GE(firstIndex, 0);
    ASSERT_GE(secondIndex, 0);
    std::istringstream input("1 " + std::to_string(firstIndex) + " " +
                             std::to_string(secondIndex) + " q 4");
    std::ostringstream terminal;
    mss::UserOutputController output({}, input, terminal);
    mss::MonSuperSysteme system;
    mss::UserInputController controller(serializer, output, system, input, terminal);
    controller.run();

    std::filesystem::remove(std::filesystem::path("configurations") /
                            (firstName + ".txt"));
    std::filesystem::remove(std::filesystem::path("configurations") /
                            (secondName + ".txt"));
    EXPECT_NE(terminal.str().find(firstName + ".txt"), std::string::npos);
    EXPECT_NE(terminal.str().find(secondName + ".txt"), std::string::npos);
}

TEST(UserInputControllerTest, DoesNotStartWithoutAnActiveConfiguration) {
    mss::SerializerTool serializer;
    std::istringstream input("3 4");
    std::ostringstream terminal;
    mss::UserOutputController output({}, input, terminal);
    mss::MonSuperSysteme system;
    mss::UserInputController controller(serializer, output, system, input, terminal);

    controller.run();

    EXPECT_THROW(system.initialMatrix(), std::logic_error);
    EXPECT_NE(terminal.str().find("Chargez ou creez une configuration avant l'execution."),
              std::string::npos);
}
