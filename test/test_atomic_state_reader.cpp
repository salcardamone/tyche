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
    toml::table static_config = toml::parse(static_toml);
    toml::table dynamic_config = toml::parse(dynamic_toml);
    AtomTypeReader reader(static_config);
    atom_types = reader.parse();

    AtomicStateReader static_reader(atom_types);
    static_atomic_state_a =
        static_reader.parse(*static_config["Atoms"].as_table());
    static_atomic_state_b =
        static_reader.parse(*dynamic_config["Atoms"].as_table());
    DynamicAtomicStateReader dynamic_reader(atom_types);
    dynamic_atomic_state_a =
        dynamic_reader.parse(*static_config["Atoms"].as_table());
    dynamic_atomic_state_b =
        dynamic_reader.parse(*dynamic_config["Atoms"].as_table());
  }

  std::map<std::string, std::shared_ptr<AtomType>> atom_types;
  AtomicState static_atomic_state_a, static_atomic_state_b;
  DynamicAtomicState dynamic_atomic_state_a, dynamic_atomic_state_b;

 private:
  static constexpr std::string_view static_toml = R"(
    [Atoms.H]
    positions = [
        [0.00000000, 0.00000000, 0.00000000],
        [0.00000000, 0.00000000, 1.00000000],
    ]

    [Atoms.O]
    positions = [
        [2.00000000, 0.00000000, 0.00000000],
        [0.00000000, 3.00000000, 1.00000000],
    ]

    [AtomTypes.H]      
    [AtomTypes.O]
    )"sv;
  static constexpr std::string_view dynamic_toml = R"(
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
TEST_F(TestAtomicStateReader, ParseStaticAtomicState) {
  ASSERT_EQ(static_atomic_state_a.num_atoms(), 4);

  ASSERT_EQ(static_atomic_state_a.num_atoms(atom_types["H"]), 2);
  ASSERT_EQ(static_atomic_state_a.atom_type(0), atom_types["H"]);
  ASSERT_EQ(static_atomic_state_a.atom_type(1), atom_types["H"]);
  Tensor<double, 2>::const_iterator h_pos = static_atomic_state_a.pos(0);
  ASSERT_EQ(*h_pos++, 0.0);
  ASSERT_EQ(*h_pos++, 0.0);
  ASSERT_EQ(*h_pos++, 0.0);
  ASSERT_EQ(*h_pos++, 0.0);
  ASSERT_EQ(*h_pos++, 0.0);
  ASSERT_EQ(*h_pos, 1.0);

  ASSERT_EQ(static_atomic_state_a.num_atoms(atom_types["O"]), 2);
  ASSERT_EQ(static_atomic_state_a.atom_type(2), atom_types["O"]);
  ASSERT_EQ(static_atomic_state_a.atom_type(3), atom_types["O"]);
  Tensor<double, 2>::const_iterator o_pos = static_atomic_state_a.pos(2);
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
TEST_F(TestAtomicStateReader, ParseDynamicAtomicState) {
  // Parse from TOML without dynamic information
  {
    Tensor<double, 2>::const_iterator vel = dynamic_atomic_state_a.vel(0);
    Tensor<double, 2>::const_iterator force = dynamic_atomic_state_a.force(0);
    for (std::size_t idx = 0; idx < 12; ++idx) {
      ASSERT_EQ(*vel++, 0.0);
      ASSERT_EQ(*force++, 0.0);
    }
  }

  // Parse from TOML with dynamic information
  {
    Tensor<double, 2>::const_iterator h_vel = dynamic_atomic_state_b.vel(0);
    ASSERT_EQ(*h_vel++, 0.5);
    ASSERT_EQ(*h_vel++, 0.0);
    ASSERT_EQ(*h_vel++, 0.5);
    ASSERT_EQ(*h_vel++, 0.0);
    ASSERT_EQ(*h_vel++, 1.0);
    ASSERT_EQ(*h_vel, 0.0);
    Tensor<double, 2>::const_iterator h_force = dynamic_atomic_state_b.force(0);
    ASSERT_EQ(*h_force++, 0.0);
    ASSERT_EQ(*h_force++, 0.125);
    ASSERT_EQ(*h_force++, 0.0);
    ASSERT_EQ(*h_force++, 2.0);
    ASSERT_EQ(*h_force++, 0.0);
    ASSERT_EQ(*h_force, 2.5);

    Tensor<double, 2>::const_iterator o_vel = dynamic_atomic_state_b.vel(2);
    ASSERT_EQ(*o_vel++, 0.25);
    ASSERT_EQ(*o_vel++, 0.0);
    ASSERT_EQ(*o_vel++, 0.25);
    ASSERT_EQ(*o_vel++, 0.0);
    ASSERT_EQ(*o_vel++, 2.0);
    ASSERT_EQ(*o_vel, 0.0);
    Tensor<double, 2>::const_iterator o_force = dynamic_atomic_state_b.force(2);
    ASSERT_EQ(*o_force++, 0.0);
    ASSERT_EQ(*o_force++, 0.0625);
    ASSERT_EQ(*o_force++, 0.0);
    ASSERT_EQ(*o_force++, 1.0);
    ASSERT_EQ(*o_force++, 0.0);
    ASSERT_EQ(*o_force, 1.25);
  }
}
