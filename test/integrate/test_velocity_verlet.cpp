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
  lj->evaluate(atomic_state);

  // VelocityVerlet vv(1E-15);
  for (std::size_t istep = 0; istep < 100; ++istep) {
    integrator->step(atomic_state, *lj);
  }
}
