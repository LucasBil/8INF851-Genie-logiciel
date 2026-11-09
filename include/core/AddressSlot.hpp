#pragma once

#include "core/AdressType.hpp"

namespace mss {

/**
 * @brief adresse mémoire
 */
class AddressSlot {
  public:
    /**
     * @brief Construit une adresse mémoire.
     */
    AddressSlot() {};

    /**
     * @brief Retourne le type de contenu de l'adresse.
     * @return Type de contenu de l'adresse.
     */
    virtual AdressType GetType();
};

} // namespace mss
