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
#include "tyche/atom/atomic_state.hpp"
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
  VelocityVerlet(std::optional<double> dt, std::optional<std::size_t> num_steps)
      : Integrate(dt, num_steps), half_dt_{dt_ / 2} {}

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
  virtual void step(AtomicState& state, Forces& forces, const Cell& cell) {
    half_step_one(state, cell);
    forces.evaluate(state, cell);
    half_step_two(state, cell);
  }

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
  void half_step_one(AtomicState& state, const Cell& cell) {
    Tensor<double, 2>::iterator pos = state.pos(), vel = state.vel();
    Tensor<double, 2>::const_iterator force = state.force();
    // Advance velocity by half timestep and position by full timestep
    for (std::size_t iatom = 0; iatom < state.num_atoms(); ++iatom) {
      double k = half_dt_ / state.atom_type(iatom)->mass();
      for (std::size_t idim = 0; idim < 3; ++idim) {
        *vel += k * *force++;
        pos[idim] += *vel++ * dt_;
      }
      // Apply periodic boundary conditions
      cell.pbc(pos[0], pos[1], pos[2]);
      pos += 3;
    }
  }

  /**
   * @brief Propagate the atomic state forwards by the time increment using
   * forces at the next time increment:
   *
   * Step 3: v(t + dt) = v(t + dt/2) + dt/2 * a(t + dt)
   *
   * @param state The atomic state to propagate forwards.
   * @param cell The simulation cell for periodic boundary conditions.
   */
  void half_step_two(AtomicState& state, const Cell& cell) {
    Tensor<double, 2>::iterator vel = state.vel();
    Tensor<double, 2>::const_iterator force = state.force();
    // Advance velocity to full timestep
    for (std::size_t iatom = 0; iatom < state.num_atoms(); ++iatom) {
      double k = half_dt_ / state.atom_type(iatom)->mass();
      for (std::size_t idim = 0; idim < 3; ++idim) {
        *vel++ += k * *force++;
      }
    }
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_INTEGRATE_VELOCITY_VERLET_HPP */
