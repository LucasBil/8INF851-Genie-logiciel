#include "io/DemoInputController.hpp"

#include "core/MonSuperSysteme.hpp"
#include "io/UserOutputController.hpp"
#include "serializer/SerializerTool.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

TEST(DemoInputControllerTest, GeneratesAndSavesTheDemoExecutionTrace) {
    mss::SerializerTool serializer;
    mss::Configuration configuration;
    configuration.set(mss::OperationName::Add, 1);
    configuration.set(mss::OperationName::Multiply, 2);
    configuration.set(mss::OperationName::Split, 3);
    configuration.set(mss::OperationName::Fuse, 4);
    std::istringstream input;
    std::ostringstream terminal;
    mss::UserOutputController output({configuration}, input, terminal);
    mss::MonSuperSysteme system;
    mss::DemoInputController controller(serializer, output, system);

    ASSERT_NO_THROW(controller.run());

    ASSERT_GT(system.executionTrace().size(), 4U);
    EXPECT_EQ(system.executionTrace()[0], mss::OperationName::Add);
    EXPECT_EQ(system.executionTrace()[1], mss::OperationName::Multiply);
    EXPECT_EQ(system.executionTrace()[2], mss::OperationName::Split);
    EXPECT_EQ(system.executionTrace()[3], mss::OperationName::Fuse);
    EXPECT_EQ(system.executionTrace()[4], mss::OperationName::Split);
    EXPECT_DOUBLE_EQ(system.initialMatrix().at(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(system.initialMatrix().at(1, 1), 4.0);
    EXPECT_NE(terminal.str().find("Trace d'execution :"), std::string::npos);
    EXPECT_TRUE(input.good());

    const std::string outputText = terminal.str();
    const std::string pathMarker = "Fichier : ";
    std::vector<std::filesystem::path> tracePaths;
    std::size_t pathMarkerPosition = 0;
    while ((pathMarkerPosition = outputText.find(pathMarker, pathMarkerPosition)) !=
           std::string::npos) {
        const std::size_t pathStart = pathMarkerPosition + pathMarker.size();
        const std::size_t pathEnd = outputText.find('\n', pathStart);
        tracePaths.emplace_back(outputText.substr(pathStart, pathEnd - pathStart));
        pathMarkerPosition = pathEnd;
    }
    ASSERT_EQ(tracePaths.size(), 2U);
    EXPECT_EQ(tracePaths[0].parent_path(), std::filesystem::path("trace"));
    EXPECT_NE(tracePaths[0].filename().string().find("demo_input_2x2_"), std::string::npos);
    EXPECT_EQ(tracePaths[1].parent_path(), std::filesystem::path("trace"));
    EXPECT_NE(tracePaths[1].filename().string().find("demo_input_4x4_"), std::string::npos);

    std::ifstream smallTrace(tracePaths[0]);
    const std::string smallContents((std::istreambuf_iterator<char>(smallTrace)),
                                    std::istreambuf_iterator<char>());
    std::ifstream largeTrace(tracePaths[1]);
    const std::string largeContents((std::istreambuf_iterator<char>(largeTrace)),
                                    std::istreambuf_iterator<char>());
    EXPECT_NE(smallContents.find("Add (CPU)"), std::string::npos);
    EXPECT_NE(largeContents.find("Add (CPU)"), std::string::npos);
    EXPECT_NE(largeContents.find("Multiply (CPU)"), std::string::npos);
    EXPECT_NE(largeContents.find("Split (RAM)"), std::string::npos);
    EXPECT_NE(largeContents.find("Fuse (RAM)"), std::string::npos);
    smallTrace.close();
    largeTrace.close();
    for (const auto &tracePath : tracePaths) {
        std::filesystem::remove(tracePath);
    }
}

TEST(DemoInputControllerTest, RequiresAConfigurationBeforeRunning) {
    mss::SerializerTool serializer;
    std::istringstream input;
    std::ostringstream terminal;
    mss::UserOutputController output({}, input, terminal);
    mss::MonSuperSysteme system;
    mss::DemoInputController controller(serializer, output, system);

    EXPECT_THROW(controller.run(), std::logic_error);
    EXPECT_THROW(system.initialMatrix(), std::logic_error);
    EXPECT_TRUE(terminal.str().empty());
}
