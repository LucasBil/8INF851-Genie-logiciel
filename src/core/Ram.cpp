#include "core/Ram.hpp"

#include <stdexcept>
#include <utility>

namespace mss {

std::size_t Ram::store(Matrix matrix) {
    memory.push_back(std::move(matrix));
    return memory.size() - 1;
}

const Matrix &Ram::at(std::size_t address) const {
    return memory.at(address);
}

std::size_t Ram::size() const noexcept {
    return memory.size();
}

int Ram::getOperationCost(Operation& operation) const {
    (void)operation;
    throw std::logic_error("Le modèle de coût RAM n'a pas encore été spécifié.");
}

} // namespace mss
