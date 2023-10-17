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
    AtomTypeReader reader(config);
    atom_types = reader.parse();

    AtomicStateReader reader2(config);
    atomic_state = std::make_shared<DynamicAtomicState>(
        std::move(reader2.parse_dynamic_atomic_state(atom_types)));

    cell = std::make_unique<UnboundedCell>();
  }

 protected:
  std::unique_ptr<Cell> cell;
  std::shared_ptr<DynamicAtomicState> atomic_state;
  std::map<std::string, std::shared_ptr<AtomType>> atom_types;

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
