/**
 * @brief
 */
#ifndef __TYCHE_INTEGRATE_VELOCITY_VERLET_NVT_ANDERSEN_HPP
#define __TYCHE_INTEGRATE_VELOCITY_VERLET_NVT_ANDERSEN_HPP

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
 * @brief Velocity-Verlet integrator using the Andersen thermostat. This
 * thermostat generates trajectories in NVT ensemble by stochastically
 * transferring momentum with fictitious particles. */
class VelocityVerletNVTAndersen : public VelocityVerlet, public Thermostat {
 public:
  /**
   * @brief Class constructor.
   * @param dt Time increment for simulation step.
   * @param num_steps Number of integration steps to run the simulation for.
   * @param temperature The kinetic temperature to keep constant during
   * simulation.
   * @param t_relax Amount of time between collisions.
   * @param softness How much of the particle's velocity to retain during
   * collision event.
   */
  VelocityVerletNVTAndersen(double dt, std::size_t num_steps,
                            double temperature, double t_relax,
                            double softness);

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
   * Velocity Verlet method with Andersen thermostat.
   * @param state The atomic state to propagate forwards.
   * @param forces The force evaluation object.
   * @param cell The simulation cell for periodic boundary conditions.
   */
  void step(DynamicAtomicState& state, Forces& forces, const Cell& cell);

 private:
  double t_relax_;
  double softness_, mix_new_;

  /**
   * @brief Thermostat the atomic velocities using the Andersen thermostat.
   *
   * Effectively, for each particle, randomly choose whether it collides with a
   * fictitious particle whose velocity is taken from the Maxwell-Boltzmann
   * distribution. If it does, mix the fictitious particle's velocity with the
   * actual particle's velocity, i.e. momentum transfer during collision.
   * @param state The atomic state to thermostat.
   */
  void thermostat(DynamicAtomicState& state);
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_INTEGRATE_VELOCITY_VERLET_NVT_ANDERSEN_HPP */
