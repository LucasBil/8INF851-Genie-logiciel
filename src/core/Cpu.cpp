#include "core/Cpu.hpp"

#include <stdexcept>

namespace mss {

int Cpu::getOperationCost(OperationName operation) const {
    (void)operation;
    throw std::logic_error("Le modèle de coût CPU n'a pas encore été spécifié.");
}

} // namespace mss
