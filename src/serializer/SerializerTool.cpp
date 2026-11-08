#include "serializer/SerializerTool.hpp"

#include <filesystem>
#include <ios>
#include <stdexcept>
#include <utility>

namespace mss {

std::string SerializerTool::getConfigurationDirectoryContent() {
    return fileExplorer.getConfigurationDirectoryContent();
}

void SerializerTool::saveConfiguration(const Configuration &configuration, const std::string &name) {
    const std::filesystem::path fileName(name);
    if (name.empty() || fileName.has_parent_path() || fileName.filename() != fileName ||
        name == "." || name == "..") {
        throw std::invalid_argument("Le nom de configuration doit etre un nom de fichier simple.");
    }

    std::filesystem::path path = std::filesystem::path("configurations") / fileName;
    if (path.extension() != ".txt") {
        path += ".txt";
    }
    std::filesystem::create_directories(path.parent_path());
    configurationSerializer.open(path.string(), std::ios::out | std::ios::trunc);
    try {
        configurationSerializer.save(configuration);
        configurationSerializer.close();
    } catch (...) {
        configurationSerializer.close();
        throw;
    }
}

Configuration SerializerTool::loadConfiguration(int index) {
    const std::string path = fileExplorer.getConfigurationPath(index);
    configurationSerializer.open(path);
    try {
        Configuration configuration = configurationSerializer.load();
        configurationSerializer.close();
        return configuration;
    } catch (...) {
        configurationSerializer.close();
        throw;
    }
}

} // namespace mss
