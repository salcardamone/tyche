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

template <typename Integrator, class LennardJonesSystem>
class TestIntegrateLennardJones : public LennardJonesSystem {
 public:
  template <class... Args>
  void SetUp(Args... args) {
    LennardJonesSystem::SetUp(args...);
    forces.add(
        [this](AtomicState& state) { return this->lj->evaluate(state); });
    integrator = std::make_unique<Integrator>(dt);
  }

 protected:
  Forces forces;
  std::unique_ptr<Integrator> integrator;
  const double dt = 1;
};

/**
 * @brief Test fixture for integrators using Lennard-Jones Argon dimer in
 * equilibrium configuration.
 */
template <typename Integrator>
class TestIntegrateLennardJonesEquilibrium
    : public TestIntegrateLennardJones<Integrator,
                                       TestLennardJonesEquilibrium> {
 public:
  /**
   * @brief Initialise the fixture with a simulation cell.
   */
  void SetUp(std::shared_ptr<Cell> input_cell) {
    TestIntegrateLennardJones<Integrator, TestLennardJonesEquilibrium>::SetUp();
    cell = input_cell;
  }

  /**
   * @brief Initialise the fixture and create an unbounded simulation cell by
   * default.
   */
  void SetUp() {
    TestIntegrateLennardJones<Integrator, TestLennardJonesEquilibrium>::SetUp();
    cell = std::make_shared<UnboundedCell>();
  }

 protected:
  std::shared_ptr<Cell> cell;
};

template <typename Integrator>
class TestIntegrateLennardJonesCrystal
    : public TestIntegrateLennardJones<Integrator, TestLennardJonesCrystal> {
 public:
  void SetUp() {
    TestIntegrateLennardJones<Integrator, TestLennardJonesCrystal>::SetUp(
        num_atoms);
  }

  static constexpr std::size_t num_atoms = 64;
};

#endif /* #ifndef __TYCHE_TEST_INTEGRATE_TEST_INTEGRATE_HPP */
