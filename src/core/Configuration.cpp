#include "core/Configuration.hpp"

#include <stdexcept>

namespace mss {

int Configuration::get(OperationName operation) const {
    const auto cost = costs.find(operation);
    if (cost == costs.end()) {
        throw std::out_of_range("Aucun coût n'est configuré pour cette opération.");
    }
    return cost->second;
}

int Configuration::set(OperationName operation, int cost) {
    costs.insert_or_assign(operation, cost);
    return cost;
}

} // namespace mss