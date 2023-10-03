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
   * @param config_table The "Simulation" node in the configuration.
   * @param atomic_state The atomic state to be simulated.
   * @return Simulation instance.
   */
  static std::unique_ptr<Simulation> create(
      const toml::table& simulation_config,
      std::shared_ptr<AtomicState> atomic_state) {
    auto type = simulation_config["type"].value<std::string>();
    if (type == std::nullopt) {
      throw std::runtime_error("Simulation type must be specified.");
    }
    spdlog::info("Creating simulation of type: {}", *type);

    std::unique_ptr<Simulation> simulation;
    if (*type == "MolecularDynamics") {
      MolecularDynamicsReader reader(
          *simulation_config["MolecularDynamics"].as_table(), atomic_state);
      simulation =
          std::make_unique<MolecularDynamics>(std::move(reader.parse()));
    } else {
      throw std::runtime_error("Unrecognised simulation type: " + *type);
    }
    return simulation;
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SIMULATION_SIMULATION_FACTORY_HPP */
