/**
 * @brief
 */
#ifndef __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_READER_HPP
#define __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_READER_HPP

// C++ Standard Libraries
#include <any>
#include <map>
#include <cstring>
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
   * @param atomic_state The atomic state we're going to simulate.
   */
  MolecularDynamicsReader(std::shared_ptr<AtomicState> atomic_state)
      : atomic_state_{atomic_state} {}

  /**
   * @brief Parse a MolecularDynamics configuration from TOML and return a new
   * MolecularDynamics instance.
   * @param config_table The "MolecularDynamics" node of the TOML configuration.
   * @return A new MolecularDynamics instance.
   */
  MolecularDynamics parse(toml::table& config) {
    return MolecularDynamics::create(atomic_state_)
        .integrator(parse_table(*config["integrator"].as_table()))
        .forces(*config["forces"].as_array())
        .cell(parse_table(*config["cell"].as_table()))
        .build();
  }

 private:
  /**
   * @brief Parse a TOML table into a mapping from keys into their values.
   * @param config The "MolecularDynamics.QQ" node of the TOML configuration.
   * @return Mapping from keys to values. The values are std::any, either a
   * string or double. Double is chosen for any numeric type because it can be
   * coerced into an integer.
   */
  std::map<std::string, std::any> parse_table(toml::table config) {
    std::map<std::string, std::any> mapping;
    config.for_each([&mapping](const toml::key& key, auto&& val) {
      std::string key_val(key.str());
      if constexpr (toml::is_number<decltype(val)>) {
        mapping[key_val] = static_cast<double>(val.get());
      } else if constexpr (toml::is_string<decltype(val)>) {
        mapping[key_val] = val.get();
      }
    });
    return mapping;
  }

  std::shared_ptr<AtomicState> atomic_state_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_READER_HPP */
