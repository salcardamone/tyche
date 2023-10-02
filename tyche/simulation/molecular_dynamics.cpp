/**
 * @brief
 */
// Standard Libraries
#include <memory>
// Third-party Libraries
//
// Project Inclusions
#include "tyche/atom/atomic_state.hpp"
#include "tyche/simulation/molecular_dynamics.hpp"
#include "tyche/simulation/molecular_dynamics_builder.hpp"

using namespace tyche;

MolecularDynamicsBuilder MolecularDynamics::create(std::shared_ptr<AtomicState> atomic_state) {
  return MolecularDynamicsBuilder(atomic_state);
}
