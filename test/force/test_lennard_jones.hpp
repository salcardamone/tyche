/**
 * @brief
 */
#ifndef __TYCHE_TEST_FORCE_TEST_LENNARD_JONES_HPP
#define __TYCHE_TEST_FORCE_TEST_LENNARD_JONES_HPP

// C++ Standard Libraries
//
// Third-Party Libraries
//
// Project Inclusions
#include "test/base_fixtures/argon_box.hpp"
#include "test/base_fixtures/argon_dimer.hpp"
#include "tyche/force/lennard_jones.hpp"

using namespace tyche;

/**
 * @brief Lennard-Jones Argon dimer where distance between the two is at the
 * potential minimum.
 */
class TestLennardJonesEquilibrium : public ArgonDimer {
 public:
  /**
   * @brief Initialise the atomic state to be at the potential minimum.
   */
  void SetUp() {
    ArgonDimer::SetUp();
    lj = std::make_unique<LennardJones>(atomic_state.atom_type_idx_);

    // Lennard-Jones minimum value of the potential is found at 2^(1/6) * sigma
    rij_min = std::pow(2, 1. / 6.) * atom_types["Ar"]->sigma_lj();
    *atomic_state.pos(0) = rij_min;
  }

 protected:
  double rij_min;
  std::unique_ptr<LennardJones> lj;
};

class TestLennardJonesCrystal : public ArgonBox {
public:
  void SetUp(std::size_t num_atoms) {
    ArgonBox::SetUp(num_atoms);
    lj = std::make_unique<LennardJones>(atomic_state.atom_type_idx_);
  }
  
protected:
  std::unique_ptr<LennardJones> lj;
};

#endif /* #ifndef __TYCHE_TEST_FORCE_TEST_LENNARD_JONES_HPP */
