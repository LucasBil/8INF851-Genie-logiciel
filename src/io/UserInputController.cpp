#include "io/UserInputController.hpp"

#include "core/Matrix.hpp"
#include "core/MonSuperSysteme.hpp"
#include "io/UserOutputController.hpp"
#include "serializer/SerializerTool.hpp"

#include <algorithm>
#include <charconv>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace mss {
namespace {

bool parseInteger(const std::string &text, int &value) {
    const char *begin = text.data();
    const char *end = begin + text.size();
    const auto result = std::from_chars(begin, end, value);
    return result.ec == std::errc{} && result.ptr == end;
}

bool isPowerOfTwo(unsigned int value) {
    return value != 0 && (value & (value - 1U)) == 0;
}

const char *operationName(OperationName operation) {
    switch (operation) {
    case OperationName::Create: return "Create";
    case OperationName::Delete: return "Delete";
    case OperationName::Split: return "Split";
    case OperationName::Fuse: return "Fuse";
    case OperationName::Load: return "Load";
    case OperationName::Unload: return "Unload";
    case OperationName::Permutations: return "Permutations";
    case OperationName::FormuleColumn: return "FormuleColumn";
    case OperationName::Rotate: return "Rotate";
    case OperationName::Add: return "Add";
    case OperationName::Multiply: return "Multiply";
    case OperationName::CalcDeterminant: return "CalcDeterminant";
    }
    throw std::logic_error("Nom d'operation inconnu.");
}

} // namespace

UserInputController::UserInputController(SerializerTool &serializer, UserOutputController &output,
                                         MonSuperSysteme &mssInstance, std::istream &input,
                                         std::ostream &terminal)
    : AbstractInputController(serializer, output, mssInstance), input(input), terminal(terminal) {}

bool UserInputController::readInteger(const std::string &prompt, int &value) {
    std::string token;
    while (true) {
        terminal << prompt;
        if (!(input >> token)) {
            return false;
        }
        if (parseInteger(token, value)) {
            return true;
        }
        terminal << "Entree invalide : un entier est requis.\n";
    }
}

void UserInputController::loadConfiguration() {
    while (true) {
        std::string content;
        try {
            content = serializer->getConfigurationDirectoryContent();
        } catch (const std::exception &error) {
            terminal << "Erreur de lecture des configurations : " << error.what() << '\n';
            return;
        }
        terminal << "Configurations disponibles :\n";
        if (content.empty()) {
            terminal << "(aucune configuration disponible)\n";
        } else {
            terminal << content;
        }

        terminal << "Indice a charger ou q pour terminer : ";
        std::string selection;
        if (!(input >> selection)) {
            return;
        }
        if (selection == "q" || selection == "Q") {
            output->setConfigurations(configurations);
            return;
        }
        int index = 0;
        if (!parseInteger(selection, index)) {
            terminal << "Entree invalide : indiquez un indice ou q.\n";
            continue;
        }

        try {
            Configuration configuration = serializer->loadConfiguration(index);
            configurations.push_back(std::move(configuration));
            terminal << "Configuration chargee.\n";
        } catch (const std::out_of_range &error) {
            terminal << "Erreur : " << error.what() << '\n';
        } catch (const std::exception &error) {
            terminal << "Erreur de chargement : " << error.what() << '\n';
            return;
        }
    }
}

void UserInputController::saveConfiguration() {
    Configuration configuration;
    for (int value = 0; value <= static_cast<int>(OperationName::CalcDeterminant); ++value) {
        const auto operation = static_cast<OperationName>(value);
        const std::string name = operationName(operation);
        int cost = 0;
        while (true) {
            if (!readInteger(std::string("Cout de ") + name +
                                 " (strictement positif) : ",
                             cost)) {
                return;
            }
            if (cost > 0) {
                break;
            }
            terminal << "Le cout doit etre strictement superieur a zero.\n";
        }
        configuration.set(operation, cost);
    }

    terminal << "Nom de la configuration : ";
    std::string name;
    if (!(input >> name)) {
        return;
    }

    serializer->saveConfiguration(configuration, name);
    configurations.push_back(std::move(configuration));
    output->setConfigurations(configurations);
    terminal << "Configuration sauvegardee et ajoutee aux configurations chargees.\n";
}

void UserInputController::execute() {
    if (!output->hasConfigurations()) {
        terminal << "Chargez ou creez une configuration avant l'execution.\n";
        return;
    }
    if (matrixLoaded) {
        terminal << "Une matrice a deja ete chargee; une nouvelle execution est impossible.\n";
        return;
    }

    int rowCount = 0;
    int columnCount = 0;
    while (true) {
        if (!readInteger("Nombre de lignes : ", rowCount) ||
            !readInteger("Nombre de colonnes : ", columnCount)) {
            return;
        }
        if (rowCount < 2 || columnCount < 2) {
            terminal << "La dimension minimale de la matrice est 2.\n";
        } else if (rowCount != columnCount) {
            terminal << "Les matrices doivent etre carrees.\n";
        } else if (!isPowerOfTwo(static_cast<unsigned int>(rowCount))) {
            terminal << "La dimension de la matrice doit etre une puissance de 2.\n";
        } else {
            break;
        }
    }

    const auto dimension = static_cast<std::size_t>(rowCount);
    std::vector<std::vector<double>> values(dimension, std::vector<double>(dimension));
    terminal << "Entrez les valeurs entieres de la matrice, ligne par ligne :\n";
    for (auto &row : values) {
        for (double &value : row) {
            int matrixValue = 0;
            if (!readInteger("Valeur : ", matrixValue)) {
                return;
            }
            value = static_cast<double>(matrixValue);
        }
    }

    mssInstance->loadInitialMatrix(Matrix(std::move(values)));
    matrixLoaded = true;
    mssInstance->run();
    if (mssInstance->executionTrace().empty()) {
        terminal << "Aucune operation n'a ete executee par MSS; aucun bilan n'est disponible.\n";
        return;
    }
    output->printTrace(mssInstance->executionTrace());
}

void UserInputController::run() {
    while (true) {
        terminal << "\nMenu principal\n"
                 << "1. Charger une configuration\n"
                 << "2. Creer et sauvegarder une configuration\n"
                 << "3. Entrer une matrice et executer MSS\n"
                 << "4. Quitter\n";
        int choice = 0;
        if (!readInteger("Votre choix : ", choice)) {
            return;
        }

        try {
            switch (choice) {
            case 1:
                loadConfiguration();
                break;
            case 2:
                saveConfiguration();
                break;
            case 3:
                execute();
                break;
            case 4:
                return;
            default:
                terminal << "Choix invalide.\n";
                break;
            }
        } catch (const std::exception &error) {
            terminal << "Erreur : " << error.what() << '\n';
        }
    }
}

} // namespace mss
