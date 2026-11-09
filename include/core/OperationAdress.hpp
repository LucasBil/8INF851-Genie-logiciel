#pragma once

#include <cstddef>
#include <vector>

#include "core/AddressSlot.hpp"
#include "core/AdressType.hpp"
#include "core/OperationName.hpp"

#include <vector>

namespace mss {

/**
 * @brief 
 */
class OperationAdress : AddressSlot {
  public:
    /**
     * @brief Retourne le type de cette adresse.
     * @return Type d'adresse d'opération.
     */
    AdressType GetType();

  private:
    std::vector<int> inputLocations;
    OperationName operationType;

};

} // namespace mss
