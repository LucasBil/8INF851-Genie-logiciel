#pragma once

#include "core/Matrix.hpp"
#include "core/Operation.hpp"
#include "core/AddressSlot.hpp"

#include <cstddef>
#include <optional>
#include <variant>
#include <vector>

namespace mss {

/** Mémoire virtuelle: une adresse contient une matrice ou une opération. */
class Ram {
  public:
    // API du prototype conservée pour MonSuperSysteme et les tests existants.
    std::size_t store(Matrix matrix);
    const Matrix &at(std::size_t address) const;
    std::size_t size() const noexcept;
    int getOperationCost(Operation &operation) const;

    // Opérations décrites dans la section 2.3.3 du cahier de conception.
    int create(const std::vector<int> &values);
    void remove(int address); // 'delete' est un mot réservé en C++.
    int split(int address);
    int fuse(const std::vector<int> &addresses);
    int load(Matrix matrix);
    int load(const std::vector<Matrix> &matrices);
    int load(Operation operation);
    int load(OperationName type, const std::vector<int> &inputLocations);
    void set(int address, Matrix matrix);
    void set(int address, Operation operation);
    void set(int address, OperationName type, const std::vector<int> &inputLocations);

    const Operation &operationAt(std::size_t address) const;
    bool occupied(std::size_t address) const noexcept;

  private:
    using Cell = std::variant<Matrix, Operation>;
    std::vector<std::optional<Cell>> memory;

    std::size_t findContiguous(std::size_t count) const;
    static std::size_t checkedAddress(int address);
};

} // namespace mss
