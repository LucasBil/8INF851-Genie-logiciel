#pragma once

#include <cstddef>
#include <vector>

#include "core/AddressSlot.hpp"
#include "core/AdressType.hpp"
#include "core/Matrix.hpp"

namespace mss {

/**
 * @brief 
 */
class MatrixAdress : AddressSlot {
  public:
    /**
     * @brief Retourne le type de cette adresse.
     * @return Type d'adresse de matrice.
     */
    AdressType GetType();

  private:
    Matrix matrix;
};

} // namespace mss
