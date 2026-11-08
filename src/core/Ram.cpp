#include "core/Ram.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

namespace mss {

std::size_t Ram::checkedAddress(int address) {
    if (address < 0) {
        throw std::out_of_range("L'adresse RAM ne peut pas etre negative.");
    }
    return static_cast<std::size_t>(address);
}

std::size_t Ram::findContiguous(std::size_t count) const {
    std::size_t start = 0;
    std::size_t freeCount = 0;
    for (std::size_t i = 0; i < memory.size(); ++i) {
        if (!memory[i].has_value()) {
            if (freeCount == 0) start = i;
            if (++freeCount == count) return start;
        } else {
            freeCount = 0;
        }
    }
    // On peut utiliser les cases libres de fin de memoire, puis l'etendre.
    return freeCount == 0 ? memory.size() : start;
}

int Ram::load(Matrix matrix) {
    const std::size_t address = findContiguous(1);
    if (address > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::overflow_error("Adresse RAM trop grande.");
    }
    if (address == memory.size()) memory.emplace_back(std::move(matrix));
    else memory[address] = Cell(std::move(matrix));
    return static_cast<int>(address);
}

int Ram::load(Operation operation) {
    const std::size_t address = findContiguous(1);
    if (address > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::overflow_error("Adresse RAM trop grande.");
    }
    if (address == memory.size()) memory.emplace_back(std::move(operation));
    else memory[address] = Cell(std::move(operation));
    return static_cast<int>(address);
}

int Ram::load(OperationName type, const std::vector<int> &inputLocations) {
    std::vector<std::size_t> locations;
    locations.reserve(inputLocations.size());
    for (int address : inputLocations) locations.push_back(checkedAddress(address));
    return load(Operation{type, std::move(locations)});
}

int Ram::load(const std::vector<Matrix> &matrices) {
    if (matrices.empty()) throw std::invalid_argument("La collection de matrices est vide.");
    const std::size_t address = findContiguous(matrices.size());
    if (matrices.size() - 1 > static_cast<std::size_t>(std::numeric_limits<int>::max()) - address) {
        throw std::overflow_error("Adresse RAM trop grande.");
    }
    // Copier d'abord les donnees permet d'eviter une insertion partielle.
    std::vector<Matrix> copies = matrices;
    if (address + copies.size() > memory.size()) memory.resize(address + copies.size());
    for (std::size_t i = 0; i < copies.size(); ++i) {
        memory[address + i] = Cell(std::move(copies[i]));
    }
    return static_cast<int>(address);
}

std::size_t Ram::store(Matrix matrix) {
    return static_cast<std::size_t>(load(std::move(matrix)));
}

const Matrix &Ram::at(std::size_t address) const {
    if (address >= memory.size() || !memory[address]) {
        throw std::out_of_range("Adresse RAM vide ou inexistante.");
    }
    if (const auto *matrix = std::get_if<Matrix>(&*memory[address])) return *matrix;
    throw std::invalid_argument("L'adresse RAM contient une operation, pas une matrice.");
}

const Operation &Ram::operationAt(std::size_t address) const {
    if (address >= memory.size() || !memory[address]) {
        throw std::out_of_range("Adresse RAM vide ou inexistante.");
    }
    if (const auto *operation = std::get_if<Operation>(&*memory[address])) return *operation;
    throw std::invalid_argument("L'adresse RAM contient une matrice, pas une operation.");
}

bool Ram::occupied(std::size_t address) const noexcept {
    return address < memory.size() && memory[address].has_value();
}

std::size_t Ram::size() const noexcept {
    // Taille de l'espace d'adressage (les trous liberes sont conserves).
    return memory.size();
}

void Ram::remove(int address) {
    const std::size_t pos = checkedAddress(address);
    if (!occupied(pos)) throw std::out_of_range("Adresse RAM vide ou inexistante.");
    memory[pos].reset();
}

void Ram::set(int address, Matrix matrix) {
    const std::size_t pos = checkedAddress(address);
    if (occupied(pos)) throw std::logic_error("L'adresse RAM est deja occupee.");
    if (pos >= memory.size()) memory.resize(pos + 1);
    memory[pos] = Cell(std::move(matrix));
}

void Ram::set(int address, Operation operation) {
    const std::size_t pos = checkedAddress(address);
    if (occupied(pos)) throw std::logic_error("L'adresse RAM est deja occupee.");
    if (pos >= memory.size()) memory.resize(pos + 1);
    memory[pos] = Cell(std::move(operation));
}

void Ram::set(int address, OperationName type, const std::vector<int> &inputLocations) {
    std::vector<std::size_t> locations;
    locations.reserve(inputLocations.size());
    for (int input : inputLocations) locations.push_back(checkedAddress(input));
    set(address, Operation{type, std::move(locations)});
}

int Ram::create(const std::vector<int> &values) {
    if (values.empty()) throw std::invalid_argument("La matrice ne peut pas etre vide.");
    std::size_t side = 1;
    while (side < values.size() / side) ++side;
    if (side * side != values.size() || (side & (side - 1)) != 0) {
        throw std::invalid_argument("Il faut un nombre carre de valeurs (cote puissance de 2).");
    }
    std::vector<std::vector<double>> rows(side, std::vector<double>(side));
    for (std::size_t i = 0; i < values.size(); ++i) {
        rows[i / side][i % side] = static_cast<double>(values[i]);
    }
    return load(Matrix(std::move(rows)));
}

int Ram::split(int address) {
    const Matrix &matrix = at(checkedAddress(address));
    if (matrix.rows() != matrix.columns() || matrix.rows() < 2 || matrix.rows() % 2 != 0) {
        throw std::invalid_argument("split exige une matrice carree de taille paire >= 2.");
    }
    const std::size_t half = matrix.rows() / 2;
    std::vector<Matrix> quadrants;
    quadrants.reserve(4);
    for (std::size_t blockRow = 0; blockRow < 2; ++blockRow) {
        for (std::size_t blockCol = 0; blockCol < 2; ++blockCol) {
            std::vector<std::vector<double>> data(half, std::vector<double>(half));
            for (std::size_t r = 0; r < half; ++r) {
                for (std::size_t c = 0; c < half; ++c) {
                    data[r][c] = matrix.at(blockRow * half + r, blockCol * half + c);
                }
            }
            quadrants.emplace_back(std::move(data));
        }
    }
    // Charger les quatre sous-matrices avant de supprimer la matrice source.
    // En cas d'echec du chargement, la matrice originale reste disponible.
    const int firstAddress = load(quadrants);
    remove(address);
    return firstAddress;
}

int Ram::fuse(const std::vector<int> &addresses) {
    if (addresses.size() != 4) {
        throw std::invalid_argument("fuse exige exactement quatre adresses.");
    }
    std::vector<Matrix> blocks;
    blocks.reserve(4);
    for (int address : addresses) blocks.push_back(at(checkedAddress(address)));
    const std::size_t n = blocks.front().rows();
    if (n == 0 || n > std::numeric_limits<std::size_t>::max() / 2) {
        throw std::invalid_argument("Taille de sous-matrice invalide.");
    }
    for (const Matrix &block : blocks) {
        if (block.rows() != n || block.columns() != n) {
            throw std::invalid_argument("Les quatre blocs doivent etre carres de meme taille.");
        }
    }
    std::vector<std::vector<double>> data(2 * n, std::vector<double>(2 * n));
    for (std::size_t b = 0; b < 4; ++b) {
        for (std::size_t r = 0; r < n; ++r) {
            for (std::size_t c = 0; c < n; ++c) {
                data[(b / 2) * n + r][(b % 2) * n + c] = blocks[b].at(r, c);
            }
        }
    }
    return load(Matrix(std::move(data)));
}

std::vector<int> Ram::reserve(std::size_t count) {
    if (count > static_cast<std::size_t>(std::numeric_limits<int>::max()) - memory.size()) {
        throw std::overflow_error("Espace d'adressage RAM depasse.");
    }
    const std::size_t first = memory.size();
    memory.resize(first + count);
    std::vector<int> addresses;
    addresses.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        memory[first + i] = Cell(Pending{});
        addresses.push_back(static_cast<int>(first + i));
    }
    return addresses;
}

