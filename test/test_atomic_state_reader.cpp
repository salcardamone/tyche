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

using namespace tyche;
using namespace std::string_view_literals;

/**
 * @brief Test fixture for atomic state reader.
 */
class TestAtomicStateReader : public ::testing::Test {
 public:
  void SetUp() override {
    spdlog::set_level(spdlog::level::off);
    toml::table config = toml::parse(basic_toml);
    AtomTypeReader reader(config);
    atom_types = reader.parse();

    AtomicStateReader reader2(config);
    atomic_state = reader2.parse(atom_types);
    atomic_state_vel_force = reader2.parse(atom_types, true, true);
  }

  std::map<std::string, std::shared_ptr<AtomType>> atom_types;
  AtomicState atomic_state, atomic_state_vel_force;

 private:
  static constexpr std::string_view basic_toml = R"(
    [Atoms.H]
    positions = [
        [0.00000000, 0.00000000, 0.00000000],
        [0.00000000, 0.00000000, 1.00000000],
    ]
    velocities = [
        [0.50000000, 0.00000000, 0.50000000],
        [0.00000000, 1.00000000, 0.00000000],
    ]
    forces = [
        [0.00000000, 0.12500000, 0.00000000],
        [2.00000000, 0.00000000, 2.50000000],
    ]

    [Atoms.O]
    positions = [
        [2.00000000, 0.00000000, 0.00000000],
        [0.00000000, 3.00000000, 1.00000000],
    ]
    velocities = [
        [0.25000000, 0.00000000, 0.25000000],
        [0.00000000, 2.00000000, 0.00000000],
    ]
    forces = [
        [0.00000000, 0.06250000, 0.00000000],
        [1.00000000, 0.00000000, 1.25000000],
    ]

    [AtomTypes.H]      
    [AtomTypes.O]
    )"sv;
};

/**
 * @brief Verify that we can parse the atomic state from a configuration TOML.
 */
TEST_F(TestAtomicStateReader, ParseAtomicState) {
  ASSERT_EQ(atomic_state.num_atoms(), 4);

  ASSERT_EQ(atomic_state.num_atoms(atom_types["H"]), 2);
  ASSERT_EQ(atomic_state.atom_type(0), atom_types["H"]);
  ASSERT_EQ(atomic_state.atom_type(1), atom_types["H"]);
  Tensor<double, 2>::const_iterator h_pos = atomic_state.pos(0);
  ASSERT_EQ(*h_pos++, 0.0);
  ASSERT_EQ(*h_pos++, 0.0);
  ASSERT_EQ(*h_pos++, 0.0);
  ASSERT_EQ(*h_pos++, 0.0);
  ASSERT_EQ(*h_pos++, 0.0);
  ASSERT_EQ(*h_pos, 1.0);

  ASSERT_EQ(atomic_state.num_atoms(atom_types["O"]), 2);
  ASSERT_EQ(atomic_state.atom_type(2), atom_types["O"]);
  ASSERT_EQ(atomic_state.atom_type(3), atom_types["O"]);
  Tensor<double, 2>::const_iterator o_pos = atomic_state.pos(2);
  ASSERT_EQ(*o_pos++, 2.0);
  ASSERT_EQ(*o_pos++, 0.0);
  ASSERT_EQ(*o_pos++, 0.0);
  ASSERT_EQ(*o_pos++, 0.0);
  ASSERT_EQ(*o_pos++, 3.0);
  ASSERT_EQ(*o_pos, 1.0);
}

/**
 * @brief Verify that we can parse the atomic state, including velocity and
 * forces, from a configuration TOML.
 */
TEST_F(TestAtomicStateReader, VelocityAndForceInitialisation) {
  Tensor<double, 2>::const_iterator h_vel = atomic_state_vel_force.vel(0);
  ASSERT_EQ(*h_vel++, 0.5);
  ASSERT_EQ(*h_vel++, 0.0);
  ASSERT_EQ(*h_vel++, 0.5);
  ASSERT_EQ(*h_vel++, 0.0);
  ASSERT_EQ(*h_vel++, 1.0);
  ASSERT_EQ(*h_vel, 0.0);
  Tensor<double, 2>::const_iterator h_force = atomic_state_vel_force.force(0);
  ASSERT_EQ(*h_force++, 0.0);
  ASSERT_EQ(*h_force++, 0.125);
  ASSERT_EQ(*h_force++, 0.0);
  ASSERT_EQ(*h_force++, 2.0);
  ASSERT_EQ(*h_force++, 0.0);
  ASSERT_EQ(*h_force, 2.5);

  Tensor<double, 2>::const_iterator o_vel = atomic_state_vel_force.vel(2);
  ASSERT_EQ(*o_vel++, 0.25);
  ASSERT_EQ(*o_vel++, 0.0);
  ASSERT_EQ(*o_vel++, 0.25);
  ASSERT_EQ(*o_vel++, 0.0);
  ASSERT_EQ(*o_vel++, 2.0);
  ASSERT_EQ(*o_vel, 0.0);
  Tensor<double, 2>::const_iterator o_force = atomic_state_vel_force.force(2);
  ASSERT_EQ(*o_force++, 0.0);
  ASSERT_EQ(*o_force++, 0.0625);
  ASSERT_EQ(*o_force++, 0.0);
  ASSERT_EQ(*o_force++, 1.0);
  ASSERT_EQ(*o_force++, 0.0);
  ASSERT_EQ(*o_force, 1.25);
}
