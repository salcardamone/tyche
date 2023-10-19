/**
 * @brief
 */
// C++ Standard Libraries
#include <cmath>
#include <stdexcept>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/system/cell.hpp"

namespace tyche {

// ========================================================================== //

CubicCell::CubicCell(std::optional<double> length) : length_(length.value()) {
  if (length_ <= 0)
    throw std::runtime_error(
        "Cubic cell length must be strictly greater than zero.");
}

// ========================================================================== //

double CubicCell::volume() const { return length() * length() * length(); }

// ========================================================================== //

double CubicCell::length() const { return length_; }

// ========================================================================== //

void CubicCell::pbc(double &x, double &y, double &z) const {
  x -= std::floor(x / length()) * length();
  y -= std::floor(y / length()) * length();
  z -= std::floor(z / length()) * length();
}

// ========================================================================== //

void CubicCell::min_image(double &x, double &y, double &z) const {
  x -= std::round(x / length()) * length();
  y -= std::round(y / length()) * length();
  z -= std::round(z / length()) * length();
}

}  // namespace tyche
