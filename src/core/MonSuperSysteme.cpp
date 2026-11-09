#include "core/MonSuperSysteme.hpp"

#include <iostream>
#include <stdexcept>
#include <utility>

namespace mss {

void MonSuperSysteme::loadInitialMatrix(Matrix matrix) {
    if (ram.size() != 0) throw std::logic_error("La matrice initiale a deja ete chargee.");
    ram.store(std::move(matrix));
}

const Matrix &MonSuperSysteme::initialMatrix() const {
    if (ram.size() == 0) throw std::logic_error("Aucune matrice initiale n'a ete chargee.");
    return ram.at(0);
}

const std::vector<OperationName> &MonSuperSysteme::executionTrace() const noexcept { return trace; }
const Cpu &MonSuperSysteme::cpu() const noexcept { return cpuInstance; }
Ram &MonSuperSysteme::getRam() noexcept { return ram; }
int MonSuperSysteme::loadMatrix(Matrix matrix) { return ram.load(std::move(matrix)); }
const Matrix &MonSuperSysteme::resultAt(int address) const {
    if (address < 0) throw std::out_of_range("Adresse negative.");
    return ram.at(static_cast<std::size_t>(address));
}
std::size_t MonSuperSysteme::pendingOperations() const noexcept { return queue.size(); }

std::vector<int> MonSuperSysteme::loadOperation(OperationName type,
        const std::vector<int> &inputs, const std::vector<int> &outputs) {
    std::size_t inputCount = 0;
    std::size_t outputCount = 1;
    switch (type) {
        case OperationName::Add:
        case OperationName::Multiply: inputCount = 2; break;
        case OperationName::Split: inputCount = 1; outputCount = 4; break;
        case OperationName::Fuse: inputCount = 4; break;
        case OperationName::Print: inputCount = 1; outputCount = 0; break;
        default: throw std::invalid_argument("Operation non prise en charge par ce moteur.");
    }
    if (inputs.size() != inputCount) throw std::invalid_argument("Nombre d'entrees incorrect.");
    for (int address : inputs)
        if (address < 0 || static_cast<std::size_t>(address) >= ram.size() ||
            !ram.occupied(static_cast<std::size_t>(address)))
            throw std::out_of_range("Entree RAM invalide.");
    if (!outputs.empty() && outputs.size() != outputCount)
        throw std::invalid_argument("Nombre de sorties incorrect.");
    for (int address : outputs)
        if (address < 0 || static_cast<std::size_t>(address) >= ram.size() ||
            !ram.occupied(static_cast<std::size_t>(address)))
            throw std::out_of_range("Sortie RAM non reservee.");
    std::vector<int> resultAddresses = outputs.empty() ? ram.reserve(outputCount) : outputs;
    // Stockage de l'instruction (ne doit pas remplacer une adresse reservee).
    std::vector<std::size_t> ins, outs;
    for (int i : inputs) ins.push_back(static_cast<std::size_t>(i));
    for (int i : resultAddresses) outs.push_back(static_cast<std::size_t>(i));
    const int opAddress = ram.load(Operation{type, std::move(ins), std::move(outs)});
    queue.push_back(opAddress);
    return resultAddresses;
}

int MonSuperSysteme::nextReady() const {
    for (int address : queue) {
        const Operation &op = ram.operationAt(static_cast<std::size_t>(address));
        bool ready = true;
        for (std::size_t input : op.inputLocations)
            if (!ram.isReady(input)) { ready = false; break; }
        if (ready) return address;
    }
    throw std::runtime_error("Interblocage : aucune operation n'a ses entrees disponibles.");
}

void MonSuperSysteme::expand(const Operation &op) {
    // Les deux sources sont conservees : leurs sous-matrices peuvent etre partagees.
    const int a = static_cast<int>(op.inputLocations.at(0));
    const int b = static_cast<int>(op.inputLocations.at(1));
    const auto A = loadOperation(OperationName::Split, {a});
    const auto B = loadOperation(OperationName::Split, {b});
    std::vector<int> C;
    C.reserve(4);
    if (op.type == OperationName::Add) {
        for (std::size_t i = 0; i < 4; ++i)
            C.push_back(loadOperation(OperationName::Add, {A[i], B[i]}).at(0));
    } else {
        for (const auto &pair : std::vector<std::vector<int>>{
                 {0, 0, 1, 2}, {0, 1, 1, 3}, {2, 0, 3, 2}, {2, 1, 3, 3}}) {
            const int p1 = loadOperation(OperationName::Multiply,
                                         {A[static_cast<std::size_t>(pair[0])], B[static_cast<std::size_t>(pair[1])]}).at(0);
            const int p2 = loadOperation(OperationName::Multiply,
                                         {A[static_cast<std::size_t>(pair[2])], B[static_cast<std::size_t>(pair[3])]}).at(0);
            C.push_back(loadOperation(OperationName::Add, {p1, p2}).at(0));
        }
    }
    loadOperation(OperationName::Fuse, C, {static_cast<int>(op.outputLocations.at(0))});
}

void MonSuperSysteme::execute(int memoryAddress) {
    if (memoryAddress < 0) throw std::out_of_range("Adresse negative.");

    const Operation op = ram.operationAt(static_cast<std::size_t>(memoryAddress));

    switch (op.type) {
        case OperationName::Split: {
            const int inputAddress = static_cast<int>(op.inputLocations.at(0));
            auto quadrants = ram.splitMatrix(inputAddress);
            for (std::size_t i = 0; i < 4; ++i)
                ram.storeResult(static_cast<int>(op.outputLocations.at(i)),
                                std::move(quadrants[i]));
            break;
        }
        case OperationName::Fuse: {
            std::vector<int> inputAddresses;
            inputAddresses.reserve(op.inputLocations.size());
            for (std::size_t address : op.inputLocations)
                inputAddresses.push_back(static_cast<int>(address));

            Matrix result = ram.fuseMatrices(inputAddresses);
            result.printMatrix();
            ram.storeResult(static_cast<int>(op.outputLocations.at(0)), std::move(result));
            break;
        }
        case OperationName::Print: {
            //const std::size_t inputAddress = op.inputLocations.at(0);
            //const Matrix &matrix = ram.at(inputAddress);
            break;
        }
        case OperationName::Add:
        case OperationName::Multiply: {
            Matrix first = ram.at(op.inputLocations.at(0));
            Matrix second = ram.at(op.inputLocations.at(1));
            if (first.rows() != first.columns() || second.rows() != second.columns() ||
                first.rows() != second.rows() || (first.rows() & (first.rows() - 1)) != 0) {
                throw std::invalid_argument("Matrices carrees de taille 2^n et dimensions identiques requises.");
            }
            if (first.rows() > 2) {
                expand(op);
                ram.remove(memoryAddress);
                return; // La decomposition n'est pas une instruction de calcul executee.
            }
            if (first.rows() != 2) {
                throw std::invalid_argument("CPU exige une matrice 2x2.");
            }

            Matrix result = op.type == OperationName::Add
                                ? cpuInstance.add(first, second)
                                : cpuInstance.multiply(first, second);

            const int outputAddress = static_cast<int>(op.outputLocations.at(0));
            std::cout << "Adresse de la sortie : " << outputAddress << std::endl;
            result.printMatrix();
            ram.storeResult(outputAddress, std::move(result));
            break;
        }
        default: throw std::invalid_argument("Operation non prise en charge.");
    }
    trace.push_back(op.type);
    ram.remove(memoryAddress);
}

void MonSuperSysteme::run() {
    while (!queue.empty()) {
        const int address = nextReady();
        for (auto it = queue.begin(); it != queue.end(); ++it) {
            if (*it == address) { queue.erase(it); break; }
        }
        execute(address);
    }
}

} // namespace mss
