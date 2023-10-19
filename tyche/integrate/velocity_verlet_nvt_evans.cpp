/**
 * @brief
 */
// C++ Standard Libraries
#include <cmath>
#include <limits>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/util/tensor.hpp"
#include "tyche/integrate/velocity_verlet_nvt_evans.hpp"

namespace tyche {

// ========================================================================== //

VelocityVerletNVTEvans::VelocityVerletNVTEvans(
    std::optional<double> dt, std::optional<std::size_t> num_steps,
    std::optional<double> temperature)
    : VelocityVerlet(dt, num_steps), Thermostat(temperature.value()) {}

// ========================================================================== //

void VelocityVerletNVTEvans::initialise(DynamicAtomicState& state) {
  initialise_velocities(state);
}

// ========================================================================== //

void VelocityVerletNVTEvans::step(DynamicAtomicState& state, Forces& forces,
                                  const Cell& cell) {
  thermostat(state);
  half_step_one(state, cell);
  forces.evaluate(state, cell);
  half_step_two(state, cell);
  thermostat(state);
}

// ========================================================================== //

void VelocityVerletNVTEvans::thermostat(DynamicAtomicState& state) {
  double scale = std::exp(-chi(state) * half_dt_);
  Tensor<double, 2>::iterator vel = state.vel();
  for (std::size_t iatom = 0; iatom < state.num_atoms(); ++iatom) {
    for (std::size_t idim = 0; idim < 3; ++idim) {
      *vel++ *= scale;
    }
  }
}

// ========================================================================== //

double VelocityVerletNVTEvans::chi(DynamicAtomicState& state) {
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

// ========================================================================== //

}  // namespace tyche
