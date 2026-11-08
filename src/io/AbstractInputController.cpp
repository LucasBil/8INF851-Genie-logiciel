#include "io/AbstractInputController.hpp"

namespace mss {

AbstractInputController::AbstractInputController(SerializerTool &serializer, UserOutputController &output, MonSuperSysteme &mssInstance) noexcept 
: serializer(&serializer), output(&output), mssInstance(&mssInstance) {}

AbstractInputController::~AbstractInputController() = default;

} // namespace mss
