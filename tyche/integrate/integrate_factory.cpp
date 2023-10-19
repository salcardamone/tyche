/**
 * @brief
 */
// C++ Standard Libraries
#include <optional>
#include <stdexcept>
// Third-Party Libraries
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/util/must.hpp"
#include "tyche/util/maybe.hpp"
#include "tyche/io/reader.hpp"
#include "tyche/integrate/integrate_factory.hpp"
#include "tyche/integrate/velocity_verlet.hpp"
#include "tyche/integrate/velocity_verlet_nvt_evans.hpp"

namespace tyche {

// ========================================================================== //

std::unique_ptr<Integrate> IntegrateFactory::create(Reader::Mapping config) {
  std::unique_ptr<Integrate> integrator;
  auto type = must_find<std::string>(config, "type");
  spdlog::info("Creating integrator of type: " + type);

  auto timestep = must_find<double>(config, "timestep");
  auto num_steps = must_find<double>(config, "num_steps");
  if (type == "VelocityVerlet") {
    integrator = std::move(select_velocity_verlet(config, timestep, num_steps));
  } else {
    throw std::runtime_error("Unrecognised integrator: " + type);
  }
  return integrator;
}

// ========================================================================== //

std::unique_ptr<Integrate> IntegrateFactory::select_velocity_verlet(
    Reader::Mapping config, double timestep, std::size_t num_steps) {
  std::unique_ptr<Integrate> integrator;

  auto control = maybe_find<std::string>(config, "control.type");
  // If there's no controller in the configuration, we just initialise a
  // regular Velocity Verlet
  if (control == std::nullopt) {
    spdlog::info("Creating Velocity Verlet integrator with no controller.");
    integrator = std::make_unique<VelocityVerlet>(timestep, num_steps);
  }

  // If we have a controller, it has to derive from a thermodynamic ensemble
  auto ensemble = must_find<std::string>(config, "ensemble");
  if (ensemble == "NVT") {
    auto temperature = must_find<double>(config, "control.temperature");
    if (control.value() == "Evans") {
      spdlog::info(
          "Creating Velocity Verlet integrator with Evans thermostat at "
          "temperature {:.2f}K.",
          temperature);
      integrator = std::make_unique<VelocityVerletNVTEvans>(timestep, num_steps,
                                                            temperature);
    }
  }
  return integrator;
}

// ==========================================================================

}  // namespace tyche
