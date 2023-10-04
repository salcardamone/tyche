/**
 * @brief
 */
#ifndef __TYCHE_FORCE_FORCE_FACTORY_HPP
#define __TYCHE_FORCE_FORCE_FACTORY_HPP

// C++ Standard Libraries
#include <any>
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
   * @param config A TOML table with a "type" field naming the force to
   * create, as well as any additional information to instantiate that force.
   * @param atom_type Mapping from an atom type to a unique index from
   * [0,num_atom_types) within the system under consideration.
   * @return The force.
   */
  static std::unique_ptr<Force> create(
      std::map<std::string, std::any> config,
      const std::map<std::shared_ptr<AtomType>, std::size_t>& atom_type) {
    auto type = maybe_find<std::string>(config, "type");
    spdlog::info("Creating force of type: {}", type.value());

    std::unique_ptr<Force> force;
    try {
      if (type.value() == "LennardJones") {
        force = std::make_unique<LennardJones>(atom_type);
      } else {
        throw std::runtime_error("Unrecognised force: " + type.value());
      }
    } catch (std::bad_optional_access& err) {
      throw std::runtime_error("Unrecognised force: " + type.value());
    }
    return force;
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_FORCE_FORCE_FACTORY_HPP */
