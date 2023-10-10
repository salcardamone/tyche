/**
 * @brief
 */
// C++ Standard Libraries
//
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/system/thermostat.hpp"
#include "test/simulation/test_molecular_dynamics.hpp"

/**
 * @brief Just make sure we can perform a simulation without error.
 */
TEST_F(TestMolecularDynamics, Basic) {
  SetUp(32);
  //Thermostat::initialise_velocities(*atomic_state, 300);
  simulation->run();
}
