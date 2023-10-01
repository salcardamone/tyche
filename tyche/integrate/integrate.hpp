/**
 * @brief
 */
#ifndef __TYCHE_INTEGRATE_INTEGRATE_HPP
#define __TYCHE_INTEGRATE_INTEGRATE_HPP

// C++ Standard Libraries
//
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/system/cell.hpp"
#include "tyche/atom/atomic_state.hpp"
#include "tyche/force/force.hpp"

namespace tyche {

/**
 * @brief Base virtual class for all integrators.
 */
class Integrate {
 public:
  /**
   * @brief Class constructor.
   * @param dt Time increment for simulation step in femtoseconds.
   */
  Integrate(double dt) : dt_{dt} {}

  /**
   * @brief Base virtual method to be overridden by all integrators. Step the
   * simulation forward by the time increment.
   * @param state The atomic state to propagate forwards.
   * @param forces The force evaluation object.
   * @param cell The simulation cell for periodic boundary conditions.
   */
  virtual void step(AtomicState& state, Forces& forces,
                    std::shared_ptr<Cell> cell) = 0;

 protected:
  double dt_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_INTEGRATE_INTEGRATE_HPP */
