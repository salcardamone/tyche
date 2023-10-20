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
  simulation->run();
}
