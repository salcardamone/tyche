/**
 * @brief
 */
// C++ Standard Libraries
#include <cmath>
#include <random>
// Third-Party Libraries
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/util/tensor.hpp"
#include "tyche/util/constants.hpp"
#include "tyche/system/thermostat.hpp"

namespace tyche {

// ========================================================================== //

Thermostat::Thermostat(double temp) : temp_(temp) {}

// ========================================================================== //

void Thermostat::initialise_velocities(DynamicAtomicState& state) {
  spdlog::info(
      "Initialising atomic state velocities from Maxwell-Boltzmann "
      "distribution at {}K.",
      temp_);
  std::vector<double> vel_com(3);

  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_real_distribution<double> distribution(-1.0, 1.0);

  // Randomly initialise velocities from uniform distribution
  Tensor<double, 2>::iterator vel = state.vel();
  for (std::size_t iatom = 0; iatom < state.num_atoms(); ++iatom) {
    for (std::size_t idim = 0; idim < 3; ++idim) {
      *vel = distribution(generator);
      vel_com[idim] += *vel++;
    }
  }

  // Remove velocity of centre-of-mass of system
  vel = state.vel();
  for (std::size_t iatom = 0; iatom < state.num_atoms(); ++iatom) {
    for (std::size_t idim = 0; idim < 3; ++idim) {
      *vel++ -= vel_com[idim] / state.num_atoms();
    }
  }

  // Rescale velocities to match desired temperature
  double scale = std::sqrt(temp_ / temperature(state));
  vel = state.vel();
  for (std::size_t iatom = 0; iatom < state.num_atoms(); ++iatom) {
    for (std::size_t idim = 0; idim < 3; ++idim) {
      *vel++ *= scale;
    }
  }
}

// ========================================================================== //

double Thermostat::temperature(DynamicAtomicState& state) {
  // Boltzmann constant is in Joules / Kelvin. We need to convert the average
  // kinetic energy into Joules
  return (2 * state.average_kinetic()) /
         (3 * constants::boltzmann * constants::joule_to_internal);
}

// ========================================================================== //

}  // namespace tyche
