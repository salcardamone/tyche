/**
 * @brief
 */
#ifndef __TYCHE_SIMULATION_SIMULATION_FACTORY_HPP
#define __TYCHE_SIMULATION_SIMULATION_FACTORY_HPP

// C++ Standard Libraries
#include <memory>
#include <string>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/io/reader.hpp"
#include "tyche/atom/atomic_state.hpp"
#include "tyche/simulation/simulation.hpp"

namespace tyche {

/**
 * @brief Factory to create simulation instances.
 */
class SimulationFactory {
 public:
  /**
   * @brief Create a simulation instance from a TOML configuration.
   * @param config Mapping from Simulation parameter keys to values.
   * @param atomic_state The atomic state to be simulated.
   * @return Simulation instance.
   */
  static std::unique_ptr<Simulation> create(
      Reader::Mapping& config, std::shared_ptr<AtomicState> atomic_state);
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SIMULATION_SIMULATION_FACTORY_HPP */
