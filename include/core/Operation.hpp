#pragma once

#include "core/OperationName.hpp"

#include <cstddef>
#include <vector>

namespace mss {

/**
 * @brief Décrit une opération et les adresses mémoire auxquelles elle se réfère.
 *
 * @note L'exécution des opérations attend la confirmation de leurs règles.
 */
struct Operation {
    OperationName type;
    std::vector<std::size_t> inputLocations;
    std::vector<std::size_t> outputLocations{}; // Adresses des resultats reservees avant calcul.
};

} // namespace mss
