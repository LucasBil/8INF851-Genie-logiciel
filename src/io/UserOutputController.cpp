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
    }
    throw std::logic_error("Nom d'operation inconnu.");
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
        report << index + 1 << ". " << operationName(trace[index]) << '\n';
    }

    for (std::size_t configurationIndex = 0; configurationIndex < configurations.size();
         ++configurationIndex) {
        const Configuration &configuration = configurations[configurationIndex];
        long long total = 0;
        std::vector<int> operationCosts;
        operationCosts.reserve(trace.size());
        for (const OperationName operation : trace) {
            const int cost = configuration.get(operation);
            operationCosts.push_back(cost);
            total += cost;
        }
        totals.push_back(total);
        report << "Configuration " << configurationIndex + 1 << " : " << total << '\n';
        for (std::size_t operationIndex = 0; operationIndex < trace.size(); ++operationIndex) {
            report << "  " << operationIndex + 1 << ". "
                   << operationName(trace[operationIndex]) << " : "
                   << operationCosts[operationIndex] << '\n';
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

    std::ofstream file(fileName);
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
}

} // namespace mss
