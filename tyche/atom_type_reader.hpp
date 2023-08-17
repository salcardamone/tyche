/**
 * @brief Reader from TOML config file to yield all AtomTypes therein.
 */
#ifndef __TYCHE_ATOM_TYPE_READER_HPP
#define __TYCHE_ATOM_TYPE_READER_HPP

// Standard Libraries
#include <map>
#include <string>
// Third-party Libraries
#include <spdlog/spdlog.h>
#include <toml++/toml.h>
// Project Inclusions
#include "atom_type.hpp"
#include "atom_type_builder.hpp"

namespace tyche {

/**
 * @brief Reader of a TOML file to retrieve atom type information.
 */
class AtomTypeReader {
 public:
  /**
   * @brief Class constructor.
   * @param config_table TOML table with an "AtomTypes" node.
   */
  AtomTypeReader(toml::table config_table)
      : toml_{*config_table["AtomTypes"].as_table()} {}

  /**
   * @brief Parse all AtomType objects from the configuration TOML.
   * @return A map from the AtomType ID to a shared pointer to the AtomType
   * object.
   */
  std::map<std::string, std::shared_ptr<AtomType>> parse() {
    // Our returned mapping from atom type identifier to the AtomType
    std::map<std::string, std::shared_ptr<AtomType>> atom_types;

    for (auto key : toml_) {
      std::ostringstream ss;
      ss << key.first;
      auto atom_type = std::make_shared<AtomType>(parse_atom_type(ss.str()));
      spdlog::info("{}", *atom_type);
      atom_types[ss.str()] = atom_type;
    }

    return atom_types;
  }

 private:
  toml::table toml_;

  /**
   * @brief Parse an AtomType from the "AtomTypes" node in the TOML table.
   * @param atom_type Identifier string for the AtomType.
   * @return The parsed AtomType.
   */
  AtomType parse_atom_type(std::string atom_type) {
    spdlog::info("Attempting to parse atom type: {}", atom_type);
    try {
      auto atom_type_toml = toml_[atom_type];

      return AtomType::create(atom_type)
          .mass(atom_type_toml["mass"].value<double>())
          .num_electrons(atom_type_toml["num_electrons"].value<uint32_t>())
          .nuclear_charge(atom_type_toml["nuclear_charge"].value<uint32_t>())
          .build();

    } catch (const toml::parse_error& err) {
      spdlog::critical("Parsing Failed: {}", err.description());
    }
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_ATOM_TYPE_READER_HPP */
