/**
 * @brief
 */
#ifndef __TYCHE_THERMOSTAT_HPP
#define __TYCHE_THERMOSTAT_HPP

// C++ Standard Libraries
//
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/atom/dynamic_atomic_state.hpp"

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
  Thermostat(double temp);

  /**
   * @brief Initialise atomic velocities from Maxwell-Boltzmann distribution at
   * parameterised temperatures.
   * @param state The atomic state to initialise velocities of.
   */
  void initialise_velocities(DynamicAtomicState& state);

  /**
   * @brief Compute the kinetic temperature associated with the atomic state.
   * @param state The atomic state to compute the kinetic temperature of.
   * @return The kinetic temperature.
   */
  static double temperature(DynamicAtomicState& state);

 protected:
  double temp_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_THERMOSTAT_HPP */
