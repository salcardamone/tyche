/**
 * @brief
 */
#ifndef __TYCHE_INTEGRATE_VELOCITY_VERLET_HPP
#define __TYCHE_INTEGRATE_VELOCITY_VERLET_HPP

// C++ Standard Libraries
//
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/cell.hpp"
#include "tyche/atomic_state.hpp"
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
   */
  VelocityVerlet(double dt) : Integrate(dt), half_dt_{dt / 2} {}

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
  void step(AtomicState& state, Forces& forces,
            std::shared_ptr<Cell> cell) override final {
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
      cell->pbc(pos[0], pos[1], pos[2]);
      pos += 3;
    }

    // Evaluate forces at new positions
    forces.evaluate(state, cell);

    vel = state.vel();
    force = state.force();
    // Advance velocity to full timestep
    for (std::size_t iatom = 0; iatom < state.num_atoms(); ++iatom) {
      double k = half_dt_ / state.atom_type(iatom)->mass();
      for (std::size_t idim = 0; idim < 3; ++idim) {
        *vel++ += k * *force++;
      }
    }
  }

 protected:
  double half_dt_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_INTEGRATE_VELOCITY_VERLET_HPP */
