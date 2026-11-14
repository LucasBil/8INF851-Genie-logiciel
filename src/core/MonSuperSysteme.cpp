#include "core/MonSuperSysteme.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>

namespace mss {
namespace {
int addressAsInt(std::size_t address) {
    if (address > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        throw std::overflow_error("Adresse RAM trop grande.");
    return static_cast<int>(address);
}

bool validDimension(const Matrix &m) {
    const std::size_t n = m.rows();
    return n >= 1 && n == m.columns() && (n & (n - 1)) == 0;
}

Matrix scalar(double value) { return Matrix({{value}}); }
} // namespace

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
std::size_t MonSuperSysteme::occupiedMemory() const noexcept { return ram.occupiedCount(); }
void MonSuperSysteme::clear() noexcept {
    queue.clear(); trace.clear(); temporaryAddresses.clear(); inputReferences.clear(); ram.clear();
}

std::vector<int> MonSuperSysteme::loadOperation(OperationName type,
        const std::vector<int> &inputs, const std::vector<int> &outputs) {
    std::size_t inputCount = 0;
    std::size_t outputCount = 1;
    switch (type) {
        case OperationName::Add:
        case OperationName::Multiply: inputCount = 2; break;
        case OperationName::Rotate:
        case OperationName::Mirror:
        case OperationName::CalcDeterminant: inputCount = 1; break;
        case OperationName::Split: inputCount = 1; outputCount = 4; break;
        case OperationName::Fuse: inputCount = 4; break;
        case OperationName::Print: inputCount = 1; outputCount = 0; break;
        case OperationName::Clean: inputCount = 0; outputCount = 0; break;
        case OperationName::Look: inputCount = 1; outputCount = 0; break;
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
            !ram.occupied(static_cast<std::size_t>(address)) || ram.isReady(static_cast<std::size_t>(address)))
            throw std::out_of_range("Sortie RAM non reservee ou deja remplie.");
    std::vector<int> resultAddresses = outputs.empty() ? ram.reserve(outputCount) : outputs;
    std::vector<std::size_t> ins, outs;
    for (int i : inputs) ins.push_back(static_cast<std::size_t>(i));
    for (int i : resultAddresses) outs.push_back(static_cast<std::size_t>(i));
    const int opAddress = ram.load(Operation{type, std::move(ins), std::move(outs)});
    queue.push_back(opAddress);
    for (int input : inputs) ++inputReferences[input];
    return resultAddresses;
}

std::vector<int> MonSuperSysteme::loadInternalOperation(OperationName type,
        const std::vector<int> &inputs, const std::vector<int> &outputs) {
    const auto out = loadOperation(type, inputs, outputs);
    if (outputs.empty()) for (int address : out) temporaryAddresses.insert(address);
    return out;
}

int MonSuperSysteme::nextReady() const {
    // Clean est une barriere de nettoyage : les operations de calcul (y compris
    // celles ajoutees par decomposition recursive) sont terminees en premier.
    // On conserve une instruction Clean en attente tant qu'il reste du travail.
    for (int address : queue) {
        const Operation &op = ram.operationAt(static_cast<std::size_t>(address));
        if (op.type == OperationName::Clean) continue;
        // Look inspecte meme une reservation Pending : pas besoin d'un resultat pret.
        if (op.type == OperationName::Look) return address;
        bool ready = true;
        for (std::size_t input : op.inputLocations)
            if (!ram.isReady(input)) { ready = false; break; }
        if (ready) return address;
    }
    for (int address : queue) {
        if (ram.operationAt(static_cast<std::size_t>(address)).type == OperationName::Clean) {
            // Une operation encore en attente de ses donnees serait un interblocage.
            const bool hasWork = std::any_of(queue.begin(), queue.end(), [&](int candidate) {
                return ram.operationAt(static_cast<std::size_t>(candidate)).type != OperationName::Clean;
            });
            if (!hasWork) return address;
            break;
        }
    }
    throw std::runtime_error("Interblocage : aucune operation n'a ses entrees disponibles.");
}

void MonSuperSysteme::expand(const Operation &op) {
    const int source = addressAsInt(op.inputLocations.at(0));
    const int output = addressAsInt(op.outputLocations.at(0));
    if (op.type == OperationName::Rotate || op.type == OperationName::Mirror) {
        const auto blocks = loadInternalOperation(OperationName::Split, {source});
        std::vector<int> rotated;
        rotated.reserve(4);
        for (int block : blocks)
            rotated.push_back(loadInternalOperation(op.type, {block}).at(0));
        const std::vector<int> reordered = op.type == OperationName::Rotate
            ? std::vector<int>{rotated[2], rotated[0], rotated[3], rotated[1]}
            : std::vector<int>{rotated[1], rotated[0], rotated[3], rotated[2]};
        loadInternalOperation(OperationName::Fuse, reordered, {output});
        return;
    }

    const int second = addressAsInt(op.inputLocations.at(1));
    const auto A = loadInternalOperation(OperationName::Split, {source});
    const auto B = loadInternalOperation(OperationName::Split, {second});
    std::vector<int> C;
    C.reserve(4);
    if (op.type == OperationName::Add) {
        for (std::size_t i = 0; i < 4; ++i)
            C.push_back(loadInternalOperation(OperationName::Add, {A[i], B[i]}).at(0));
    } else {
        for (const auto &pair : std::vector<std::vector<int>>{
                 {0, 0, 1, 2}, {0, 1, 1, 3}, {2, 0, 3, 2}, {2, 1, 3, 3}}) {
            const int p1 = loadInternalOperation(OperationName::Multiply,
                {A[static_cast<std::size_t>(pair[0])], B[static_cast<std::size_t>(pair[1])]}).at(0);
            const int p2 = loadInternalOperation(OperationName::Multiply,
                {A[static_cast<std::size_t>(pair[2])], B[static_cast<std::size_t>(pair[3])]}).at(0);
            C.push_back(loadInternalOperation(OperationName::Add, {p1, p2}).at(0));
        }
    }
    loadInternalOperation(OperationName::Fuse, C, {output});
}

void MonSuperSysteme::expandDeterminant(const Operation &op) {
    const int source = addressAsInt(op.inputLocations.at(0));
    const Matrix &matrix = ram.at(op.inputLocations.at(0));
    const int n = static_cast<int>(matrix.rows());
    const int h = n / 2;
    int accumulated = -1;
    std::vector<int> selected;
    selected.reserve(static_cast<std::size_t>(h));

    // Developpement de Laplace par groupes de h colonnes, tel que decrit dans le cahier.
    std::function<void(int)> choose = [&](int next) {
        if (static_cast<int>(selected.size()) == h) {
            std::vector<int> columns = selected;
            for (int c = 0; c < n; ++c)
                if (std::find(selected.begin(), selected.end(), c) == selected.end())
                    columns.push_back(c);
            // Une copie permutee est ajoutee dans la RAM; la matrice initiale reste intacte.
            const int rearranged = ram.load(ram.reorderColumns(source, columns));
            temporaryAddresses.insert(rearranged);
            const auto blocks = loadInternalOperation(OperationName::Split, {rearranged});
            const int upperDet = loadInternalOperation(OperationName::CalcDeterminant, {blocks[0]}).at(0);
            const int lowerDet = loadInternalOperation(OperationName::CalcDeterminant, {blocks[3]}).at(0);
            int term = loadInternalOperation(OperationName::Multiply, {upperDet, lowerDet}).at(0);
            int sum = 0;
            for (int c : selected) sum += c + 1; // indices de colonnes 1-based
            const int exponent = sum - h * (h + 1) / 2;
            if (exponent % 2 != 0) {
                const int minusOne = ram.load(scalar(-1));
                temporaryAddresses.insert(minusOne);
                term = loadInternalOperation(OperationName::Multiply, {term, minusOne}).at(0);
            }
            if (accumulated == -1) accumulated = term;
            else accumulated = loadInternalOperation(OperationName::Add, {accumulated, term}).at(0);
            return;
        }
        const int remaining = h - static_cast<int>(selected.size());
        for (int c = next; c <= n - remaining; ++c) {
            selected.push_back(c);
            choose(c + 1);
            selected.pop_back();
        }
    };
    choose(0);
    if (accumulated < 0) throw std::logic_error("Developpement du determinant vide.");
    // L'adresse de sortie appartient a l'appelant et ne doit pas etre liberee.
    // Une copie 1x1 vers cette adresse est effectuee lors d'une operation interne.
    // Utiliser une addition avec zero pour conserver l'ordonnancement des dependances.
    const int zero = ram.load(scalar(0));
    temporaryAddresses.insert(zero);
    loadInternalOperation(OperationName::Add, {accumulated, zero},
                          {addressAsInt(op.outputLocations.at(0))});
}

void MonSuperSysteme::finishOperation(const Operation &op) {
    for (std::size_t input : op.inputLocations) {
        const int key = addressAsInt(input);
        auto it = inputReferences.find(key);
        if (it == inputReferences.end()) throw std::logic_error("Compteur de references RAM incoherent.");
        if (--it->second == 0) inputReferences.erase(it);
    }
    cleanupUnused();
}

void MonSuperSysteme::cleanupUnused() {
    for (auto it = temporaryAddresses.begin(); it != temporaryAddresses.end();) {
        const int address = *it;
        if (inputReferences.find(address) == inputReferences.end() &&
            ram.isReady(static_cast<std::size_t>(address))) {
            if (ram.occupied(static_cast<std::size_t>(address))) ram.remove(address);
            it = temporaryAddresses.erase(it);
        } else ++it;
    }
}

void MonSuperSysteme::execute(int memoryAddress) {
    if (memoryAddress < 0) throw std::out_of_range("Adresse negative.");
    const Operation op = ram.operationAt(static_cast<std::size_t>(memoryAddress));
    bool expanded = false;
    switch (op.type) {
        case OperationName::Split: {
            const auto quadrants = ram.splitMatrix(addressAsInt(op.inputLocations.at(0)));
            for (std::size_t i = 0; i < 4; ++i)
                ram.storeResult(addressAsInt(op.outputLocations.at(i)), quadrants[i]);
            break;
        }
        case OperationName::Fuse: {
            std::vector<int> inputs;
            for (std::size_t a : op.inputLocations) inputs.push_back(addressAsInt(a));
            ram.storeResult(addressAsInt(op.outputLocations.at(0)), ram.fuseMatrices(inputs));
            break;
        }
        case OperationName::Print:
            // L'affichage appartient au controleur de sortie.
            break;
        case OperationName::Look:
            ram.look(addressAsInt(op.inputLocations.at(0)));
            break;
        case OperationName::Clean:
            // Supprime uniquement les temporaires sans references encore utiles.
            // Les matrices d'entree, resultats publics et sorties Pending sont preserves.
            cleanupUnused();
            break;
        case OperationName::Rotate:
        case OperationName::Mirror: {
            Matrix first = ram.at(op.inputLocations.at(0));
            if (!validDimension(first)) throw std::invalid_argument("Matrice carree 2^n requise.");
            if (first.rows() > 2) { expand(op); expanded = true; break; }
            if (first.rows() == 2) {
                if (op.type == OperationName::Rotate) cpuInstance.rotate(first);
                else cpuInstance.permuteColumn(first);
            } else if (op.type == OperationName::Rotate || op.type == OperationName::Mirror) {
                // Une matrice 1x1 est invariante par rotation et miroir.
            }
            ram.storeResult(addressAsInt(op.outputLocations.at(0)), std::move(first));
            break;
        }
        case OperationName::CalcDeterminant: {
            Matrix first = ram.at(op.inputLocations.at(0));
            if (!validDimension(first)) throw std::invalid_argument("Determinant : matrice carree 2^n requise.");
            if (first.rows() > 2) { expandDeterminant(op); expanded = true; break; }
            if (first.rows() == 2) cpuInstance.calcDeterminant(first);
            ram.storeResult(addressAsInt(op.outputLocations.at(0)), std::move(first));
            break;
        }
        case OperationName::Add:
        case OperationName::Multiply: {
            Matrix first = ram.at(op.inputLocations.at(0));
            Matrix second = ram.at(op.inputLocations.at(1));
            if (!validDimension(first) || !validDimension(second) || first.rows() != second.rows())
                throw std::invalid_argument("Matrices carrees 2^n de dimensions identiques requises.");
            if (first.rows() > 2) { expand(op); expanded = true; break; }
            Matrix result = first;
            if (first.rows() == 1) {
                const double a = first.at(0, 0), b = second.at(0, 0);
                result = scalar(op.type == OperationName::Add ? a + b : a * b);
            } else if (op.type == OperationName::Add) result = cpuInstance.add(first, second);
            else result = cpuInstance.multiply(first, second);
            ram.storeResult(addressAsInt(op.outputLocations.at(0)), std::move(result));
            break;
        }
        default: throw std::invalid_argument("Operation non prise en charge.");
    }
    if (!expanded) trace.push_back(op.type);
    ram.remove(memoryAddress);
    finishOperation(op);
}

void MonSuperSysteme::run() {
    while (!queue.empty()) {
        const int address = nextReady();
        const auto it = std::find(queue.begin(), queue.end(), address);
        queue.erase(it);
        execute(address);
    }
    cleanupUnused();
}

} // namespace mss
