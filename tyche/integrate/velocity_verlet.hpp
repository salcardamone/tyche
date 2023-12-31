/**
 * @brief
 */
#ifndef __TYCHE_INTEGRATE_VELOCITY_VERLET_HPP
#define __TYCHE_INTEGRATE_VELOCITY_VERLET_HPP

// C++ Standard Libraries
#include <cstdint>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/system/cell.hpp"
#include "tyche/atom/dynamic_atomic_state.hpp"
#include "tyche/force/force.hpp"
#include "tyche/integrate/integrate.hpp"

namespace tyche {

/**
 * @brief Velocity Verlet integrator.
 */
class VelocityVerlet : public Integrate {
 public:
  /**
   * @brief Class constructor.
   * @param dt Time increment for simulation step.
   * @param num_steps Number of integration steps to run the simulation for.
   */
  VelocityVerlet(double dt, std::size_t num_steps);

  /**
   * @brief Various post-construction initialisation tasks for the atomic state.
   *
   * For this case, atomic velocities will be initialised from the
   * Maxwell-Boltzmann distribution at a given temperature.
   * @param state The atomic state to initialise.
   */
  void initialise(DynamicAtomicState& state);

  /**
   * @brief Propagate the atomic state forwards by the time increment using the
   * Velocity Verlet method:
   *
   * Step 1: v(t + dt/2) = v(t) + dt/2 * a(t)
   * Step 2: r(t + dt) = r(t) + dt * v(t + dt/2)
   * Step 3: v(t + dt) = v(t + dt/2) + dt/2 * a(t + dt)
   *
   * @param state The atomic state to propagate forwards.
   * @param forces The force evaluation object.
   * @param cell The simulation cell for periodic boundary conditions.
   */
  virtual void step(DynamicAtomicState& state, Forces& forces,
                    const Cell& cell);

 protected:
  double half_dt_;

  /**
   * @brief Propagate the atomic state forwards by the time increment using
   * forces at the current time increment:
   *
   * Step 1: v(t + dt/2) = v(t) + dt/2 * a(t)
   * Step 2: r(t + dt) = r(t) + dt * v(t + dt/2)
   *
   * @param state The atomic state to propagate forwards.
   * @param cell The simulation cell for periodic boundary conditions.
   */
  void half_step_one(DynamicAtomicState& state, const Cell& cell);

  /**
   * @brief Propagate the atomic state forwards by the time increment using
   * forces at the next time increment:
   *
   * Step 3: v(t + dt) = v(t + dt/2) + dt/2 * a(t + dt)
   *
   * @param state The atomic state to propagate forwards.
   * @param cell The simulation cell for periodic boundary conditions.
   */
  void half_step_two(DynamicAtomicState& state, const Cell& cell);
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_INTEGRATE_VELOCITY_VERLET_HPP */
