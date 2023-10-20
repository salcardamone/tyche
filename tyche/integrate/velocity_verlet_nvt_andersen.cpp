/**
 * @brief
 */
// C++ Standard Libraries
#include <cmath>
#include <random>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/util/constants.hpp"
#include "tyche/util/tensor.hpp"
#include "tyche/integrate/velocity_verlet_nvt_andersen.hpp"

namespace tyche {

// ========================================================================== //

VelocityVerletNVTAndersen::VelocityVerletNVTAndersen(double dt,
                                                     std::size_t num_steps,
                                                     double temperature,
                                                     double t_relax,
                                                     double softness)
    : VelocityVerlet(dt, num_steps),
      Thermostat(temperature),
      t_relax_(t_relax),
      softness_(softness),
      mix_new_(std::sqrt(1 - softness * softness)) {}

// ========================================================================== //

void VelocityVerletNVTAndersen::initialise(DynamicAtomicState& state) {
  initialise_velocities(state);
}

// ========================================================================== //

void VelocityVerletNVTAndersen::step(DynamicAtomicState& state, Forces& forces,
                                     const Cell& cell) {
  half_step_one(state, cell);
  forces.evaluate(state, cell);
  half_step_two(state, cell);
  thermostat(state);
  ++current_step_;
}

// ========================================================================== //

void VelocityVerletNVTAndersen::thermostat(DynamicAtomicState& state) {
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_real_distribution<double> uniform(0.0, 1.0);
  std::normal_distribution<double> normal(0.0, 1.0);

  // Probability of a collision for each particle at this timestep
  const double prob_collision = 1 - std::exp(-dt_ / t_relax_);

  Tensor<double, 2>::iterator vel = state.vel();
  for (std::size_t iatom = 0; iatom < state.num_atoms(); ++iatom) {
    // If a collision has occured, do momentum transfer with fictitious particle
    if (uniform(generator) <= prob_collision) {
      // Is this correct? The DL POLY manual says the denominator should be (2 *
      // mass), but this just damps the instantaneous temperature way below what
      // it should be. The following appears to give qualitatively decent
      // results.
      const double vscale =
          std::sqrt(constants::boltzmann * constants::joule_to_internal *
                    temp_ / (state.atom_type(iatom)->mass()));
      // Mix velocity with that of fictitious particle
      for (std::size_t idim = 0; idim < 3; ++idim) {
        double vnew = vscale * normal(generator);
        vel[idim] = softness_ * vel[idim] + mix_new_ * vnew;
      }
    }
    vel += 3;
  }
}

// ========================================================================== //

}  // namespace tyche
