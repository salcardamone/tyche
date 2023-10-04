/**
 * @brief
 */
#ifndef __TYCHE_INTEGRATE_INTEGRATE_FACTORY_HPP
#define __TYCHE_INTEGRATE_INTEGRATE_FACTORY_HPP

// C++ Standard Libraries
#include <any>
#include <memory>
#include <cstring>
#include <optional>
// Third-Party Libraries
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/util/maybe.hpp"
#include "tyche/integrate/integrate.hpp"
#include "tyche/integrate/velocity_verlet.hpp"

namespace tyche {

/**
 * @brief Factory pattern to return an integrator.
 */
class IntegrateFactory {
 public:
  /**
   * @brief Create an integrator from a map of parameters and their values.
   * @param config Mapping from Integrator parameter keys to values.
   * @return The integrator.
   */
  static std::unique_ptr<Integrate> create(
      std::map<std::string, std::any> config) {
    auto type = maybe_find<std::string>(config, "type");
    spdlog::info("Creating integrator of type: " + type.value());

    std::unique_ptr<Integrate> integrator;
    try {
      if (type.value() == "VelocityVerlet") {
        // Note that although we're maybe finding doubles, they'll be coerced
        // into optionals of the appropriate numeric type in the constructor
        integrator = std::make_unique<VelocityVerlet>(
            maybe_find<double>(config, "timestep"),
            maybe_find<double>(config, "num_steps"));
      } else {
        throw std::runtime_error("Unrecognised integrator: " + type.value());
      }
    } catch (std::bad_optional_access& err) {
      throw std::runtime_error(
          "Required integrator parameters were not found.");
    }
    return integrator;
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_INTEGRATE_INTEGRATE_FACTORY_HPP */
