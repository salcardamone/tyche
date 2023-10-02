/**
 * @brief
 */
#ifndef __TYCHE_FORCE_FORCE_FACTORY_HPP
#define __TYCHE_FORCE_FORCE_FACTORY_HPP

// C++ Standard Libraries
#include <map>
#include <memory>
#include <cstring>
// Third-Party Libraries
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
   * @brief Create something that derives from Force.
   * @tparam Args Variadic types of arguments to be forwarded onto the
   * appropriate child Force constructor.
   * @param type The type of the force to create.
   * @param args The remaining arguments to be forwarded onto the appropriate
   * child Force constructor.
   * @return The force.
   */
  template <typename... Args>
  static std::unique_ptr<Force> create(std::string type, Args&&... args) {
    std::unique_ptr<Force> force;
    spdlog::info("Creating force of type: {}", type);
    if (type == "LennardJones") {
      force = std::move(
          std::make_unique<LennardJones>(std::forward<Args>(args)...));
    } else {
      throw std::runtime_error("Unrecognised force: " + type);
    }
    return force;
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_FORCE_FORCE_FACTORY_HPP */
