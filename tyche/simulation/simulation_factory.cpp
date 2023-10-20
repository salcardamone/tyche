/**
 * @brief
 */
// C++ Standard Libraries
#include <vector>
#include <stdexcept>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/io/reader.hpp"
#include "tyche/atom/dynamic_atomic_state.hpp"
#include "tyche/simulation/simulation_factory.hpp"
#include "tyche/simulation/molecular_dynamics_builder.hpp"

namespace tyche {

namespace {

// ========================================================================== //

/**
 * @brief Create a MolecularDynamics instance from configuration.
 * @param config Mapping from MolecularDynamics parameter keys to values.
 * @param atomic_state The atomic state we're simulating.
 * @return The instantiated MolecularDynamics instance.
 */
std::unique_ptr<MolecularDynamics> create_molecular_dynamics(
    Reader::Mapping& config, std::shared_ptr<DynamicAtomicState> atomic_state) {
  auto builder = MolecularDynamics::create(atomic_state);

  auto integrator_config = Reader::remove_prefix(config, "Integrator.");
  builder.integrator(integrator_config);

  auto cell_config = Reader::remove_prefix(config, "Cell.");
  builder.cell(cell_config);

  auto forces_config = std::any_cast<std::vector<std::any>>(config["Forces"]);
  for (auto&& force_config : forces_config) {
    builder.force(std::any_cast<Reader::Mapping>(force_config));
  }

  auto outputs_config = std::any_cast<std::vector<std::any>>(config["Outputs"]);
  for (auto&& output_config : outputs_config) {
    builder.output(std::any_cast<Reader::Mapping>(output_config));
  }

  return std::make_unique<MolecularDynamics>(builder.build());
}

// ========================================================================== //

}  // namespace

// ========================================================================== //

std::unique_ptr<Simulation> SimulationFactory::create(
    Reader::Mapping& config, std::shared_ptr<AtomicState> atomic_state) {
  std::unique_ptr<Simulation> simulation;
  auto type = must_find<std::string>(config, "type");
  if (type == "MolecularDynamics") {
    simulation = create_molecular_dynamics(
        config, std::static_pointer_cast<DynamicAtomicState>(atomic_state));
  } else {
    throw std::runtime_error("Unrecognised Simulation type: " + type);
  }
  return simulation;
}

// ========================================================================== //

}  // namespace tyche
