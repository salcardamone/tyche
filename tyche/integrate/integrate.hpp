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
#include "tyche/atomic_state.hpp"
#include "tyche/force/lennard_jones.hpp"

namespace tyche {

/**
 * @brief Base virtual class for all integrators.
 */
class Integrate {
 public:
  /**
   * @brief Class constructor.
   * @param dt Time increment for simulation step.
   */
  Integrate(double dt) : dt_{dt} {}

  /**
   * @brief Base virtual method to be overridden by all integrators. Step the
   * simulation forward by the time increment.
   * @param state The atomic state to propagate forwards.
   * @param forces The force evaluation object.
   */
  virtual void step(AtomicState& state, LennardJones& forces) = 0;

 protected:
  double dt_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_INTEGRATE_INTEGRATE_HPP */
