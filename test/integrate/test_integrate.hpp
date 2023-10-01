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
#include "tyche/system/cell.hpp"
#include "tyche/integrate/integrate.hpp"
#include "test/force/test_lennard_jones.hpp"

using namespace tyche;

/**
 * @brief Lennard-Jones system integrator.
 * @tparam Integrator The integrator to use, deriving from Integrate.
 * @tparam LennardJonesSystem The atomic system with Lennard-Jones evaluator.
 */
template <class Integrator, class LennardJonesSystem>
class TestIntegrateLennardJones : public LennardJonesSystem {
 public:
  
  /**
   * @brief Initialise the Lennard-Jones system integrator.
   * @param args Variadic arguments to forward to the LennardJonesSystem::SetUp
   * method.
   */
  template <class... Args>
  void SetUp(Args... args) {
    LennardJonesSystem::SetUp(args...);
    forces.add([this](AtomicState& state, std::shared_ptr<Cell> cell) {
      return this->lj->evaluate(state, cell);
    });
    integrator = std::make_unique<Integrator>(dt);
  }

 protected:
  Forces forces;
  std::unique_ptr<Integrator> integrator;
  static constexpr double dt = 1;
};

/**
 * @brief Test fixture for integrator using the Lennard-Jones Argon dimer at
 * equilibrium.
 * @tparam Integrator The integrator to use, deriving from Integrate.
 */
template <class Integrator>
class TestIntegrateLennardJonesEquilibrium
    : public TestIntegrateLennardJones<Integrator,
                                       TestLennardJonesEquilibrium> {
 public:
  /**
   * @brief Initialise the fixture with a simulation cell.
   */
  void SetUp(std::shared_ptr<Cell> input_cell) {
    TestIntegrateLennardJones<Integrator, TestLennardJonesEquilibrium>::SetUp();
    this->cell = input_cell;
  }

  /**
   * @brief Initialise the fixture and create an unbounded simulation cell by
   * default.
   */
  void SetUp() {
    TestIntegrateLennardJones<Integrator, TestLennardJonesEquilibrium>::SetUp();
  }
};

/**
 * @brief Test fixture for integrator using a Lennard-Jones Argon crystal.
 * @tparam Integrator The integrator to use, deriving from Integrate.
 */
template <class Integrator>
class TestIntegrateLennardJonesCrystal
    : public TestIntegrateLennardJones<Integrator, TestLennardJonesCrystal> {
 public:
  /**
   * @brief Initialise the fixture with some given number of atoms that'll be
   * arranged into a crystal.
   */
  void SetUp(std::size_t num_atoms,
             std::optional<double> density = std::nullopt) {
    TestIntegrateLennardJones<Integrator, TestLennardJonesCrystal>::SetUp(
        num_atoms, density);
  }
};

#endif /* #ifndef __TYCHE_TEST_INTEGRATE_TEST_INTEGRATE_HPP */
