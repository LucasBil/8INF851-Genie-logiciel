#include "serializer/SerializerTool.hpp"

#include <filesystem>
#include <ios>
#include <stdexcept>
#include <utility>

namespace mss {

std::string SerializerTool::getConfigurationDirectoryContent() {
    try {
        return fileExplorer.getConfigurationDirectoryContent();
    } catch (const std::exception &error) {
        logger.error(std::string("Impossible de lister les configurations : ") + error.what());
        throw;
    }
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
    try {
        std::filesystem::create_directories(path.parent_path());
        configurationSerializer.open(path.string(), std::ios::out | std::ios::trunc);
        configurationSerializer.save(configuration);
        configurationSerializer.close();
        logger.info("Configuration sauvegardee : " + path.string());
    } catch (const std::exception &error) {
        logger.error("Echec de sauvegarde de la configuration " + path.string() + " : " +
                     error.what());
        configurationSerializer.close();
        throw;
    } catch (...) {
        configurationSerializer.close();
        throw;
    }
}

Configuration SerializerTool::loadConfiguration(int index) {
    const std::string path = fileExplorer.getConfigurationPath(index);
    try {
        configurationSerializer.open(path);
        Configuration configuration = configurationSerializer.load();
        configurationSerializer.close();
        logger.info("Configuration chargee : " + path);
        return configuration;
    } catch (const std::exception &error) {
        logger.error("Echec de chargement de la configuration " + path + " : " +
                     error.what());
        configurationSerializer.close();
        throw;
    } catch (...) {
        configurationSerializer.close();
        throw;
    }
}

} // namespace mss
