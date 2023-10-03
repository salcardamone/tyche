/**
 * @brief
 */
#ifndef __TYCHE_FORCE_FORCE_FACTORY_HPP
#define __TYCHE_FORCE_FORCE_FACTORY_HPP

// C++ Standard Libraries
#include <map>
#include <memory>
#include <cstring>
#include <optional>
// Third-Party Libraries
#include <toml++/toml.h>
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/atom/atom_type.hpp"
#include "tyche/force/force.hpp"
#include "tyche/force/lennard_jones.hpp"

namespace tyche {

/**
 * @brief Factory pattern to return a force.
 */
class ForceFactory {
 public:
  /**
   * @brief Create something that derives from Force using an inline TOML table.
   * @param force_config A TOML table with a "type" field naming the force to
   * create, as well as any additional information to instantiate that force.
   * @param atom_type Mapping from an atom type to a unique index from
   * [0,num_atom_types) within the system under consideration.
   * @return The force.
   */
  static std::unique_ptr<Force> create(
      const toml::table& force_config,
      const std::map<std::shared_ptr<AtomType>, std::size_t>& atom_type) {
    auto type = force_config["type"].value<std::string>();
    if (type == std::nullopt) {
      throw std::runtime_error("Force type must be specified.");
    }
    spdlog::info("Creating force of type: {}", *type);

    std::unique_ptr<Force> force;
    if (*type == "LennardJones") {
      force = std::make_unique<LennardJones>(atom_type);
    } else {
      throw std::runtime_error("Unrecognised force: " + *type);
    }
    return force;
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_FORCE_FORCE_FACTORY_HPP */
