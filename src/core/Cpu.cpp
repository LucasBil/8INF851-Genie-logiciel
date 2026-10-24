#include "core/Cpu.hpp"

#include <stdexcept>

namespace mss {

int Cpu::getOperationCost(Operation& operation) const {
    (void)operation;
    throw std::logic_error("Le modèle de coût CPU n'a pas encore été spécifié.");
}

    Matrix& Cpu::permuteColumn(Matrix& matrix){
        throw std::logic_error("Calcul non implémenté.");
    }

   Matrix& Cpu::permuteLine(Matrix& matrix){
        throw std::logic_error("Calcul non implémenté.");
    }

    Matrix& Cpu::rotate(Matrix& matrix) {
        throw std::logic_error("Calcul non implémenté.");
    }

    Matrix& Cpu::add(Matrix& matrix){
        throw std::logic_error("Calcul non implémenté.");
    }

    Matrix& Cpu::multiply(Matrix& matrix){
        throw std::logic_error("Calcul non implémenté.");
    }

    Matrix& Cpu::calcDeterminant(Matrix& matrix){
        throw std::logic_error("Calcul non implémenté.");
    }

} // namespace mss
