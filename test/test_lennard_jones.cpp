/**
 * @brief
 */
// C++ Standard Libraries
//
// Third-Party Libraries
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/atom_type_reader.hpp"
#include "tyche/atomic_state_reader.hpp"
#include "tyche/force/lennard_jones.hpp"

using namespace tyche;
using namespace std::string_view_literals;

static constexpr std::string_view basic_toml = R"(
    [Atoms.Ar]
    positions = [
        [0.00000000, 0.00000000, 0.00000000],
        [0.00000000, 0.00000000, 0.00000000],
    ]

    [AtomTypes.Ar]      
)"sv;

/**
 * @brief Verify that we can find the minimum of the Lennard-Jones potential for
 * two Argon atoms, and that the forces between the atoms disappears at this
 * point.
 */
TEST(TestLennardJones, MinimumDistance) {
  spdlog::set_level(spdlog::level::off);

  toml::table config = toml::parse(basic_toml);
  AtomTypeReader reader(config);
  auto atom_types = reader.parse();

  AtomicStateReader reader2(config);
  auto atomic_state = reader2.parse(atom_types, true, true);

  // Lennard-Jones minimum value of the potential is found at 2^(1/6) * sigma
  *atomic_state.pos(0) = std::pow(2, 1. / 6.) * atom_types["Ar"]->sigma_lj();

  LennardJones lj(atomic_state.atom_type_idx_);
  // Potential at rmin should be -epsilon
  double pot = lj.evaluate(atomic_state);
  ASSERT_EQ(-atom_types["Ar"]->eps_lj(), pot);

  // Forces between atoms should be zero
  auto forces = atomic_state.force(0);
  ASSERT_NEAR(forces[0], 0.0, 1E-15);
  ASSERT_NEAR(forces[1], 0.0, 1E-15);
  ASSERT_NEAR(forces[2], 0.0, 1E-15);
  ASSERT_NEAR(forces[3], 0.0, 1E-15);
  ASSERT_NEAR(forces[4], 0.0, 1E-15);
  ASSERT_NEAR(forces[5], 0.0, 1E-15);
}
