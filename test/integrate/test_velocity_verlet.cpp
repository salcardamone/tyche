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
#include "tyche/atomic_state_writer.hpp"

using namespace tyche;

/**
 * @brief Specialisation of the TestIntegrateLennardJonesEquilibrium fixture
 * using the Velocity Verlet integrator.
 */
class TestVelocityVerlet
    : public TestIntegrateLennardJonesEquilibrium<VelocityVerlet> {};

/**
 * @brief Make sure the Argon dimer initialised at Lennard-Jones equilibrium
 * doesn't deviate appreciably from start position.
 */
TEST_F(TestVelocityVerlet, StationaryEquilibrium) {
  lj->evaluate(atomic_state);

  const std::size_t num_steps = std::size_t(1E5);
  for (std::size_t istep = 0; istep < num_steps; ++istep) {
    integrator->step(atomic_state, *lj, cell);
    ASSERT_NEAR(*atomic_state.pos(0), rij_min, 1E-15);
    ASSERT_NEAR(*atomic_state.pos(1), 0.0, 1E-15);
    ASSERT_NEAR(*atomic_state.vel(0), 0.0, 1E-15);
    ASSERT_NEAR(*atomic_state.vel(1), 0.0, 1E-15);
  }
}
