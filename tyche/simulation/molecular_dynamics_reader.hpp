/**
 * @brief
 */
#ifndef __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_READER_HPP
#define __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_READER_HPP

// C++ Standard Libraries
//
// Third-Party Libraries
#include <toml++/toml.h>
// Project Inclusions
#include "tyche/simulation/molecular_dynamics.hpp"
#include "tyche/simulation/molecular_dynamics_builder.hpp"

namespace tyche {

/**
 * @brief Reader for the MolecularDynamics class.
 */
class MolecularDynamicsReader {
 public:
  /**
   * @brief Class constructor.
   * @param config_table TOML config with a "MolecularDynamics" node.
   */
  MolecularDynamicsReader(toml::table config_table,
                          std::shared_ptr<AtomicState> atomic_state)
      : toml_{config_table}, atomic_state_{atomic_state} {}

  /**
   * @brief Parse a MolecularDynamics configuration from TOML and return a new
   * MolecularDynamics instance.
   * @return A new MolecularDynamics instance.
   */
  MolecularDynamics parse() {
    return MolecularDynamics::create(atomic_state_)
        .num_steps(toml_["num_steps"].value<std::size_t>())
        .integrator(toml_["integrator"].value<std::string>(),
                    toml_["timestep"].value<double>())
        .forces(*toml_["forces"].as_array())
        .build();
  }

 private:
  toml::table toml_;
  std::shared_ptr<AtomicState> atomic_state_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_READER_HPP */
