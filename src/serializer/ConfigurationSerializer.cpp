#include "serializer/ConfigurationSerializer.hpp"

#include <algorithm>
#include <array>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace mss {
namespace {

constexpr std::size_t operationCount =
    static_cast<std::size_t>(OperationName::CalcDeterminant) + 1;
constexpr std::array<const char *, operationCount> operationNames = {
    "Create", "Delete", "Split", "Fuse", "Load", "Unload",
    "Permutations", "FormuleColumn", "Rotate", "Add", "Multiply", "CalcDeterminant"};

} // namespace

void ConfigurationSerializer::open(const std::string &path, std::ios::openmode mode) {
    if (file.is_open()) {
        throw std::logic_error("Un fichier de configuration est deja ouvert.");
    }
    file.open(path, mode);
    if (!file) {
        throw std::runtime_error("Impossible d'ouvrir le fichier de configuration.");
    }
}

void ConfigurationSerializer::close() {
    if (!file.is_open()) {
        return;
    }
    file.clear();
    file.close();
    if (file.fail()) {
        throw std::runtime_error("La fermeture du fichier de configuration a echoue.");
    }
}

Configuration ConfigurationSerializer::load() {
    if (!file.is_open()) {
        throw std::logic_error("Aucun fichier de configuration n'est ouvert.");
    }

    Configuration configuration;
    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        if (line.empty()) {
            continue;
        }

        std::istringstream entry(line);
        std::string operationName;
        int cost = 0;
        std::string extra;
        if (!(entry >> operationName >> cost) || (entry >> extra)) {
            throw std::runtime_error("Ligne invalide dans le fichier de configuration: " +
                                     std::to_string(lineNumber) + ".");
        }
        const auto operation = std::find(operationNames.begin(), operationNames.end(), operationName);
        if (operation == operationNames.end()) {
            throw std::runtime_error("Operation inconnue dans le fichier de configuration: " +
                                     operationName + ".");
        }
        if (cost <= 0) {
            throw std::runtime_error("Le cout d'une operation doit etre superieur a zero.");
        }

        const auto operationIndex =
            static_cast<OperationName>(std::distance(operationNames.begin(), operation));
        if (configuration.costs.find(operationIndex) != configuration.costs.end()) {
            throw std::runtime_error("Operation dupliquee dans le fichier de configuration.");
        }
        configuration.set(operationIndex, cost);
    }
    if (file.bad()) {
        throw std::runtime_error("La lecture du fichier de configuration a echoue.");
    }
    return configuration;
}

void ConfigurationSerializer::save(const Configuration &configuration) {
    if (!file.is_open()) {
        throw std::logic_error("Aucun fichier de configuration n'est ouvert.");
    }

    std::vector<std::pair<OperationName, int>> costs(configuration.costs.begin(),
                                                      configuration.costs.end());
    std::sort(costs.begin(), costs.end(), [](const auto &left, const auto &right) {
        return static_cast<int>(left.first) < static_cast<int>(right.first);
    });

    for (const auto &entry : costs) {
        const int operation = static_cast<int>(entry.first);
        if (operation < 0 || static_cast<std::size_t>(operation) >= operationNames.size()) {
            throw std::logic_error("La configuration contient une operation inconnue.");
        }
        if (entry.second <= 0) {
            throw std::invalid_argument("Le cout d'une operation doit etre superieur a zero.");
        }
        file << operationNames[static_cast<std::size_t>(operation)] << ' ' << entry.second << '\n';
    }
    if (!file) {
        throw std::runtime_error("L'ecriture du fichier de configuration a echoue.");
    }
    file.flush();
    if (!file) {
        throw std::runtime_error("L'ecriture du fichier de configuration a echoue.");
    }
}

} // namespace mss
