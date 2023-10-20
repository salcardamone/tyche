/**
 * @brief
 */
#ifndef __TYCHE_TEST_SIMULATION_MOLECULAR_DYNAMICS_HPP
#define __TYCHE_TEST_SIMULATION_MOLECULAR_DYNAMICS_HPP

// C++ Standard Libraries
#include <memory>
// Third-Party Libraries
#include <toml++/toml.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
// Project Inclusions
#include "test/base_fixtures/argon_box.hpp"
#include "tyche/simulation/simulation_reader.hpp"
#include "tyche/simulation/simulation_factory.hpp"

using namespace tyche;
using namespace std::string_view_literals;

/**
 * @brief Test fixture for a molecular dynamics simulation using the box of
 * Argon.
 */
class TestMolecularDynamics : public ArgonBox {
 public:
  /**
   * @brief Setup the simulation and atomic state.
   * @param num_atoms The number of atoms to initialise the system with.
   */
  void SetUp(std::size_t num_atoms) {
    ArgonBox::SetUp(num_atoms, density * 10);
    toml::table config = toml::parse(simulation_config);
    // Need to rejig the cell length to match the box we initialise the atomic
    // state to
    config.at_path("Simulation.MolecularDynamics.Cell.length").ref<double>() =
        cell->length();
    auto reader = SimulationReader(atomic_state);
    simulation = reader.parse(*config["Simulation"].as_table());
  }

 protected:
  std::unique_ptr<Simulation> simulation;

  // Since we derive from ArgonBox, we already have an atomic state set-up, so
  // we don't need to specify that configuration here
  static constexpr std::string_view simulation_config = R"(
    [Simulation.MolecularDynamics.Cell]
    type = "Cubic"
    length = 1.0

    [Simulation.MolecularDynamics.Integrator]
    type = "VelocityVerlet"
    timestep = 1.0
    num_steps = 1E5

      [Simulation.MolecularDynamics.Integrator.Control]
      ensemble = "NVT"
      type = "Andersen"
      temperature = 300
      t_relax = 100
      softness = 0.75

    [[Simulation.MolecularDynamics.Outputs]]
    type = "xyz"
    frequency = 1E3
    path = "/tmp/test_molecular_dynamics.xyz"

    [[Simulation.MolecularDynamics.Outputs]]
    type = "therm"
    frequency = 1
    path = "/tmp/test_molecular_dynamics.therm"

    [[Simulation.MolecularDynamics.Forces]]
    type = "LennardJones"
)"sv;
};

#endif /* #ifndef __TYCHE_TEST_SIMULATION_MOLECULAR_DYNAMICS_HPP */
