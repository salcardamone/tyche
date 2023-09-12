/**
 * @brief
 */
// C++ Standard Libraries
//
// Third-Party Libraries
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/atom_type_reader.hpp"
#include "tyche/atomic_state_reader.hpp"
#include "tyche/force/lennard_jones.hpp"
#include "test/force/test_lennard_jones.hpp"

using namespace tyche;

/**
 * @brief Verify that we can find the minimum of the Lennard-Jones potential for
 * two Argon atoms, and that the forces between the atoms disappears at this
 * point.
 */
TEST_F(TestLennardJonesEquilibrium, PotentialAndForce) {
  // Potential at rmin should be -epsilon
  double v_pot = lj->evaluate(atomic_state);
  ASSERT_EQ(-atom_types["Ar"]->eps_lj(), v_pot);

  // Forces between atoms should be zero
  auto forces = atomic_state.force(0);
  ASSERT_NEAR(forces[0], 0.0, 1E-14);
  ASSERT_NEAR(forces[1], 0.0, 1E-14);
  ASSERT_NEAR(forces[2], 0.0, 1E-14);
  ASSERT_NEAR(forces[3], 0.0, 1E-14);
  ASSERT_NEAR(forces[4], 0.0, 1E-14);
  ASSERT_NEAR(forces[5], 0.0, 1E-14);
}
