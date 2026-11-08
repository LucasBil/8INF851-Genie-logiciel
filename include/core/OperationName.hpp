#pragma once

namespace mss {

/**
 * @brief Noms des opérations MSS
 */
enum class OperationName {
    /// Création d'une matrice.
    Create,
    /// Suppression d'une matrice.
    Delete,
    /// Séparation d'une matrice.
    Split,
    /// Fusion de matrices.
    Fuse,
    /// Chargement d'une matrice ou d'une opération.
    Load,
    /// Déchargement d'une matrice ou d'une opération.
    Unload,
    /// Permutation d'éléments.
    Permutations,
    /// Application d'une formule sur une colonne.
    FormuleColumn,
    /// Rotation d'une matrice.
    Rotate,
    /// Addition de matrices.
    Add,
    /// Multiplication de matrices.
    Multiply,
    /// Calcul du déterminant.
    CalcDeterminant
};

} // namespace mss
