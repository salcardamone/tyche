/**
 * @brief
 */
// C++ Standard Libraries
#include <iostream>
// Third-Party Libraries
#include <gtest/gtest.h>
// Project Inclusions
#include "tyche/atom_type_reader.hpp"

using namespace tyche;
using namespace std::string_view_literals;

static constexpr std::string_view basic_toml = R"(
    [AtomTypes.H]
    mass = 1.0
    nuclear_charge = 1
    num_electrons = 1
      
    [AtomTypes.O]
    mass = 16.0
    nuclear_charge = 8
    num_electrons = 8
    )"sv;

TEST(TestAtomTypeReader, ParseAtomType) {
  toml::table config = toml::parse(basic_toml);
  AtomTypeReader reader(config);
  auto atom_types = reader.parse();

  ASSERT_EQ(atom_types["H"]->mass(), 1.0);
  ASSERT_EQ(atom_types["H"]->nuclear_charge(), 1);
  ASSERT_EQ(atom_types["H"]->num_electrons(), 1);

  ASSERT_EQ(atom_types["O"]->mass(), 16.0);
  ASSERT_EQ(atom_types["O"]->nuclear_charge(), 8);
  ASSERT_EQ(atom_types["O"]->num_electrons(), 8);
}
