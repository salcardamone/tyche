/**
 * @brief Reader from TOML config file to yield all AtomTypes therein.
 */
#ifndef __TYCHE_ATOM_TYPE_READER_HPP
#define __TYCHE_ATOM_TYPE_READER_HPP

// Standard Libraries
#include <map>
#include <string>
#include <ranges>
#include <vector>
// Third-party Libraries
#include <spdlog/spdlog.h>
#include <toml++/toml.h>
// Project Inclusions
#include "tyche/atom/atom_type.hpp"
#include "tyche/atom/atom_type_builder.hpp"
#include "tyche/util/maybe.hpp"
#include "tyche/io/toml_reader.hpp"

namespace tyche {

class AtomTypeReader
    : public TOMLReader<std::map<std::string, std::shared_ptr<AtomType>>> {
 public:
  using AtomTypeMapping = std::map<std::string, std::shared_ptr<AtomType>>;
  using TOMLReader<AtomTypeMapping>::Mapping;

  AtomTypeReader() {}

  AtomTypeMapping parse(toml::table& config) {
    AtomTypeMapping mapping;
    auto atom_types = parse_keys(config);
    for (const auto& atom_type : atom_types) {
      mapping[atom_type] = std::make_shared<AtomType>(
          parse_atom_type(*config[atom_type].as_table(), atom_type));
    }
    return mapping;
  }

 private:
  AtomType parse_atom_type(toml::table& config, std::string atom_type) {
    auto mapping = parse_table(config);
    // Partially create the builder, setting all fundamental parameters
    auto builder =
        AtomType::create(atom_type)
            .mass(maybe_find<double>(mapping, "mass"))
            .num_electrons(maybe_find<double>(mapping, "num_electrons"))
            .nuclear_charge(maybe_find<double>(mapping, "nuclear_charge"));
    // Remove fundamental parameters from the mapping
    mapping.erase("mass");
    mapping.erase("num_electrons");
    mapping.erase("nuclear_charge");
    // Set non.fundamental parameters and return the AtomType
    return builder.others(mapping).build();
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_ATOM_TYPE_READER_HPP */
