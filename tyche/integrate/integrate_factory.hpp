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
#include "tyche/integrate/velocity_verlet_nvt_evans.hpp"

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
    std::unique_ptr<Integrate> integrator;
    try {
      auto type = maybe_find<std::string>(config, "type");
      spdlog::info("Creating integrator of type: " + type.value());

      auto timestep = maybe_find<double>(config, "timestep");
      auto num_steps = maybe_find<double>(config, "num_steps");
      if (type.value() == "VelocityVerlet") {
        integrator =
            std::move(select_velocity_verlet(config, timestep, num_steps));
      } else {
        throw std::runtime_error("Unrecognised integrator: " + type.value());
      }
    } catch (std::bad_optional_access& err) {
      throw std::runtime_error(err.what());
    }
    return integrator;
  }

 private:
  /**
   * @brief Create an integrator that uses the Velocity Verlet convention.
   * @param config Mapping from Integrator parameter keys to values.
   * @param timestep The integration timestep.
   * @param num_steps The number of steps to take for the simulation.
   * @return The integrator.
   */
  static std::unique_ptr<Integrate> select_velocity_verlet(
      std::map<std::string, std::any> config, std::optional<double> timestep,
      std::optional<std::size_t> num_steps) {
    std::unique_ptr<Integrate> integrator;
    try {
      auto control = maybe_find<std::string>(config, "control.type");
      // If there's no controller in the configuration, we just initialise a
      // regular Velocity Verlet
      if (control == std::nullopt) {
        spdlog::info("Creating Velocity Verlet integrator with no controller.");
        integrator = std::make_unique<VelocityVerlet>(timestep, num_steps);
      }

      auto ensemble = maybe_find<std::string>(config, "ensemble");
      if (ensemble.value() == "NVT") {
        auto temperature = maybe_find<double>(config, "control.temperature");
        if (control.value() == "Evans") {
          spdlog::info(
              "Creating Velocity Verlet integrator with Evans thermostat at "
              "temperature {}K.",
              temperature.value());
          integrator = std::make_unique<VelocityVerletNVTEvans>(
              timestep, num_steps, temperature);
        }
      }
    } catch (std::bad_optional_access& err) {
      throw std::runtime_error(err.what());
    }
    return integrator;
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_INTEGRATE_INTEGRATE_FACTORY_HPP */
