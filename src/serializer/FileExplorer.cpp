#include "serializer/FileExplorer.hpp"

#include <algorithm>
#include <filesystem>
#include <sstream>
#include <stdexcept>

namespace mss {

const std::string FileExplorer::configurationDirectory = "configurations";

std::string FileExplorer::getConfigurationDirectoryContent() const {
    configurationsPaths.clear();

    const std::filesystem::path directory(configurationDirectory);

    if (std::filesystem::exists(directory)) {
        if (!std::filesystem::is_directory(directory)) {
            throw std::runtime_error("Le chemin des configurations n'est pas un repertoire.");
        }

        for (const auto &entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                configurationsPaths.push_back(entry.path().string());
            }
        }
    }
    std::sort(configurationsPaths.begin(), configurationsPaths.end());
    std::ostringstream content;
    for (std::size_t index = 0; index < configurationsPaths.size(); ++index) {
        content << index << ". " << std::filesystem::path(configurationsPaths[index]).filename().string()
                << '\n';
    }
    return content.str();
}

std::string FileExplorer::getConfigurationPath(int index) const {
    if (index < 0) {
        throw std::out_of_range("L'index de configuration est invalide.");
    }
    std::sort(configurationsPaths.begin(), configurationsPaths.end());
    const auto position = static_cast<std::size_t>(index);
    if (position >= configurationsPaths.size()) {
        throw std::out_of_range("Aucune configuration ne correspond a cet index.");
    }
    return configurationsPaths[position];
}

} // namespace mss
