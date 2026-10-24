#include "core/MonSuperSysteme.hpp"

#include <stdexcept>
#include <utility>

namespace mss {

void MonSuperSysteme::loadInitialMatrix(Matrix matrix) {
    if (ram.size() != 0) {
        throw std::logic_error("La matrice initiale a déjà été chargée.");
    }
    ram.store(std::move(matrix));
}

const Matrix &MonSuperSysteme::initialMatrix() const {
    if (ram.size() == 0) {
        throw std::logic_error("Aucune matrice initiale n'a été chargée.");
    }
    return ram.at(0);
}

const Cpu &MonSuperSysteme::cpu() const noexcept {
    return cpuInstance;
}

void MonSuperSysteme::execute(int){
    // 1. Lire le type de l'opération.
    // 2. Lire les adresses mémoire d'entrée.
    // 3. Exécuter l'opération (Create, Add, Multiply, etc.).
    // 4. Ajouter l'opération exécutée à trace.
    // 5. Ajouter de nouvelles opérations à stack si nécessaire.
}

void MonSuperSysteme::run() {
    while (!stack.empty()) {
        int operation = stack.top();
        stack.pop();
        execute(operation);
    }
}


} // namespace mss
