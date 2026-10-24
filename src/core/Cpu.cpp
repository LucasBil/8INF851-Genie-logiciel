#include "core/Cpu.hpp"

#include <stdexcept>

namespace mss {

int Cpu::getOperationCost(Operation& operation) const {
    (void)operation;
    throw std::logic_error("Le modèle de coût CPU n'a pas encore été spécifié.");
}

void requireSquare2(const Matrix& m) {
    if (m.rows() != 2 || m.columns() != 2)
        throw std::invalid_argument("matrice 2x2 attendue");
}

Matrix& Cpu::permuteColumn(Matrix& matrix){
    requireSquare2(matrix);
    matrix = Matrix({{matrix.at(0, 1), matrix.at(0, 0)},
                    {matrix.at(1, 1), matrix.at(1, 0)}});
    return matrix;
}
Matrix& Cpu::permuteLine(Matrix& matrix){
    requireSquare2(matrix);
    matrix = Matrix({{matrix.at(1, 0), matrix.at(1, 1)},
                     {matrix.at(0, 0), matrix.at(0, 1)}});
    return matrix;
}

Matrix& Cpu::rotate(Matrix& matrix) {
    requireSquare2(matrix);
    matrix = Matrix({{matrix.at(1, 0), matrix.at(0, 0)},
                     {matrix.at(1, 1), matrix.at(0, 1)}});
    return matrix;
}

Matrix& Cpu::add(Matrix& matrix){
    throw std::logic_error("Calcul non implémenté.");
    return matrix;
}

Matrix& Cpu::multiply(Matrix& matrix){
    throw std::logic_error("Calcul non implémenté.");
    return matrix;
}
Matrix& Cpu::calcDeterminant(Matrix& matrix){
    requireSquare2(matrix);
    const double det = matrix.at(0, 0) * matrix.at(1, 1)
                     - matrix.at(0, 1) * matrix.at(1, 0);
    matrix = Matrix({{det}});
    return matrix;
}

void Cpu::printMatrix(Matrix& matrix){
    for(size_t row = 0; row < matrix.rows(); row++){
        for(size_t col=0; col<matrix.columns(); col++){
            std::cout << matrix.at(row,col) << " ";
        }
    std::cout <<"\n";
    }

    std::cout << "\n";
}
} // namespace mss
