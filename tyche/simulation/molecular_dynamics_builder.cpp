/**
 * @brief
 */
// C++ Standard Libraries
//
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/util/must.hpp"
#include "tyche/io/writer_factory.hpp"
#include "tyche/system/cell_factory.hpp"
#include "tyche/force/force_factory.hpp"
#include "tyche/integrate/integrate_factory.hpp"
#include "tyche/simulation/molecular_dynamics_builder.hpp"

namespace tyche {

// ========================================================================== //

MolecularDynamicsBuilder::MolecularDynamicsBuilder(
    std::shared_ptr<DynamicAtomicState> atomic_state) {
  simulation_.atomic_state_ = atomic_state;
}

// ========================================================================== //

MolecularDynamicsBuilder& MolecularDynamicsBuilder::integrator(
    Reader::Mapping map) {
  simulation_.integrator_ = std::move(IntegrateFactory::create(map));
  return *this;
}

// ========================================================================== //

MolecularDynamicsBuilder& MolecularDynamicsBuilder::force(Reader::Mapping map) {
  simulation_.forces_->add(
      ForceFactory::create(map, simulation_.atomic_state_->atom_type_idx()));
  return *this;
}

// ========================================================================== //

MolecularDynamicsBuilder& MolecularDynamicsBuilder::cell(Reader::Mapping map) {
  simulation_.cell_ = std::move(CellFactory::create(map));
  return *this;
}

// ========================================================================== //

MolecularDynamicsBuilder& MolecularDynamicsBuilder::output(
    Reader::Mapping map) {
  MolecularDynamics::WriterConfig writer_config;
  writer_config.writer = std::move(WriterFactory::create(
      map, simulation_.atomic_state_, simulation_.integrator_));
  writer_config.frequency = must_find<double>(map, "frequency");
  simulation_.writers_.push_back(std::move(writer_config));
  return *this;
}

// ========================================================================== //

MolecularDynamics MolecularDynamicsBuilder::build() {
  // Apply any post-construction manipulations to the atomic state
  simulation_.integrator_->initialise(*simulation_.atomic_state_);
  return std::move(simulation_);
}

// ========================================================================== //

}  // namespace tyche
