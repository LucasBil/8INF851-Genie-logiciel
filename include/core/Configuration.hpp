#pragma once

#include "core/OperationName.hpp"

#include <unordered_map>

namespace mss {

/**
 * @brief Associe un coût entier à chaque type d'opération.
 */
class Configuration {
  public:
    /**
     * @brief Retourne le coût configuré pour une opération.
     * @throws std::out_of_range si aucun coût n'est configuré pour l'opération.
     */
    int get(OperationName operation) const;

    /**
     * @brief Configure le coût d'une opération.
     * @return Le coût configuré.
     */
    int set(OperationName operation, int cost);

    std::unordered_map<OperationName, int> costs;
};

} // namespace mss