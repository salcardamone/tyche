/**
 * @brief
 */
// C++ Standard Libraries
//
// Third-Party Libraries
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
// Project Inclusions
#include "test/integrate/test_integrate.hpp"
#include "tyche/integrate/velocity_verlet.hpp"
#include "tyche/atom/atomic_state_writer.hpp"

using namespace tyche;

/**
 * @brief Specialisation of the TestIntegrateLennardJonesEquilibrium fixture
 * using the Velocity Verlet integrator.
 */
class TestVelocityVerletArgonDimer
    : public TestIntegrateLennardJonesEquilibrium<VelocityVerlet> {};

/**
 * @brief Specialisation of the TestIntegrateLennardJonesCrystal fixture using
 * the Velocity Verlet integrator.
 */
class TestVelocityVerletArgonCrystal
    : public TestIntegrateLennardJonesCrystal<VelocityVerlet> {};

/**
 * @brief Make sure the Argon dimer initialised at Lennard-Jones equilibrium
 * doesn't deviate appreciably from start position.
 */
TEST_F(TestVelocityVerletArgonDimer, StationaryEquilibrium) {
  forces.evaluate(atomic_state, cell);
  const std::size_t num_steps = std::size_t(1E5);
  for (std::size_t istep = 0; istep < num_steps; ++istep) {
    integrator->step(atomic_state, forces, cell);
    ASSERT_NEAR(*atomic_state.pos(0), rij_min, 1E-15);
    ASSERT_NEAR(*atomic_state.pos(1), 0.0, 1E-15);
    ASSERT_NEAR(*atomic_state.vel(0), 0.0, 1E-15);
    ASSERT_NEAR(*atomic_state.vel(1), 0.0, 1E-15);
  }
}

/*
TEST_F(TestVelocityVerletArgonCrystal, Test) {
  std::ofstream kinetic;
  kinetic.open("/tmp/kinetic.txt");
  SetUp(64, 1.784E-2);
  AtomicStateWriterXYZ xyz_writer(std::filesystem::path("/tmp/tmp.xyz"));
  forces.evaluate(atomic_state, cell);
  const std::size_t num_steps = std::size_t(1E7);
  for (std::size_t istep = 0; istep < num_steps; ++istep) {
    integrator->step(atomic_state, forces, cell);
    if (!(istep % (num_steps / 1000))) {
      std::string tag("Frame ");
      xyz_writer.add(atomic_state, tag + std::to_string(istep));
      kinetic << atomic_state.average_kinetic() << '\t'
              << forces.evaluate(atomic_state, cell) << '\n';
    }
  }
  kinetic.close();
}
*/
