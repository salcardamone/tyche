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
   * @brief
   */
  void SetUp() {
    TestLennardJonesEquilibrium::SetUp();
    integrator = std::make_unique<Integrator>(dt);
  }

 protected:
  std::unique_ptr<Integrator> integrator;
  const double dt = 1E-15;
};

#endif /* #ifndef __TYCHE_TEST_INTEGRATE_TEST_INTEGRATE_HPP */
