/**
 * @brief
 */
// Standard Libraries
#include <memory>
// Third-party Libraries
//
// Project Inclusions
#include "tyche/atom/dynamic_atomic_state.hpp"
#include "tyche/simulation/molecular_dynamics.hpp"
#include "tyche/simulation/molecular_dynamics_builder.hpp"

namespace tyche {

// ========================================================================== //

MolecularDynamics::MolecularDynamics() {
  // We're only going to invoke Forces::add when we build the
  // MolecularDynamics object, rather than assign a new Forces to the
  // unique_ptr as we do with the other member variables. Hence we need to
  // explicitly construct a Forces object
  forces_ = std::make_unique<Forces>();
}

// ========================================================================== //

MolecularDynamicsBuilder MolecularDynamics::create(
    std::shared_ptr<DynamicAtomicState> atomic_state) {
  return MolecularDynamicsBuilder(atomic_state);
}

// ========================================================================== //

void MolecularDynamics::run() {
  forces_->evaluate(*atomic_state_, *cell_);
  while (integrator_->current_step() < integrator_->num_steps()) {
    integrator_->step(*atomic_state_, *forces_, *cell_);
    write(integrator_->current_step());
  }
}

// ========================================================================== //

void MolecularDynamics::write(std::size_t istep) {
  std::string comment = "Step " + std::to_string(istep);
  for (auto& writer : writers_) {
    if (!(istep % writer.frequency)) {
      writer.writer->write(comment);
    }
  }
}

// ========================================================================== //

}  // namespace tyche
