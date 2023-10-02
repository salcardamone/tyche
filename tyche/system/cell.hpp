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

  virtual void min_image(double &x, double &y, double &z) const = 0;
};

/**
 * @brief Unbounded cell specialisation of the simulation cell.
 */
class UnboundedCell : public Cell {
 public:
  /**
   * @brief Return the volume of the cell.
   * @return The volume of the cell. Dummy value of zero since cell is
   * unbounded.
   */
  double volume() const { return 0; }

  /**
   * @brief Apply periodic-boundary conditions to a position vector. Unbounded
   * cell does nothing here; just return without modifying.
   * @param x x-coordinate of the input position vector.
   * @param y y-coordinate of the input position vector.
   * @param z z-coordinate of the input position vector.
   */
  void pbc(double &x, double &y, double &z) const override final { return; }

  void min_image(double &x, double &y, double &z) const override final {
    return;
  }
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
  double volume() const { return length() * length() * length(); }

  /**
   *
   */
  double length() const { return length_; }

  /**
   * @brief Apply periodic-boundary conditions to a position vector. Origin is
   * at (0,0,0) and the cell is in the positive octant of the coordinate system.
   * @param x x-coordinate of the input position vector.
   * @param y y-coordinate of the input position vector.
   * @param z z-coordinate of the input position vector.
   */
  void pbc(double &x, double &y, double &z) const override final {
    x -= std::floor(x / length()) * length();
    y -= std::floor(y / length()) * length();
    z -= std::floor(z / length()) * length();
  }

  void min_image(double &x, double &y, double &z) const override final {
    x -= std::round(x / length()) * length();
    y -= std::round(y / length()) * length();
    z -= std::round(z / length()) * length();
  }

 private:
  double length_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_CELL_HPP */