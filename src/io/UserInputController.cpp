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

bool parseOperationChoice(int choice, OperationName &operation) {
    switch (choice) {
    case 1: operation = OperationName::Split; return true;
    case 2: operation = OperationName::Fuse; return true;
    case 3: operation = OperationName::Rotate; return true;
    case 4: operation = OperationName::Add; return true;
    case 5: operation = OperationName::Multiply; return true;
    case 6: operation = OperationName::CalcDeterminant; return true;
    default: return false;
    }
}

std::size_t requiredInputCount(OperationName operation) {
    switch (operation) {
    case OperationName::Split:
    case OperationName::Rotate:
    case OperationName::CalcDeterminant:
        return 1;
    case OperationName::Fuse:
        return 4;
    case OperationName::Add:
    case OperationName::Multiply:
        return 2;
    default:
        throw std::logic_error("Nombre d'entrees non defini pour cette operation.");
    }
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
    case OperationName::Print: return "Print";
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
            if (!readInteger(std::string("Cout de ") + name +" (strictement positif) : ",
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

void UserInputController::createMatrix() {
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

    auto &ram = mssInstance->getRam();
    std::size_t address = 0;
    Matrix matrix(std::move(values));
    if (ram.size() == 0) {
        mssInstance->loadInitialMatrix(std::move(matrix));
    } else {
        address = ram.store(std::move(matrix));
    }
    terminal << "Matrice creee a l'adresse RAM " << address << ".\n";
    output->printMatrix(ram.at(address));
}

void UserInputController::execute() {
    if (!output->hasConfigurations()) {
        terminal << "Chargez ou creez une configuration avant l'execution.\n";
        return;
    }

    auto &ram = mssInstance->getRam();
    std::vector<std::size_t> matrixAddresses;
    for (std::size_t address = 0; address < ram.size(); ++address) {
        if (!ram.occupied(address)) {
            continue;
        }
        try {
            const Matrix &matrix = ram.at(address);
            matrixAddresses.push_back(address);
            terminal << "Matrice disponible a l'adresse RAM " << address << ":\n";
            output->printMatrix(matrix);
        } catch (const std::invalid_argument &) {
            continue;
        }
    }
    if (matrixAddresses.empty()) {
        terminal << "Entrez ou generez une matrice avant de planifier une operation.\n";
        return;
    }

    std::vector<std::pair<OperationName, std::vector<int>>> operations;
    terminal << "Operations disponibles :\n"
             << "1. Split (1 matrice)\n"
             << "2. Fuse (4 matrices)\n"
             << "3. Rotate (1 matrice)\n"
             << "4. Add (2 matrices)\n"
             << "5. Multiply (2 matrices)\n"
             << "6. CalcDeterminant (1 matrice)\n";

    while (true) {
        terminal << "Operation a ajouter (q pour terminer) : ";
        std::string selection;
        if (!(input >> selection)) {
            return;
        }
        if (selection == "q" || selection == "Q") {
            break;
        }

        int choice = 0;
        OperationName operation = OperationName::Split;
        if (!parseInteger(selection, choice) || !parseOperationChoice(choice, operation)) {
            terminal << "Operation invalide : indiquez un numero entre 1 et 6, ou q.\n";
            continue;
        }

        std::vector<int> inputLocations;
        const std::size_t inputCount = requiredInputCount(operation);
        if (matrixAddresses.size() < inputCount) {
            terminal << "Cette operation necessite " << inputCount
                     << " matrice(s), mais seulement " << matrixAddresses.size()
                     << " est/sont disponible(s).\n";
            return;
        }
        inputLocations.reserve(inputCount);
        const auto firstInput = matrixAddresses.end() -
                                static_cast<std::ptrdiff_t>(inputCount);
        for (auto address = firstInput; address != matrixAddresses.end(); ++address) {
            inputLocations.push_back(static_cast<int>(*address));
        }
        operations.emplace_back(operation, std::move(inputLocations));
    }

    std::vector<int> operationAddresses;
    operationAddresses.reserve(operations.size());
    for (auto &operation : operations) {
        operationAddresses.push_back(
            mssInstance->getRam().load(operation.first, operation.second));
    }
    for (auto address = operationAddresses.rbegin(); address != operationAddresses.rend();
         ++address) {
        mssInstance->getStack().push(*address);
    }
    terminal << operations.size() << " operation(s) ajoutee(s) a MSS.\n";
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
                 << "3. Entrer une matrice\n"
                 << "4. Executer une operation\n"
                 << "5. Quitter\n";
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
                createMatrix();
                break;
            case 4:
                execute();
                break;
            case 5:
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
