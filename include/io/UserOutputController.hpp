#pragma once

#include "configuration/Configuration.hpp"
#include "core/Matrix.hpp"

#include <iosfwd>
#include <vector>

namespace mss {

/**
 * @brief Affiche les traces MSS et permet de les enregistrer.
 */
class UserOutputController {
  public:
    /**
     * @brief Construit le contrôleur avec les configurations à comparer.
     */
    UserOutputController(std::vector<Configuration> configurations, std::istream &input,
                         std::ostream &output);

    /**
     * @brief Remplace les configurations utilisées pour évaluer une trace.
     * @param configurations Nouvelles configurations.
     */
    void setConfigurations(std::vector<Configuration> configurations);

    /**
     * @brief Indique si au moins une configuration est disponible.
     * @return true si une configuration est disponible.
     */
    bool hasConfigurations() const noexcept;

    /**
     * @brief Affiche une matrice provenant du système MSS.
     */
    void printMatrix(const Matrix &matrix);

    /**
     * @brief Affiche la trace et le coût de chaque configuration, puis propose de l'enregistrer.
     */
    void printTrace(const std::vector<OperationName> &trace, bool askToSave = true);

    /**
     * @brief Enregistre la trace dans le fichier indiqué par l'utilisateur.
     */
    void outputTrace(const std::vector<OperationName> &trace);

  private:
    std::vector<Configuration> configurations;
    std::istream &input;
    std::ostream &output;
};

} // namespace mss
