/**
 * @brief
 */
// C++ Standard Libraries
//
// Third-Party Libraries
#include <gtest/gtest.h>
// Project Inclusions
#include "tyche/atom/atom_type_reader.hpp"
#include "tyche/atom/atomic_state_reader.hpp"
#include "tyche/simulation/simulation_factory.hpp"

using namespace tyche;
using namespace std::string_view_literals;

static constexpr std::string_view basic_toml = R"(
    [Atoms.Ar]
    positions = [
        [0.00000000, 0.00000000, 0.00000000],
        [0.00000000, 0.00000000, 0.00000000],
    ]

    [AtomTypes.Ar]      

    [Simulation]
    type = "MolecularDynamics"

    [Simulation.MolecularDynamics]
    num_steps = 1E6
    integrator = "VelocityVerlet"
    timestep = 2
    cell = { type = "CubicCell", length = 2.0 }
    forces = [
        { type = "LennardJones" }
    ]
)"sv;

TEST(TestMolecularDynamicsReader, QQ) {
  toml::table config = toml::parse(basic_toml);

  AtomTypeReader reader(config);
  auto atom_types = reader.parse();
  AtomicStateReader reader2(config);
  auto atomic_state = std::make_shared<AtomicState>(reader2.parse(atom_types));

  auto sim = SimulationFactory::create(
      config["Simulation"]["type"].value_or<std::string>(""),
      *config["Simulation"].as_table(), atomic_state);
}
