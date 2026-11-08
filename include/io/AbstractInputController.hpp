#pragma once

namespace mss {

class MonSuperSysteme;
class SerializerTool;
class UserOutputController;

/**
 * @brief Contrat abstrait d'un contrôleur d'entrée du système.
 *
 * Les dépendances sont empruntées et doivent rester valides pendant toute la
 * durée de vie du contrôleur.
 */
class AbstractInputController {
  public:
    /**
     * @brief Construit un contrôleur avec ses dépendances empruntées.
     * @param serializer Outil de lecture et d'enregistrement des configurations.
     * @param output Contrôleur de sortie associé.
     * @param mssInstance Système MSS à piloter.
     */
    AbstractInputController(SerializerTool &serializer, UserOutputController &output, MonSuperSysteme &mssInstance) noexcept;

    /**
     * @brief Détruit le contrôleur d'entrée.
     */
    virtual ~AbstractInputController();

    /**
     * @brief Exécute le comportement propre à l'interface concrète.
     */
    virtual void run() = 0;

  protected:
    SerializerTool *serializer;
    UserOutputController *output;
    MonSuperSysteme *mssInstance;
};

} // namespace mss
