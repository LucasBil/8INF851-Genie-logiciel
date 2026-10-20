#pragma once

#include "core/OperationName.hpp"

namespace mss {

/**
 * @brief Processeur virtuel; son modèle de coût reste à préciser.
 */
class Cpu {
  public:
    /**
     * @brief Signale que le modèle de coût CPU doit encore être défini.
     * @param operation Opération dont le coût est demandé.
     * @return Coût CPU lorsque le modèle sera défini.
     * @throws std::logic_error tant que la règle de coût n'est pas spécifiée.
     */
    int getOperationCost(OperationName operation) const;
};

} // namespace mss
