/**
 * @brief
 */
#ifndef __TYCHE_FORCE_FORCE_HPP
#define __TYCHE_FORCE_FORCE_HPP

// C++ Standard Libraries
#include <functional>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/cell.hpp"
#include "tyche/atomic_state.hpp"

namespace tyche {

/**
 * @brief Abstract base class for all forces.
 */
class Force {
 public:
  /**
   * @brief Abstract base evaluation method for force.
   * @param state The atomic state for which we're calculating forces for.
   * @return The total energy associated with the forces.
   */
  virtual double evaluate(AtomicState& state, std::shared_ptr<Cell> cell) = 0;
};

/**
 * @brief Wrapper for all forces required for atomic state propagation.
 */
class Forces : public Force {
 public:
  /**
   * @brief Evaluate all forces for the argument atomic state.
   * @param state The atomic state.
   * @return The potential energy accumulated across all forces.
   */
  double evaluate(AtomicState& state, std::shared_ptr<Cell> cell) override {
    // We're going to accumulate to the force_ component of the atomic state, so
    // zero before starting
    state.zero_forces();

    double pot = 0;
    for (auto& force : forces_) {
      pot += force(state, cell);
    }
    return pot;
  }

  /**
   * @brief Add a force evaluation function to the iterable of other force
   * evaluation functions already registered.
   * @param eval The function which takes an AtomicState object and return the
   * potential energy from the force evaluation.
   */
  void add(std::function<double(AtomicState&, std::shared_ptr<Cell>)> eval) {
    forces_.push_back(eval);
  }

 private:
  std::vector<std::function<double(AtomicState&, std::shared_ptr<Cell>)>>
      forces_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_FORCE_FORCE_HPP */
