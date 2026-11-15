#include "io/DemoInputController.hpp"

#include "core/Matrix.hpp"
#include "core/MonSuperSysteme.hpp"
#include "io/UserOutputController.hpp"

#include <chrono>
#include <cstddef>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace mss {
namespace {

std::filesystem::path nextDemoTracePath(const std::string &caseName) {
    const std::filesystem::path directory("trace");
    std::filesystem::create_directories(directory);

    const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const std::string stem =
        "demo_input_" + caseName + "_" + std::to_string(timestamp);
    std::filesystem::path filePath = directory / (stem + ".txt");
    for (std::size_t suffix = 1; std::filesystem::exists(filePath); ++suffix) {
        filePath = directory / (stem + "_" + std::to_string(suffix) + ".txt");
    }
    return filePath;
}

void saveDemoTrace(UserOutputController &output, const std::vector<OperationName> &trace,
                   const std::string &caseName) {
    output.printTrace(trace, false);
    output.outputTrace(trace, nextDemoTracePath(caseName));
}

} // namespace

DemoInputController::DemoInputController(SerializerTool &serializer, UserOutputController &output,
                                         MonSuperSysteme &mssInstance)
    : AbstractInputController(serializer, output, mssInstance) {}

void DemoInputController::run() {
    if (!output->hasConfigurations()) {
        throw std::logic_error("Chargez une configuration avant de lancer la demonstration.");
    }

    mssInstance->loadInitialMatrix(Matrix({{1.0, 2.0}, {3.0, 4.0}}));
    const int secondMatrix = mssInstance->loadMatrix(Matrix({{5.0, 6.0}, {7.0, 8.0}}));
    mssInstance->loadOperation(OperationName::Add, {0, secondMatrix});
    mssInstance->loadOperation(OperationName::Multiply, {0, secondMatrix});
    const std::vector<int> quadrants =
        mssInstance->loadOperation(OperationName::Split, {0});
    mssInstance->loadOperation(OperationName::Fuse, quadrants);

    mssInstance->run();
    if (mssInstance->executionTrace().empty()) {
        throw std::logic_error("MSS n'a execute aucune operation dans le scenario de demonstration.");
    }
    saveDemoTrace(*output, mssInstance->executionTrace(), "2x2");

    const std::size_t secondCaseTraceStart = mssInstance->executionTrace().size();
    const int firstFourByFour =
        mssInstance->loadMatrix(Matrix({{1.0, 2.0, 3.0, 4.0},
                                        {5.0, 6.0, 7.0, 8.0},
                                        {9.0, 10.0, 11.0, 12.0},
                                        {13.0, 14.0, 15.0, 16.0}}));
    const int secondFourByFour =
        mssInstance->loadMatrix(Matrix({{16.0, 15.0, 14.0, 13.0},
                                        {12.0, 11.0, 10.0, 9.0},
                                        {8.0, 7.0, 6.0, 5.0},
                                        {4.0, 3.0, 2.0, 1.0}}));
    mssInstance->loadOperation(OperationName::Add, {firstFourByFour, secondFourByFour});
    mssInstance->loadOperation(OperationName::Multiply,
                               {firstFourByFour, secondFourByFour});
    mssInstance->run();

    const auto &completeTrace = mssInstance->executionTrace();
    if (completeTrace.size() == secondCaseTraceStart) {
        throw std::logic_error("MSS n'a execute aucune operation dans le scenario 4x4.");
    }
    const std::vector<OperationName> secondCaseTrace(
        completeTrace.begin() + static_cast<std::ptrdiff_t>(secondCaseTraceStart),
        completeTrace.end());
    saveDemoTrace(*output, secondCaseTrace, "4x4");
}

} // namespace mss