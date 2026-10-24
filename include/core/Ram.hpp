#pragma once

#include "core/Matrix.hpp"
#include "core/Operation.hpp"
#include "core/AddressSlot.hpp"

#include <cstddef>
#include <vector>

namespace mss {

/**
 * @brief Mémoire virtuelle contenant les matrices de la simulation.
 */
class Ram {
  public: 
    /**
     * @brief Ajoute une matrice et retourne son adresse mémoire.
     * @param matrix Matrice à stocker.
     * @return Adresse mémoire attribuée.
     */
    std::size_t store(Matrix matrix);

    /**
     * @brief Accède à une matrice stockée.
     * @param address Adresse de la matrice.
     * @return Matrice à l'adresse donnée.
     * @throws std::out_of_range si l'adresse est invalide.
     */
    const Matrix &at(std::size_t address) const;

    /**
     * @brief Retourne le nombre de matrices stockées.
     * @return Nombre de matrices en mémoire.
     */
    std::size_t size() const noexcept;

    /**
     * @brief Signale que le modèle de coût RAM doit encore être défini.
     * @param operation Opération dont le coût est demandé.
     * @return Coût RAM.
     * @throws std::logic_error tant que la règle de coût n'est pas spécifiée.
     */
    int getOperationCost(Operation& operation) const;

    /**
     * @brief  prend en entrée un ensemble d’entiers, crée une matrice et la range en mémoire
     * @param vector de values de la matrice a creer
     * @return renvoie son adresse
     */
    int create(std::vector<int>& values);
    std::vector<int>& split(int address);
    int fuse(std::vector<int>& addresses);
    int load(Matrix);
    int load(std::vector<Matrix>& matrixList);
    int load(Operation operation);
    void set(int value, Matrix);
    void set(int value, Operation);


  private:
    std::vector<Matrix> memory;
};

} // namespace mss
