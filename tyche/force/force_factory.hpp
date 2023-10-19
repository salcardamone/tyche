/**
 * @brief
 */
#ifndef __TYCHE_FORCE_FORCE_FACTORY_HPP
#define __TYCHE_FORCE_FORCE_FACTORY_HPP

// C++ Standard Libraries
#include <any>
#include <map>
#include <memory>
#include <string>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/io/reader.hpp"
#include "tyche/atom/atom_type.hpp"
#include "tyche/force/force.hpp"

namespace tyche {

/**
 * @brief Factory pattern to return a force.
 */
class ForceFactory {
 public:
  /**
   * @brief Create something that derives from Force using an inline TOML table.
   * @param config A TOML table with a "type" field naming the force to
   * create, as well as any additional information to instantiate that force.
   * @param atom_type Mapping from an atom type to a unique index from
   * [0,num_atom_types) within the system under consideration.
   * @return The force.
   */
  static std::unique_ptr<Force> create(
      Reader::Mapping config,
      const std::map<std::shared_ptr<AtomType>, std::size_t>& atom_type);
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_FORCE_FORCE_FACTORY_HPP */
