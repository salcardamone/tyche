/**
 * @brief
 */
#ifndef __TYCHE_SIMULATION_SIMULATION_FACTORY_HPP
#define __TYCHE_SIMULATION_SIMULATION_FACTORY_HPP

// C++ Standard Libraries
#include <memory>
#include <cstring>
// Third-Party Libraries
#include <toml++/toml.h>
// Project Inclusions
#include "tyche/atom/atomic_state.hpp"
#include "tyche/simulation/simulation.hpp"
#include "tyche/simulation/molecular_dynamics.hpp"
#include "tyche/simulation/molecular_dynamics_reader.hpp"

namespace tyche {

/**
 * @brief Factory to create simulation instances.
 */
class SimulationFactory {
 public:
  /**
   * @brief Create a simulation instance from a TOML configuration.
   * @param type The type of simulation to instantiate.
   * @param config_table The "Simulation" node in the configuration.
   * @return Simulation instance.
   */
  static std::unique_ptr<Simulation> create(
      std::string type, toml::table config_table,
      std::shared_ptr<AtomicState> atomic_state) {
    std::unique_ptr<Simulation> simulation;
    if (type == "MolecularDynamics") {
      MolecularDynamicsReader reader(
          *config_table["MolecularDynamics"].as_table(), atomic_state);
      simulation = std::make_unique<MolecularDynamics>(reader.parse());
    } else {
      throw std::runtime_error("Unrecognised simulation type: " + type);
    }
    return simulation;
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SIMULATION_SIMULATION_FACTORY_HPP */
