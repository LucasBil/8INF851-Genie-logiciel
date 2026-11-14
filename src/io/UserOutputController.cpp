#include "io/UserOutputController.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace mss {
namespace {

enum class Component {
    Ram,
    Cpu,
    Io
};

const char *operationName(OperationName operation) {
    //Permet de transformer les OperationName enum en chaîne de caractère
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
    case OperationName::Mirror: return "Mirror";
    }
    throw std::logic_error("Nom d'operation inconnu.");
}

Component operationComponent(OperationName operation) {
    switch (operation) {
    case OperationName::Create:
    case OperationName::Delete:
    case OperationName::Split:
    case OperationName::Fuse:
    case OperationName::Load:
    case OperationName::Unload:
        return Component::Ram;
    case OperationName::Permutations:
    case OperationName::FormuleColumn:
    case OperationName::Rotate:
    case OperationName::Add:
    case OperationName::Multiply:
    case OperationName::CalcDeterminant:
        return Component::Cpu;
    case OperationName::Print:
        return Component::Io;
    }
    throw std::logic_error("Composant d'operation inconnu.");
}

const char *componentName(Component component) {
    switch (component) {
    case Component::Ram: return "RAM";
    case Component::Cpu: return "CPU";
    case Component::Io: return "E/S";
    }
    throw std::logic_error("Nom de composant inconnu.");
}

void addToComponentTotal(std::vector<long long> &totals, Component component, int cost) {
    switch (component) {
    case Component::Ram: totals[0] += cost; break;
    case Component::Cpu: totals[1] += cost; break;
    case Component::Io: totals[2] += cost; break;
    }
}

std::string buildReport(const std::vector<Configuration> &configurations,
                        const std::vector<OperationName> &trace) {
    if (configurations.empty()) {
        throw std::logic_error("Aucune configuration n'est active.");
    }

    std::vector<long long> totals;
    totals.reserve(configurations.size());
    std::ostringstream report;
    report << "Trace d'execution :\n";
    for (std::size_t index = 0; index < trace.size(); ++index) {
        report << index + 1 << ". " << operationName(trace[index]) << " ("
               << componentName(operationComponent(trace[index])) << ")\n";
    }

    for (std::size_t configurationIndex = 0; configurationIndex < configurations.size();
         ++configurationIndex) {
        const Configuration &configuration = configurations[configurationIndex];
        long long total = 0;
        std::vector<long long> componentTotals(3, 0);
        std::vector<int> operationCosts;
        operationCosts.reserve(trace.size());
        for (const OperationName operation : trace) {
            const int cost = configuration.get(operation);
            operationCosts.push_back(cost);
            total += cost;
            addToComponentTotal(componentTotals, operationComponent(operation), cost);
        }
        totals.push_back(total);
        report << "Configuration " << configurationIndex + 1 << " : " << total << '\n';
        report << "  Sous-total RAM : " << componentTotals[0] << '\n'
               << "  Sous-total CPU : " << componentTotals[1] << '\n';
        if (componentTotals[2] != 0) {
            report << "  Sous-total E/S : " << componentTotals[2] << '\n';
        }
        for (std::size_t operationIndex = 0; operationIndex < trace.size(); ++operationIndex) {
            report << "  " << operationIndex + 1 << ". "
                   << operationName(trace[operationIndex]) << " : "
                   << operationCosts[operationIndex] << " ("
                   << componentName(operationComponent(trace[operationIndex])) << ")\n";
        }
    }

    const auto best = std::min_element(totals.begin(), totals.end());
    report << "Meilleure configuration : " << std::distance(totals.begin(), best) + 1
           << " (cout : " << *best << ")\n";
    return report.str();
}

} // namespace

UserOutputController::UserOutputController(std::vector<Configuration> configurations,
                                           std::istream &input, std::ostream &output)
    : configurations(std::move(configurations)), input(input), output(output) {}

void UserOutputController::setConfigurations(std::vector<Configuration> newConfigurations) {
    configurations = std::move(newConfigurations);
}

bool UserOutputController::hasConfigurations() const noexcept {
    return !configurations.empty();
}

void UserOutputController::printMatrix(const Matrix &matrix) {
    output << "Matrice :\n";
    for (std::size_t row = 0; row < matrix.rows(); ++row) {
        for (std::size_t column = 0; column < matrix.columns(); ++column) {
            if (column != 0) {
                output << ' ';
            }
            output << matrix.at(row, column);
        }
        output << '\n';
    }
}

void UserOutputController::printTrace(const std::vector<OperationName> &trace, bool askToSave) {
    const std::string report = buildReport(configurations, trace);
    output << report;
    if (!askToSave) {
        return;
    }

    while (true) {
        output << "Voulez-vous sauvegarder cette trace ? (o/n) ";
        std::string choice;
        if (!(input >> choice)) {
            throw std::runtime_error("Lecture du choix de sauvegarde impossible.");
        }
        std::transform(choice.begin(), choice.end(), choice.begin(),
                       [](unsigned char character) {
                           return static_cast<char>(std::tolower(character));
                       });
        if (choice == "o" || choice == "oui") {
            outputTrace(trace);
            return;
        }
        if (choice == "n" || choice == "non") {
            return;
        }
        output << "Repondez par oui ou non.\n";
    }
}

void UserOutputController::outputTrace(const std::vector<OperationName> &trace) {
    output << "Nom du fichier de sauvegarde : ";
    std::string fileName;
    if (!(input >> fileName)) {
        throw std::runtime_error("Lecture du nom de fichier impossible.");
    }

    outputTrace(trace, std::filesystem::path(fileName));
}

void UserOutputController::outputTrace(const std::vector<OperationName> &trace,
                                       const std::filesystem::path &filePath) {
    std::ofstream file(filePath);
    if (!file) {
        throw std::runtime_error("Impossible d'ouvrir le fichier de sauvegarde.");
    }
    file << buildReport(configurations, trace);
    if (!file) {
        throw std::runtime_error("Ecriture de la trace impossible.");
    }
    file.close();
    if (!file) {
        throw std::runtime_error("Ecriture de la trace impossible.");
    }
    output << "Trace sauvegardee.\n";
    output << "Fichier : " << filePath.string() << '\n';
}

} // namespace mss
