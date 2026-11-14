#pragma once

#include "core/Cpu.hpp"
#include "core/Matrix.hpp"
#include "core/Ram.hpp"

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>


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
     * @brief Retourne la trace des opérations exécutées.
     */
    const std::vector<OperationName> &executionTrace() const noexcept;

    /**
     * @brief Retourne le processeur associé au système.
     * @return Processeur du système.
     */
    const Cpu &cpu() const noexcept;

    /**
     * @brief Retourne la RAM du système.
     * @return Référence modifiable à la RAM.
     */
    Ram &getRam() noexcept;

    /**
     * @brief execute l'operation passée en argument avec les adresses mémoires données
     * @return void
     */
    void execute(int); 

    /**
     * @brief appelle execute dans que la pile n'est pas vide
     * @return void
     */
    void run();

    // Charger une matrice dans la RAM du simulateur.
    int loadMatrix(Matrix matrix);
    // Enregistre une instruction et reserve ses adresses de sortie.
    // Split : 4, Print : 0, les autres operations supportees : 1.
    std::vector<int> loadOperation(OperationName type, const std::vector<int> &inputs,
                                   const std::vector<int> &outputs = {});
    const Matrix &resultAt(int address) const;
    std::size_t pendingOperations() const noexcept;
    // Nombre de cases occupees; les matrices temporaires sont liberees apres usage.
    std::size_t occupiedMemory() const noexcept;
    // Vide RAM, file et trace pour recommencer une simulation.
    void clear() noexcept;

    /**
     * @brief retourne un version écrite de l'historique des operations effecutées
     * @return string
     */

    //string returnTrace(Config& config);

  private:

    std::deque<int> queue;
    int nextReady() const;
    void expand(const Operation &operation);
    void expandDeterminant(const Operation &operation);
    std::vector<int> loadInternalOperation(OperationName type, const std::vector<int> &inputs,
                                           const std::vector<int> &outputs = {});
    void finishOperation(const Operation &operation);
    void cleanupUnused();
    std::unordered_set<int> temporaryAddresses;
    std::unordered_map<int, std::size_t> inputReferences;
    std::vector<OperationName> trace; // Taille du tableau à spécifier

    //Configuration configuration;
    Cpu cpuInstance;
    Ram ram;
};

} // namespace mss