bool Ram::isReady(std::size_t address) const {
    if (address >= memory.size()) throw std::out_of_range("Adresse RAM inexistante.");
    return memory[address] && std::holds_alternative<Matrix>(*memory[address]);
}

void Ram::storeResult(int address, Matrix result) {
    const auto pos = checkedAddress(address);
    if (pos >= memory.size() || !memory[pos] ||
        !std::holds_alternative<Pending>(*memory[pos])) {
        throw std::logic_error("Le resultat doit etre ecrit dans une adresse reservee.");
    }
    memory[pos] = Cell(std::move(result));
}

std::vector<Matrix> Ram::splitMatrix(int address) const {
    const Matrix &matrix = at(checkedAddress(address));
    if (matrix.rows() != matrix.columns() || matrix.rows() < 2 ||
        (matrix.rows() & (matrix.rows() - 1)) != 0) {
        throw std::invalid_argument("splitMatrix exige une matrice carree de taille 2^n >= 2.");
    }
    const std::size_t half = matrix.rows() / 2;
    std::vector<Matrix> blocks;
    blocks.reserve(4);
    for (std::size_t br = 0; br < 2; ++br) {
        for (std::size_t bc = 0; bc < 2; ++bc) {
            std::vector<std::vector<double>> data(half, std::vector<double>(half));
            for (std::size_t r = 0; r < half; ++r)
                for (std::size_t c = 0; c < half; ++c)
                    data[r][c] = matrix.at(br * half + r, bc * half + c);
            blocks.emplace_back(std::move(data));
        }
    }
    return blocks;
}

Matrix Ram::fuseMatrices(const std::vector<int> &addresses) const {
    if (addresses.size() != 4) throw std::invalid_argument("fuseMatrices exige 4 blocs.");
    std::vector<Matrix> blocks;
    for (int a : addresses) blocks.push_back(at(checkedAddress(a)));
    const std::size_t n = blocks.front().rows();
    if (n == 0 || n > std::numeric_limits<std::size_t>::max() / 2)
        throw std::invalid_argument("Dimension invalide.");
    for (const Matrix &b : blocks)
        if (b.rows() != n || b.columns() != n)
            throw std::invalid_argument("Blocs de dimensions incompatibles.");
    std::vector<std::vector<double>> result(2 * n, std::vector<double>(2 * n));
    for (std::size_t i = 0; i < 4; ++i)
        for (std::size_t r = 0; r < n; ++r)
            for (std::size_t c = 0; c < n; ++c)
                result[(i / 2) * n + r][(i % 2) * n + c] = blocks[i].at(r, c);
    return Matrix(std::move(result));
}

int Ram::getOperationCost(Operation &operation) const {
    (void)operation;
    throw std::logic_error("Le modele de cout RAM n'a pas encore ete specifie.");
}

} // namespace mss
