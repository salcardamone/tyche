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
   * @param config The "MolecularDynamics" node of the TOML configuration.
   * @return A new MolecularDynamics instance.
   */
  MolecularDynamics parse(toml::table& config) {
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
  /**
   * @brief Parse a TOML table into a mapping from keys into their values.
   * @param config The "MolecularDynamics.QQ" node of the TOML configuration.
   * @param prefix Optional string to prepend to key values.
   * @return Mapping from keys to values. The values are std::any, either a
   * string or double. Double is chosen for any numeric type because it can be
   * coerced into an integer.
   */
  std::map<std::string, std::any> parse_table(
      toml::table config, std::optional<std::string> prefix = std::nullopt) {
    std::map<std::string, std::any> mapping;

    config.for_each([&](const toml::key& key, auto&& val) {
      // If we have a prefix, then make sure we prepend it to the key
      std::string key_val("");
      if (prefix != std::nullopt) {
        key_val += prefix.value() + ".";
      }
      key_val += key.str();
      if constexpr (toml::is_number<decltype(val)>) {
        mapping[key_val] = static_cast<double>(val.get());
      } else if constexpr (toml::is_string<decltype(val)>) {
        mapping[key_val] = val.get();
      } else if constexpr (toml::is_table<decltype(val)>) {
        // Recursively descend tables, using current key value as prefix for all
        // keys therein
        auto subtable_map =
            parse_table(val, std::optional<std::string>(key_val));
        mapping.insert(subtable_map.begin(), subtable_map.end());
      } else {
        spdlog::error("Unrecognised type of TOML entry.");
      }
    });
    return mapping;
  }

  /**
   * @brief Parse a TOML array of TOML tables into an interable of mappings from
   * keys into their values.
   * @param array The "MolecularDynamics.QQ" node of the TOML configuration.
   * @return An iterable of mappings from keys to values. See the return type of
   * MolecularDynamicsReader::parse_table for more details.
   */
  std::vector<std::map<std::string, std::any>> parse_array_of_tables(
      toml::array array) {
    std::vector<std::map<std::string, std::any>> mappings;

    for (auto&& table : array) {
      mappings.push_back(parse_table(*table.as_table()));
    }
    return mappings;
  }

  std::shared_ptr<AtomicState> atomic_state_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_READER_HPP */
