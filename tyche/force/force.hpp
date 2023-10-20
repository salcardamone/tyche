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
#include "tyche/system/cell.hpp"
#include "tyche/atom/dynamic_atomic_state.hpp"

namespace tyche {

/**
 * @brief Abstract base class for all forces.
 */
class Force {
 public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~Force() = default;

  /**
   * @brief Abstract base evaluation method for force.
   * @param state The atomic state for which we're calculating forces for.
   * @return The total energy associated with the forces.
   */
  virtual double evaluate(DynamicAtomicState& state, const Cell& cell) = 0;
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
  double evaluate(DynamicAtomicState& state, const Cell& cell) override {
    // We're going to accumulate to the force_ component of the atomic state, so
    // zero before starting
    state.zero_forces();

    double pot = 0;
    for (auto& force : forces_) {
      pot += force->evaluate(state, cell);
    }
    return pot;
  }

  /**
   * @brief Add a Force object to the iterable of other force objects already
   * registered.
   * @param force An object which derives from Force, i.e. it provides an
   * evaluate method.
   */
  void add(std::unique_ptr<Force> force) {
    forces_.push_back(std::move(force));
  }

 private:
  std::vector<std::unique_ptr<Force>> forces_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_FORCE_FORCE_HPP */
