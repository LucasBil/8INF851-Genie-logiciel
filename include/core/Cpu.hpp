#pragma once

#include "core/Operation.hpp"
#include "core/Matrix.hpp"

#include <iostream>

namespace mss {

/**
 * @brief Processeur virtuel;
 */
class Cpu {
  public:

    /**
     * @brief Construit un processeur virtuel.
     */
    cpu();
    /**
     * @brief Signale que le modèle de coût CPU doit encore être défini.
     * @param operation Opération dont le coût est demandé.
     * @return Coût CPU lorsque le modèle sera défini.
     * @throws std::logic_error tant que la règle de coût n'est pas spécifiée.
     */

    int getOperationCost(Operation& operation) const;

    /**
     * @brief permute les colonnes d'une matrice 2x2
     * @param reference a Matrix 
     * @return Matrice modifie
     * @throws std::logic_error si la matrice n'est pas 2x2
     */
    Matrix& permuteColumn(Matrix& matrix);

    /**
     * @brief permute les lignes d'une matrice 2x2
     * @param reference a Matrix 
     * @return Matrice modifie
     * @throws std::logic_error si la matrice n'est pas 2x2
     */
    Matrix& permuteLine(Matrix& matrix);

    /**
     * @brief roration de 90 degre sens horaire
     * @param reference a Matrix 
     * @return Matrice modifie
     * @throws std::logic_error si la matrice n'est pas 2x2
     */
    Matrix& rotate(Matrix& matrix);

    /**
     * @brief calcul le determinant d'une matrice
     * @param reference a Matrix 
     * @return matrice 1x1
     * @throws std::logic_error si la matrice n'est pas 2x2
     */
    Matrix& calcDeterminant(Matrix& matrix);

    /**
     * @brief Calcule l'addition associée à la matrice.
     * @param matrix Matrice concernée par l'opération.
     * @return Référence à la matrice modifiée.
     * @throws std::logic_error si le calcul n'est pas implémenté.
     */
    Matrix& add(Matrix& matrix);

    /**
     * @brief Calcule la multiplication associée à la matrice.
     * @param matrix Matrice concernée par l'opération.
     * @return Référence à la matrice modifiée.
     * @throws std::logic_error si le calcul n'est pas implémenté.
     */
    Matrix& multiply(Matrix& matrix);

    /**
     * @brief Affiche une matrice dans la sortie standard.
     * @param matrix Matrice à afficher.
     */
    void printMatrix(Matrix& matrix);

};

} // namespace mss
