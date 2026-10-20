#pragma once

#include "core/Matrix.hpp"

namespace mss {

/**
 * @brief Contrat d'entrée/sortie pour fournir et afficher une matrice.
 */
class AbstractIOController {
  public:
    /**
     * @brief Détruit le contrôleur d'entrée/sortie.
     */
    virtual ~AbstractIOController() = default;

    /**
     * @brief Lit une matrice.
     * @return Matrice lue depuis la source d'entrée.
     */
    virtual Matrix readMatrix() = 0;

    /**
     * @brief Affiche une matrice.
     * @param matrix Matrice à afficher.
     */
    virtual void writeMatrix(const Matrix &matrix) = 0;
};

} // namespace mss
