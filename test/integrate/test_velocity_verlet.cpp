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
 * @brief
 */
TEST_F(TestVelocityVerlet, QQ) {
  AtomicStateWriter writer(std::filesystem::path("/tmp/tmp.xyz"));

  *atomic_state.pos(0) = 2;
  lj->evaluate(atomic_state);

  const std::size_t num_steps = std::size_t(1E8), out_freq = std::size_t(1E6);
  for (std::size_t istep = 0; istep < num_steps; ++istep) {
    integrator->step(atomic_state, *lj);
    if (!(istep % out_freq)) writer.add(atomic_state, "Argon Dimer");
  }
}
