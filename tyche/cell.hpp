/**
 * @brief
 */
#ifndef __TYCHE_CELL_HPP
#define __TYCHE_CELL_HPP

// C++ Standard Libraries
#include <cmath>
// Third-Party Libraries
//
// Project Inclusions
//

namespace tyche {

/**
 * @brief Simulation cell abstract base class.
 */
class Cell {
 public:
  /**
   * @brief Return the volume of the cell.
   * @return The volume of the cell.
   */
  virtual double volume() const = 0;

  /**
   * @brief Apply periodic-boundary conditions to a position vector.
   * @param x x-coordinate of the input position vector.
   * @param y y-coordinate of the input position vector.
   * @param z z-coordinate of the input position vector.
   */
  virtual void pbc(double &x, double &y, double &z) const = 0;
};

/**
 * @brief Cubic cell specialisation of the simulation cell.
 */
class CubicCell : public Cell {
 public:
  /**
   * @brief Class constructor.
   * @param length The length of each side of the cubic cell.
   */
  CubicCell(double length) : length_(length) {
    if (length_ <= 0)
      throw std::runtime_error(
          "Cubic cell length must be strictly greater than zero.");
  }

  /**
   * @brief Return the volume of the cell.
   * @return The volume of the cell.
   */
  double volume() const { return length_ * length_ * length_; }

  /**
   * @brief Apply periodic-boundary conditions to a position vector. Origin is
   * at (0,0,0) and the cell is in the positive octant of the coordinate system.
   * @param x x-coordinate of the input position vector.
   * @param y y-coordinate of the input position vector.
   * @param z z-coordinate of the input position vector.
   */
  void pbc(double &x, double &y, double &z) const {
    x -= std::floor(x / length_) * length_;
    y -= std::floor(y / length_) * length_;
    z -= std::floor(z / length_) * length_;
  }

 private:
  double length_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_CELL_HPP */
