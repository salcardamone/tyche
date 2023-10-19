/**
 * @brief
 */
#ifndef __TYCHE_INTEGRATE_VELOCITY_VERLET_NVT_EVANS_HPP
#define __TYCHE_INTEGRATE_VELOCITY_VERLET_NVT_EVANS_HPP

// C++ Standard Libraries
#include <cstdint>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/system/cell.hpp"
#include "tyche/atom/dynamic_atomic_state.hpp"
#include "tyche/force/force.hpp"
#include "tyche/system/thermostat.hpp"
#include "tyche/integrate/velocity_verlet.hpp"

namespace tyche {

/**
 * @brief Velocity-Verlet integrator using the Evans thermostat, where Ekin is
 * maintained as an integration constant (i.e. technically it generates
 * trajectories in the NVEkin ensemble).
 */
class VelocityVerletNVTEvans : public VelocityVerlet, public Thermostat {
 public:
  /**
   * @brief Class constructor.
   * @param dt Time increment for simulation step.
   * @param num_steps Number of integration steps to run the simulation for.
   * @param temperature The kinetic temperature to keep constant during
   * simulation.
   */
  VelocityVerletNVTEvans(double dt, std::size_t num_steps, double temperature);

  /**
   * @brief Various post-construction initialisation tasks for the atomic state.
   *
   * For this case, atomic velocities will be initialised from the
   * Maxwell-Boltzmann distribution at a given temperature.
   * @param state The atomic state to initialise.
   */
  void initialise(DynamicAtomicState& state) override;

  /**
   * @brief Propagate the atomic state forwards by the time increment using the
   * Velocity Verlet method with Evans thermostat.
   * @param state The atomic state to propagate forwards.
   * @param forces The force evaluation object.
   * @param cell The simulation cell for periodic boundary conditions.
   */
  void step(DynamicAtomicState& state, Forces& forces, const Cell& cell);

 protected:
  /**
   * @brief Thermostat the atomic velocities using the kinetic temperature
   * constraint, chi.
   * @param state The atomic state to thermostat.
   */
  void thermostat(DynamicAtomicState& state);

  /**
   * @brief Compute the kinetic temperature constraint at an instant:
   *
   *      \chi(t) = \frac{\sum_i v_i(t) \cdot f_i(t)}{\sum_i m_i v_i(t)^2}
   *              = \frac{\sum_i v_i(t) \cdot f_i(t)}{2 \cdot E_kin}
   *
   * @param state The atomic state to thermostat.
   * @return The kinetic temperature constraint.
   */
  double chi(DynamicAtomicState& state);
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_INTEGRATE_VELOCITY_VERLET_NVT_EVANS_HPP */
