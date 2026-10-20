#pragma once

#include "io/AbstractIOController.hpp"

#include <iosfwd>

namespace mss {

/**
 * @brief Contrôleur interactif de matrices sur des flux C++ injectables.
 */
class UserIOController final : public AbstractIOController {
  public:
    /**
     * @brief Crée un contrôleur associé aux flux fournis.
     * @param inputStream Flux utilisé pour lire les données.
     * @param outputStream Flux utilisé pour afficher les invites et résultats.
     */
    UserIOController(std::istream &inputStream, std::ostream &outputStream);

    /**
     * @brief Lit les dimensions puis les valeurs de la matrice.
     * @return Matrice saisie par l'utilisateur.
     * @throws std::runtime_error si le flux ne contient pas les données attendues.
     * @throws std::invalid_argument si les dimensions ne sont pas positives.
     */
    Matrix readMatrix() override;

    /**
     * @brief Écrit les dimensions puis les lignes de la matrice.
     * @param matrix Matrice à afficher.
     */
    void writeMatrix(const Matrix &matrix) override;

  private:
    std::istream &input;
    std::ostream &output;
};

} // namespace mss
