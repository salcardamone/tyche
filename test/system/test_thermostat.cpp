/**
 * @brief
 */
// C++ Standard Libraries
//
// Third-Party Libraries
#include <gtest/gtest.h>
// Project Inclusions
#include "tyche/system/thermostat.hpp"
#include "test/base_fixtures/argon_box.hpp"

class TestThermostat : public ArgonBox {
 public:
  void SetUp(std::size_t num_atoms) {
    ArgonBox::SetUp(num_atoms, density * 10);
  }
};

using namespace tyche;

TEST_F(TestThermostat, QQ) {
  SetUp(64);
  Thermostat::initialise_velocities(*atomic_state, 300);
  ASSERT_NEAR(Thermostat::temperature(*atomic_state), 300, 1E-8);
}
