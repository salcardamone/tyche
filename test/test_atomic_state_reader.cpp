/**
 * @brief
 */
// C++ Standard Libraries
//
// Third-Party Libraries
#include <gtest/gtest.h>
// Project Inclusions
#include "tyche/atom_type_reader.hpp"
#include "tyche/atomic_state_reader.hpp"

using namespace tyche;
using namespace std::string_view_literals;

static constexpr std::string_view basic_toml = R"(
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

/**
 * @brief Verify that we can parse the atomic state from a configuration TOML.
 */
TEST(TestAtomicStateReader, ParseAtomicState) {
  toml::table config = toml::parse(basic_toml);
  AtomTypeReader reader(config);
  auto atom_types = reader.parse();

  AtomicStateReader reader2(config);
  auto atomic_state = reader2.parse(atom_types);
  ASSERT_EQ(atomic_state.num_atoms(), 4);

  ASSERT_EQ(atomic_state.num_atoms(atom_types["H"]), 2);
  auto h_pos = atomic_state.pos(atom_types["H"]);
  ASSERT_EQ(h_pos(0, 0), 0.0);
  ASSERT_EQ(h_pos(1, 0), 0.0);
  ASSERT_EQ(h_pos(2, 0), 0.0);
  ASSERT_EQ(h_pos(0, 1), 0.0);
  ASSERT_EQ(h_pos(1, 1), 0.0);
  ASSERT_EQ(h_pos(2, 1), 1.0);

  ASSERT_EQ(atomic_state.num_atoms(atom_types["O"]), 2);
  auto o_pos = atomic_state.pos(atom_types["O"]);
  ASSERT_EQ(o_pos(0, 0), 2.0);
  ASSERT_EQ(o_pos(1, 0), 0.0);
  ASSERT_EQ(o_pos(2, 0), 0.0);
  ASSERT_EQ(o_pos(0, 1), 0.0);
  ASSERT_EQ(o_pos(1, 1), 3.0);
  ASSERT_EQ(o_pos(2, 1), 1.0);
}
