/**
 * @brief
 */
#ifndef __TYCHE_INTEGRATE_VELOCITY_VERLET_NVT_EVANS_HPP
#define __TYCHE_INTEGRATE_VELOCITY_VERLET_NVT_EVANS_HPP

// C++ Standard Libraries
#include <limits>
#include <cstdint>
#include <optional>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/util/tensor.hpp"
#include "tyche/atom/atomic_state.hpp"
#include "tyche/force/force.hpp"
#include "tyche/system/cell.hpp"
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
  VelocityVerletNVTEvans(std::optional<double> dt,
                         std::optional<std::size_t> num_steps,
                         std::optional<double> temperature)
      : VelocityVerlet(dt, num_steps), Thermostat(temperature.value()) {}

  /**
   * @brief Various post-construction initialisation tasks for the atomic state.
   *
   * For this case, atomic velocities will be initialised from the
   * Maxwell-Boltzmann distribution at a given temperature.
   * @param state The atomic state to initialise.
   */
  void initialise(AtomicState& state) override { initialise_velocities(state); }

  /**
   * @brief Propagate the atomic state forwards by the time increment using the
   * Velocity Verlet method with Evans thermostat.
   * @param state The atomic state to propagate forwards.
   * @param forces The force evaluation object.
   * @param cell The simulation cell for periodic boundary conditions.
   */
  void step(AtomicState& state, Forces& forces, const Cell& cell) {
    thermostat(state);
    half_step_one(state, cell);
    forces.evaluate(state, cell);
    half_step_two(state, cell);
    thermostat(state);
  }

 protected:
  /**
   * @brief Thermostat the atomic velocities using the kinetic temperature
   * constraint, chi.
   * @param state The atomic state to thermostat.
   */
  void thermostat(AtomicState& state) {
    double scale = std::exp(-chi(state) * half_dt_);
    Tensor<double, 2>::iterator vel = state.vel();
    for (std::size_t iatom = 0; iatom < state.num_atoms(); ++iatom) {
      for (std::size_t idim = 0; idim < 3; ++idim) {
        *vel++ *= scale;
      }
    }
  }

  /**
   * @brief Compute the kinetic temperature constraint at an instant:
   *
   *      \chi(t) = \frac{\sum_i v_i(t) \cdot f_i(t)}{\sum_i m_i v_i(t)^2}
   *              = \frac{\sum_i v_i(t) \cdot f_i(t)}{2 \cdot E_kin}
   *
   * @param state The atomic state to thermostat.
   * @return The kinetic temperature constraint.
   */
  double chi(AtomicState& state) {
    double power = 0;
    Tensor<double, 2>::const_iterator vel = state.vel(), force = state.force();
    for (std::size_t iatom = 0; iatom < state.num_atoms(); ++iatom) {
      for (std::size_t idim = 0; idim < 3; ++idim) {
        power += *vel++ * *force++;
      }
    }
    // If velocities are just zero-initialised, then we need to avoid the
    // division by zero
    return 0.5 * power /
           (state.kinetic() + std::numeric_limits<double>::epsilon());
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_INTEGRATE_VELOCITY_VERLET_NVT_EVANS_HPP */
