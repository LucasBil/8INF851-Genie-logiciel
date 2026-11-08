#include "io/DemoInputController.hpp"

#include "core/MonSuperSysteme.hpp"
#include "io/UserOutputController.hpp"
#include "serializer/SerializerTool.hpp"

#include <gtest/gtest.h>

#include <sstream>

TEST(DemoInputControllerTest, ReportsWhenTheCoreDoesNotExecuteTheDemoOperation) {
    mss::SerializerTool serializer;
    mss::Configuration configuration;
    std::istringstream input;
    std::ostringstream terminal;
    mss::UserOutputController output({configuration}, input, terminal);
    mss::MonSuperSysteme system;
    mss::DemoInputController controller(serializer, output, system);

    EXPECT_THROW(controller.run(), std::logic_error);

    EXPECT_DOUBLE_EQ(system.initialMatrix().at(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(system.initialMatrix().at(1, 1), 4.0);
    EXPECT_EQ(terminal.str().find("Trace d'execution :"), std::string::npos);
    EXPECT_EQ(terminal.str().find("Voulez-vous sauvegarder"), std::string::npos);
    EXPECT_TRUE(input.good());
}
