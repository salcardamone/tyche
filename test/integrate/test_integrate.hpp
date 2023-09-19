/**
 * @brief
 */
#ifndef __TYCHE_TEST_INTEGRATE_TEST_INTEGRATE_HPP
#define __TYCHE_TEST_INTEGRATE_TEST_INTEGRATE_HPP

// C++ Standard Libraries
//
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/cell.hpp"
#include "tyche/integrate/integrate.hpp"
#include "test/force/test_lennard_jones.hpp"

using namespace tyche;

/**
 * @brief Test fixture for integrators using Lennard-Jones Argon dimer in
 * equilibrium configuration.
 */
template <typename Integrator>
class TestIntegrateLennardJonesEquilibrium
    : public TestLennardJonesEquilibrium {
 public:
  /**
   * @brief Initialise the fixture with a simulation cell.
   */
  void SetUp(std::shared_ptr<Cell> input_cell) {
    TestLennardJonesEquilibrium::SetUp();
    integrator = std::make_unique<Integrator>(dt);
    cell = input_cell;
  }

  /**
   * @brief Initialise the fixture and create an unbounded simulation cell by
   * default.
   */
  void SetUp() {
    TestLennardJonesEquilibrium::SetUp();
    integrator = std::make_unique<Integrator>(dt);
    cell = std::make_shared<UnboundedCell>();
  }

 protected:
  std::unique_ptr<Integrator> integrator;
  std::shared_ptr<Cell> cell;
  const double dt = 1;
};

#endif /* #ifndef __TYCHE_TEST_INTEGRATE_TEST_INTEGRATE_HPP */
