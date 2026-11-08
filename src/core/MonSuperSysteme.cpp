#include "core/MonSuperSysteme.hpp"

#include <stdexcept>
#include <utility>

namespace mss {

void MonSuperSysteme::loadInitialMatrix(Matrix matrix) {
    if (ram.size() != 0) {
        throw std::logic_error("La matrice initiale a deja ete chargee.");
    }
    ram.store(std::move(matrix));
}

const Matrix &MonSuperSysteme::initialMatrix() const {
    if (ram.size() == 0) {
        throw std::logic_error("Aucune matrice initiale n'a ete chargee.");
    }
    return ram.at(0);
}

const std::vector<OperationName> &MonSuperSysteme::executionTrace() const noexcept {
    return trace;
}

const Cpu &MonSuperSysteme::cpu() const noexcept {
    return cpuInstance;
}

Ram &MonSuperSysteme::getRam() noexcept {
    return ram;
}

std::stack<int> &MonSuperSysteme::getStack() noexcept {
    return stack;
}

void MonSuperSysteme::execute(int operationAddress) {
    std::cout << "test";
    if (operationAddress < 0) {
        throw std::out_of_range("L'adresse de l'operation ne peut pas etre negative.");
    }

    const Operation &operation = ram.operationAt(static_cast<std::size_t>(operationAddress));
    switch (operation.type) {
        case OperationName::Create:
            // TODO: creer une matrice a partir des entrees.
            break;
        case OperationName::Delete:
            // TODO: supprimer les adresses visees.
            break;
        case OperationName::Split:
            // TODO: separer la matrice d'entree et stocker les resultats.
            break;
        case OperationName::Fuse:
            // TODO: fusionner les matrices d'entree et stocker le resultat.
            break;
        case OperationName::Load:
            // TODO: charger les donnees demandees.
            break;
        case OperationName::Unload:
            // TODO: decharger les donnees demandees.
            break;
        case OperationName::Permutations:
            // TODO: appliquer la permutation demandee.
            break;
        case OperationName::FormuleColumn:
            // TODO: appliquer la formule a la colonne visee.
            break;
        case OperationName::Rotate:
            // TODO: faire traiter la matrice par le CPU.
            break;
        case OperationName::Add:
            // TODO: faire traiter les matrices par le CPU.
            break;
        case OperationName::Multiply:
            // TODO: faire traiter les matrices par le CPU.
            break;
        case OperationName::CalcDeterminant:
            // TODO: calculer le determinant avec le CPU.
            break;
    }

    // TODO: ajouter l'operation a la trace apres son execution reussie.
    // TODO: supprimer l'operation consommee et empiler les operations suivantes.
}

void MonSuperSysteme::run() {
    std::cout << "test";
    while (!stack.empty()) {
        int operation = stack.top();
        stack.pop();
        execute(operation);
    }
}


} // namespace mss
