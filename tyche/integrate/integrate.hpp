/**
 * @brief
 */
#ifndef __TYCHE_INTEGRATE_INTEGRATE_HPP
#define __TYCHE_INTEGRATE_INTEGRATE_HPP

// C++ Standard Libraries
#include <cstdint>
#include <optional>
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
   * @param num_steps The number of integration steps to perform.
   */
  Integrate(std::optional<double> dt, std::optional<std::size_t> num_steps)
      : dt_{dt.value()}, num_steps_{num_steps.value()} {}

  /**
   * @brief Base virtual method to be overridden by all integrators. Step the
   * simulation forward by the time increment.
   * @param state The atomic state to propagate forwards.
   * @param forces The force evaluation object.
   * @param cell The simulation cell for periodic boundary conditions.
   */
  virtual void step(AtomicState& state, Forces& forces, const Cell& cell) = 0;

  /**
   * @brief Getter for the time increment of the integrator.
   * @return The time increment.
   */
  double dt() const { return dt_; }

  /**
   * @brief Getter for the number of timesteps to integrate for.
   * @return The number of timesteps.
   */
  std::size_t num_steps() const { return num_steps_; }

 protected:
  double dt_;
  std::size_t num_steps_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_INTEGRATE_INTEGRATE_HPP */
