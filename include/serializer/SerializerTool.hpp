#pragma once

#include "configuration/Configuration.hpp"
#include "serializer/ConfigurationSerializer.hpp"
#include "serializer/FileExplorer.hpp"

#include <string>

namespace mss {

/**
 * @brief sauvegarde et de chargement des configurations.
 */
class SerializerTool {
  public:
    /**
     * @brief Retourne la liste des fichiers de configuration disponibles.
     * @return Liste des configurations avec leur index.
     */
    std::string getConfigurationDirectoryContent();

    /**
     * @brief Enregistre une configuration dans le répertoire des configurations.
     * @param configuration Configuration à enregistrer.
     * @param name Nom de fichier de la configuration.
     * @throws std::invalid_argument si le nom n'est pas un nom de fichier simple.
     */
    void saveConfiguration(const Configuration &configuration, const std::string &name);

    /**
     * @brief Charge une configuration par son index zero-based dans la liste des fichiers.
     * @throws std::out_of_range si l'index ne correspond pas a une configuration.
     */
    Configuration loadConfiguration(int index);

  private:
    FileExplorer fileExplorer;
    ConfigurationSerializer configurationSerializer;
};

} // namespace mss
