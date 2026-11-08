#pragma once

#include "configuration/Configuration.hpp"
#include "io/AbstractInputController.hpp"

#include <iosfwd>
#include <string>
#include <vector>

namespace mss {

/**
 * @brief Contrôleur d'entrée interactif utilisant le terminal.
 */
class UserInputController final : public AbstractInputController {
  public:
    /**
     * @brief Construit le contrôleur avec ses dépendances et les flux du terminal.
     */
    UserInputController(SerializerTool &serializer, UserOutputController &output,
                        MonSuperSysteme &mssInstance, std::istream &input, std::ostream &terminal);

    /**
     * @brief Lit la matrice, lance MSS et transmet sa trace au contrôleur de sortie.
     */
    void run() override;

  private:
    /**
     * @brief Lit un entier depuis le flux d'entrée, en redemandant en cas d'erreur.
     * @param prompt Invite affichée à l'utilisateur.
     * @param value Reçoit l'entier lu.
     * @return true si un entier a été lu, false si le flux est terminé.
     */
    bool readInteger(const std::string &prompt, int &value);

    /**
     * @brief Charge les configurations choisies par l'utilisateur.
     */
    void loadConfiguration();

    /**
     * @brief Demande et enregistre une nouvelle configuration.
     */
    void saveConfiguration();

    /**
     * @brief Demande une matrice à l'utilisateur et la charge dans la RAM.
     */
    void createMatrix();

    /**
     * @brief Demande une suite d'opérations et lance leur exécution.
     */
    void execute();

    std::istream &input;
    std::ostream &terminal;
    std::vector<Configuration> configurations;
};

} // namespace mss
