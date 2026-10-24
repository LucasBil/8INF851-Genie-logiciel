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
    AdressType GetType();

  private:
    Matrix matrix;
};

} // namespace mss
