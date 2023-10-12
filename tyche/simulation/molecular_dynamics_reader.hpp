/**
 * @brief
 */
#ifndef __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_READER_HPP
#define __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_READER_HPP

// C++ Standard Libraries
#include <memory>
// Third-Party Libraries
#include <toml++/toml.h>
// Project Inclusions
#include "tyche/io/toml_reader.hpp"
#include "tyche/simulation/molecular_dynamics.hpp"
#include "tyche/simulation/molecular_dynamics_builder.hpp"

namespace tyche {

/**
 * @brief Reader for the MolecularDynamics class.
 */
class MolecularDynamicsReader : public TOMLReader<MolecularDynamics> {
 public:
  /**
   * @brief Class constructor.
   * @param atomic_state The atomic state we're going to simulate.
   */
  MolecularDynamicsReader(std::shared_ptr<AtomicState> atomic_state)
      : atomic_state_{atomic_state} {}

  /**
   * @brief Parse a MolecularDynamics configuration from TOML and return a new
   * MolecularDynamics instance.
   * @param config The "MolecularDynamics" node of the TOML configuration.
   * @return A new MolecularDynamics instance.
   */
  MolecularDynamics parse(toml::table& config) override final {
    auto builder = MolecularDynamics::create(atomic_state_);
    builder.integrator(parse_table(*config["Integrator"].as_table()));
    builder.cell(parse_table(*config["cell"].as_table()));
    for (auto force : parse_array_of_tables(*config["forces"].as_array())) {
      builder.force(std::move(force));
    }
    for (auto output : parse_array_of_tables(*config["output"].as_array())) {
      builder.output(std::move(output));
    }
    return builder.build();
  }

 private:
  std::shared_ptr<AtomicState> atomic_state_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_READER_HPP */
