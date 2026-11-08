#pragma once

#include "configuration/Configuration.hpp"

#include <fstream>
#include <ios>
#include <string>

namespace mss {

/**
 * @brief Contrat de lecture et d'ecriture d'une configuration.
 */
class ConfigurationSerializer {
  public:
    /**
     * @brief Ouvre un fichier de configuration.
     * @param path Chemin du fichier à ouvrir.
     * @param mode Mode d'ouverture du fichier.
     * @throws std::logic_error si un fichier est déjà ouvert.
     * @throws std::runtime_error si le fichier ne peut pas être ouvert.
     */
    void open(const std::string &path, std::ios::openmode mode = std::ios::in);

    /**
     * @brief Ferme le fichier de configuration ouvert.
     * @throws std::runtime_error si la fermeture échoue.
     */
    void close();

    /**
     * @brief Lit une configuration depuis le fichier ouvert.
     * @return Configuration lue.
     * @throws std::logic_error si aucun fichier n'est ouvert.
     * @throws std::runtime_error si le contenu du fichier est invalide ou illisible.
     */
    Configuration load();

    /**
     * @brief Écrit une configuration dans le fichier ouvert.
     * @param configuration Configuration à enregistrer.
     * @throws std::logic_error si aucun fichier n'est ouvert.
     * @throws std::invalid_argument si un coût n'est pas strictement positif.
     * @throws std::runtime_error si l'écriture échoue.
     */
    void save(const Configuration &configuration);

  private:
    std::fstream file;
};

} // namespace mss
