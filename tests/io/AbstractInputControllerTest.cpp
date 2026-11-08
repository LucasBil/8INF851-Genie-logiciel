#include "io/AbstractInputController.hpp"

#include "core/MonSuperSysteme.hpp"
#include "io/UserOutputController.hpp"
#include "serializer/SerializerTool.hpp"

#include <gtest/gtest.h>

#include <sstream>

namespace {

class BaseRunController final : public mss::AbstractInputController {
  public:
    BaseRunController(mss::SerializerTool &serializer, mss::UserOutputController &output,
                      mss::MonSuperSysteme &system)
        : AbstractInputController(serializer, output, system) {}

    void run() override { ran = true; }

    bool ran = false;
};

} // namespace

TEST(AbstractInputControllerTest, BaseRunCanBeOverriddenByAConcreteController) {
    mss::SerializerTool serializer;
    mss::Configuration configuration;
    std::istringstream input;
    std::ostringstream terminal;
    mss::UserOutputController output({configuration}, input, terminal);
    mss::MonSuperSysteme system;
    BaseRunController controller(serializer, output, system);

    EXPECT_NO_THROW(controller.run());
    EXPECT_TRUE(controller.ran);
}
