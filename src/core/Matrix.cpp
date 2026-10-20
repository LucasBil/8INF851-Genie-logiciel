#include "core/Matrix.hpp"

#include <stdexcept>
#include <utility>

namespace mss {

Matrix::Matrix(std::vector<std::vector<double>> matrixValues) : values(std::move(matrixValues)) {
    if (values.empty() || values.front().empty()) {
        throw std::invalid_argument("Une matrice doit contenir au moins une ligne et une colonne.");
    }

    const std::size_t expectedColumns = values.front().size();
    for (const auto &row : values) {
        if (row.size() != expectedColumns) {
            throw std::invalid_argument(
                "Toutes les lignes de la matrice doivent avoir la même taille.");
        }
    }
}

std::size_t Matrix::rows() const noexcept {
    return values.size();
}

std::size_t Matrix::columns() const noexcept {
    return values.front().size();
}

double Matrix::at(std::size_t row, std::size_t column) const {
    return values.at(row).at(column);
}

} // namespace mss
