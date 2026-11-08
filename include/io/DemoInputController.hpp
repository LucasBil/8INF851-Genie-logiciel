#pragma once

#include "io/AbstractInputController.hpp"

namespace mss {

/**
 * @brief Controleur d'entree utilisant des donnees fixes pour les demonstrations.
 */
class DemoInputController final : public AbstractInputController {
  public:
    /**
     * @brief Construit le contrôleur de démonstration avec ses dépendances.
     * @param serializer Outil de lecture et d'enregistrement des configurations.
     * @param output Contrôleur de sortie associé.
     * @param mssInstance Système MSS à piloter.
     */
    DemoInputController(SerializerTool &serializer, UserOutputController &output,
                        MonSuperSysteme &mssInstance);

    /**
     * @brief Charge la matrice de demonstration, lance MSS et affiche sa trace.
     */
    void run() override;
};

} // namespace mss