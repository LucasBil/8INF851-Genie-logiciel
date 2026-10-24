#pragma once

#include "core/AdressType.hpp"

namespace mss {

/**
 * @brief adresse mémoire
 */
class AddressSlot {
  public:
    AddressSlot() {};
    virtual AdressType GetType();
};

} // namespace mss
