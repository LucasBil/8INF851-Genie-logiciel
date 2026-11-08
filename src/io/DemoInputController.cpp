#include "io/DemoInputController.hpp"

#include "core/Matrix.hpp"
#include "core/MonSuperSysteme.hpp"
#include "io/UserOutputController.hpp"

#include <stdexcept>

namespace mss {

DemoInputController::DemoInputController(SerializerTool &serializer, UserOutputController &output,
                                         MonSuperSysteme &mssInstance)
    : AbstractInputController(serializer, output, mssInstance) {}

void DemoInputController::run() {
    mssInstance->loadInitialMatrix(Matrix({{1.0, 2.0}, {3.0, 4.0}}));
    mssInstance->run();
    if (mssInstance->executionTrace().empty()) {
        throw std::logic_error("MSS n'a execute aucune operation dans le scenario de demonstration.");
    }
    output->printTrace(mssInstance->executionTrace(), false);
}

} // namespace mss