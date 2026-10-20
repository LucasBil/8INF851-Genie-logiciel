#pragma once

#include <cstddef>
#include <vector>

namespace mss {

/**
 * @brief Matrice rectangulaire de nombres réels.
 */
class Matrix {
  public:
    /**
     * @brief Construit une matrice à partir de lignes non vides et rectangulaires.
     * @param matrixValues Valeurs de la matrice.
     * @throws std::invalid_argument si la matrice est vide ou non rectangulaire.
     */
    explicit Matrix(std::vector<std::vector<double>> matrixValues);

    /**
     * @brief Retourne le nombre de lignes.
     * @return Nombre de lignes.
     */
    std::size_t rows() const noexcept;

    /**
     * @brief Retourne le nombre de colonnes.
     * @return Nombre de colonnes.
     */
    std::size_t columns() const noexcept;

    /**
     * @brief Accède à une valeur avec vérification des bornes.
     * @param row Indice de la ligne.
     * @param column Indice de la colonne.
     * @return Valeur à la position demandée.
     * @throws std::out_of_range si une coordonnée est hors limites.
     */
    double at(std::size_t row, std::size_t column) const;

  private:
    std::vector<std::vector<double>> values;
};

} // namespace mss
