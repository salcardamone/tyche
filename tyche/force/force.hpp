/**
 * @brief
 */
#ifndef __TYCHE_FORCE_FORCE_HPP
#define __TYCHE_FORCE_FORCE_HPP

// C++ Standard Libraries
//
// Third-Party Libraries
//
// Project Inclusions
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
  virtual double evaluate(AtomicState& state) = 0;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_FORCE_FORCE_HPP */
