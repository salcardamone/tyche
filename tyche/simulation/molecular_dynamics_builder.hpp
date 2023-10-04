/**
 * @brief
 */
#ifndef __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_BUILDER_HPP
#define __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_BUILDER_HPP

// C++ Standard Libraries
#include <memory>
#include <cstring>
#include <optional>
// Third-Party Libraries
#include <toml++/toml.h>
// Project Inclusions
#include "tyche/atom/atomic_state.hpp"
#include "tyche/system/cell_factory.hpp"
#include "tyche/force/force_factory.hpp"
#include "tyche/integrate/integrate_factory.hpp"
#include "tyche/simulation/molecular_dynamics.hpp"

namespace tyche {

/**
 * @brief Builder for a MolecularDynamics object.
 */
class MolecularDynamicsBuilder {
 public:
  /**
   * @brief Class constructor.
   * @param atomic_state The atomic state we're going to be performing the
   * simulation of. Note that forces potentially need atomic information
   * (Lennard-Jones, for instance, needs to know the epsilons and sigmas for
   * each atom type), hence we should make it available to the builder.
   */
  MolecularDynamicsBuilder(std::shared_ptr<AtomicState> atomic_state) {
    simulation_.atomic_state_ = atomic_state;
  }

  /**
   * @brief Set the integrator for the MolecularDynamics object.
   * @param map Mapping containing integrator creation parameters.
   * @return The modified builder.
   */
  MolecularDynamicsBuilder& integrator(std::map<std::string, std::any> map) {
    simulation_.integrator_ = std::move(IntegrateFactory::create(map));
    return *this;
  }

  /**
   * @brief Add force evaluation objects for the MolecularDynamics object.
   * @param forces An array of inline tables, where each entry has a "type"
   * field denoting the type of force to instantiate from the ForceFactory, as
   * well as any additional parameters that might be required.
   * @return The modified builder.
   */
  MolecularDynamicsBuilder& forces(toml::array& forces) {
    for (auto& val : forces) {
      toml::table force_config = *val.as_table();
      std::unique_ptr<Force> force = ForceFactory::create(
          force_config, simulation_.atomic_state_->atom_type_idx());
      simulation_.forces_->add(std::move(force));
    }
    return *this;
  }

  /**
   * @brief Create a Cell for the MolecularDynamics object.
   * @param map Mapping containing cell creation parameters.
   * @return The modified builder.
   */
  MolecularDynamicsBuilder& cell(std::map<std::string, std::any> map) {
    simulation_.cell_ = std::move(CellFactory::create(map));
    return *this;
  }

  /**
   * @brief Return the built MolecularDynamics object.
   * @return The final MolecularDynamics object.
   */
  MolecularDynamics build() { return std::move(simulation_); }

 private:
  MolecularDynamics simulation_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_BUILDER_HPP */
