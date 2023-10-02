/**
 * @brief
 */
#ifndef __TYCHE_THERMOSTAT_HPP
#define __TYCHE_THERMOSTAT_HPP

// C++ Standard Libraries
#include <cmath>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/util/tensor.hpp"
#include "tyche/util/constants.hpp"
#include "tyche/atom/atomic_state.hpp"

namespace tyche {

/**
 * @brief Abstract base thermostat class.
 */
class Thermostat {
 public:
  /**
   * @brief Class constructor.
   * @param temp The desired temperature to maintain.
   */
  Thermostat(double temp) : temp_(temp) {}

  /**
   * @brief Compute the kinetic temperature associated with the atomic state.
   * @param state The atomic state to compute the kinetic temperature of.
   * @return The kinetic temperature.
   */
  static double temperature(AtomicState& state) {
    // Boltzmann constant is in Joules / Kelvin. We need to convert the average
    // kinetic energy into Joules
    return (2 * state.average_kinetic()) /
           (3 * constants::boltzmann * constants::joule_to_internal);
  }

  /**
   * @brief Abstract application method; thermostat an atomic state.
   * @param state The atomic state to thermostat.
   */
  virtual void apply(AtomicState& state) = 0;

 protected:
  double temp_;
};

/**
 * @brief Velocity rescaling specialisation of the thermostat.
 */
class VelocityRescalingThermostat : public Thermostat {
 public:
  /**
   * @brief Class constructor.
   * @param temp The desired temperature to maintain.
   */
  VelocityRescalingThermostat(double temp) : Thermostat(temp) {}

  /**
   * @brief Apply velocity rescaling to the atomic state to maintain a constant
   * kinetic energy in the system.
   *
   * You can find this in Equation (8.15) of Computational Physics, Thijssen.
   * @param state The atomic state to thermostat.
   */
  void apply(AtomicState& state) {
    double lambda = std::sqrt((temp_ * (state.num_atoms() - 1)) /
                              Thermostat::temperature(state));

    Tensor<double, 2>::iterator vel = state.vel();
    for (std::size_t iatom = 0; iatom < state.num_atoms(); ++iatom) {
      for (std::size_t idim = 0; idim < 3; ++idim) {
        *vel++ *= lambda;
      }
    }
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_THERMOSTAT_HPP */
