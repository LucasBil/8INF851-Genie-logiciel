#pragma once

#include "core/Cpu.hpp"
#include "core/Matrix.hpp"
#include "core/Ram.hpp"

#include <iostream>
#include <stack>
#include <array>


namespace mss {

/**
 * @brief Façade du moteur MSS, initialisée avec une matrice à l'adresse zéro.
 *
 * @note L'exécution de la pile attend la confirmation des règles d'opération.
 */
class MonSuperSysteme {
  public:
    /**
     * @brief Place la matrice d'entrée à la première adresse de la RAM.
     * @param matrix Matrice initiale de la simulation.
     * @throws std::logic_error si une matrice initiale a déjà été chargée.
     */
    void loadInitialMatrix(Matrix matrix);

    /**
     * @brief Retourne la matrice initiale à l'adresse zéro.
     * @return Matrice chargée à l'adresse mémoire zéro.
     * @throws std::logic_error si aucune matrice n'a encore été chargée.
     */
    const Matrix &initialMatrix() const;

    /**
     * @brief Retourne le processeur associé au système.
     * @return Processeur du système.
     */
    const Cpu &cpu() const noexcept;

    /**
     * @brief execute l'operation passée en argument avec les adresses mémoires données
     * @return void
     */
    void execute(Operation& operation); 

    /**
     * @brief appelle execute dans que la pile n'est pas vide
     * @return void
     */
    void run();

    /**
     * @brief retourne un version écrite de l'historique des operations effecutées
     * @return string
     */

    //string returnTrace(Config& config);

  private:

    std::stack<Operation> stack;
    std::array<Operation, 100> trace; // Taille du tableau à spécifier

    Cpu cpuInstance;
    Ram ram;
};

} // namespace mss
