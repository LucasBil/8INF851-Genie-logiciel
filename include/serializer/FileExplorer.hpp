#pragma once

#include <string>
#include <vector>

namespace mss {

/**
 * @brief Explore les fichiers de configuration disponibles.
 */
class FileExplorer {
  public:
    /**
     * @brief Liste les fichiers de configuration disponibles.
     * @return Liste des configurations triées avec leur index.
     * @throws std::runtime_error si le chemin des configurations n'est pas un répertoire.
     */
    std::string getConfigurationDirectoryContent() const;

    /**
     * @brief Retourne le chemin d'une configuration à partir de son index.
     * @param index Index zéro-based dans la dernière liste obtenue.
     * @return Chemin du fichier de configuration.
     * @throws std::out_of_range si l'index ne correspond pas à une configuration.
     */
    std::string getConfigurationPath(int index) const;

  private:
    static const std::string configurationDirectory;
    mutable std::vector<std::string> configurationsPaths;
};

} // namespace mss
