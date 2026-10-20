#pragma once

namespace mss {

/**
 * @brief Noms des opérations MSS proposées par le cahier de conception.
 */
enum class OperationName {
    Create,
    Delete,
    Split,
    Fuse,
    Load,
    Unload,
    Permutations,
    FormuleColumn,
    Rotate,
    Add,
    Multiply,
    CalcDeterminant
};

} // namespace mss
