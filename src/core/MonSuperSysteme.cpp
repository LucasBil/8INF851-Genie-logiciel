#include "core/MonSuperSysteme.hpp"

#include <stdexcept>
#include <utility>

namespace mss {

void MonSuperSysteme::loadInitialMatrix(Matrix matrix) {
    if (ram.size() != 0) {
        throw std::logic_error("La matrice initiale a déjà été chargée.");
    }
    ram.store(std::move(matrix));
}

const Matrix &MonSuperSysteme::initialMatrix() const {
    if (ram.size() == 0) {
        throw std::logic_error("Aucune matrice initiale n'a été chargée.");
    }
    return ram.at(0);
}

const Cpu &MonSuperSysteme::cpu() const noexcept {
    return cpuInstance;
}

} // namespace mss
