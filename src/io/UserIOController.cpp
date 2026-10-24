#include "io/UserIOController.hpp"

#include <istream>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace mss {

UserIOController::UserIOController(std::istream &inputStream, std::ostream &outputStream)
    : input(inputStream), output(outputStream) {}

Matrix UserIOController::readMatrix() {
    long long rowCount = 0;
    long long columnCount = 0;
    output << "Entrez le nombre de lignes et de colonnes : ";
    if (!(input >> rowCount >> columnCount)) {
        throw std::runtime_error("Lecture des dimensions de la matrice impossible.");
    }
    if (rowCount <= 0 || columnCount <= 0) {
        throw std::invalid_argument("Les dimensions de la matrice doivent être positives.");
    }
    if(rowCount != columnCount){
        throw std::invalid_argument("Les matrices doivent forcement etre carrees");
    }
    if (rowCount < 2) {
        throw std::invalid_argument("La dimension minimale de la matrice est 2.");
    }
    const auto dimension = static_cast<unsigned long long>(rowCount);
    if ((dimension & (dimension - 1ULL)) != 0ULL) {
        //Pour verifier que la taille est une puissance on fait une conversion binaire
        //ex 4 & 3 => 0100 & 0011 => 0000 => Puissance de 2
        throw std::invalid_argument("La dimension de la matrice doit etre une puissance de 2.");
    }

    const auto rows = static_cast<std::size_t>(rowCount);
    const auto columns = static_cast<std::size_t>(columnCount);
    output << "Entrez les valeurs de la matrice, ligne par ligne :\n";

    std::vector<std::vector<double>> values(rows, std::vector<double>(columns));
    for (auto &row : values) {
        for (double &value : row) {
            if (!(input >> value)) {
                throw std::runtime_error("Lecture des valeurs de la matrice impossible.");
            }
        }
    }
    return Matrix(std::move(values));
}

void UserIOController::writeMatrix(const Matrix &matrix) {
    output << matrix.rows() << " x " << matrix.columns() << '\n';
    for (std::size_t row = 0; row < matrix.rows(); ++row) {
        for (std::size_t column = 0; column < matrix.columns(); ++column) {
            if (column != 0) {
                output << ' ';
            }
            output << matrix.at(row, column);
        }
        output << '\n';
    }
}

} // namespace mss
