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
#include "tyche/system/cell.hpp"
#include "tyche/atom/atom_type_reader.hpp"
#include "tyche/atom/dynamic_atomic_state.hpp"
#include "tyche/atom/atomic_state_reader.hpp"

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
    AtomTypeReader atom_type_reader;
    atom_types = atom_type_reader.parse(*config["AtomTypes"].as_table());

    DynamicAtomicStateReader atomic_state_reader(atom_types);
    atomic_state = std::make_shared<DynamicAtomicState>(
        std::move(atomic_state_reader.parse(*config["Atoms"].as_table())));

    cell = std::make_unique<UnboundedCell>();
  }

 protected:
  std::unique_ptr<Cell> cell;
  std::shared_ptr<DynamicAtomicState> atomic_state;
  std::map<std::string, std::shared_ptr<AtomType>> atom_types;

  // Lennard-Jones parameters are taken from
  // https://openkim.org/id/MO_959249795837_003
  static constexpr std::string_view toml = R"(
    [Atoms.Ar]
    positions = [
        [0.00000000, 0.00000000, 0.00000000],
        [0.00000000, 0.00000000, 0.00000000],
    ]

    [AtomTypes.Ar]
    sigma_lj = 3.405
    eps_lj = 0.000119188
  )"sv;
};

#endif /* #ifndef __TYCHE_TEST_BASE_FIXTURES_ARGON_DIMER_HPP */
