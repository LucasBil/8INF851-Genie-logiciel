#pragma once

namespace mss {

/**
 * @brief Noms des opérations MSS
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
    CalcDeterminant,
    Print // Extension: affichage d'un resultat pret.
};

} // namespace mss
