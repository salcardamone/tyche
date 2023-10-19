/**
 * @brief
 */
// C++ Standard Libraries
//
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/util/tensor.hpp"
#include "tyche/integrate/velocity_verlet.hpp"

namespace tyche {

// ========================================================================== //

VelocityVerlet::VelocityVerlet(double dt, std::size_t num_steps)
    : Integrate(dt, num_steps), half_dt_{dt_ / 2} {}

// ========================================================================== //

void VelocityVerlet::initialise(DynamicAtomicState& state) {}

// ========================================================================== //

void VelocityVerlet::step(DynamicAtomicState& state, Forces& forces,
                          const Cell& cell) {
  half_step_one(state, cell);
  forces.evaluate(state, cell);
  half_step_two(state, cell);
}

// ========================================================================== //

void VelocityVerlet::half_step_one(DynamicAtomicState& state,
                                   const Cell& cell) {
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

// ========================================================================== //

void VelocityVerlet::half_step_two(DynamicAtomicState& state,
                                   const Cell& cell) {
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

// ========================================================================== //

}  // namespace tyche
