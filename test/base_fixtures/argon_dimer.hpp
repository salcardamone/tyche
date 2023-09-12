/**
 * @brief
 */
#ifndef __TYCHE_TEST_BASE_FIXTURES_ARGON_DIMER_HPP
#define __TYCHE_TEST_BASE_FIXTURES_ARGON_DIMER_HPP

// C++ Standard Libraries
//
// Third-Party Libraries
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/atom_type_reader.hpp"
#include "tyche/atomic_state_reader.hpp"

using namespace tyche;
using namespace std::string_view_literals;

/**
 * @brief Argon dimer base test fixture.
 */
class ArgonDimer : public ::testing::Test {
 public:
  /**
   * @brief Initialise the atomic state and atom types.
   */
  void SetUp() {
    toml::table config = toml::parse(toml);
    AtomTypeReader reader(config);
    atom_types = reader.parse();

    AtomicStateReader reader2(config);
    atomic_state = reader2.parse(atom_types, true, true);
  }

 protected:
  std::map<std::string, std::shared_ptr<AtomType>> atom_types;
  AtomicState atomic_state;

  static constexpr std::string_view toml = R"(
    [Atoms.Ar]
    positions = [
        [0.00000000, 0.00000000, 0.00000000],
        [0.00000000, 0.00000000, 0.00000000],
    ]

    [AtomTypes.Ar]      
  )"sv;
};

#endif /* #ifndef __TYCHE_TEST_BASE_FIXTURES_ARGON_DIMER_HPP */
