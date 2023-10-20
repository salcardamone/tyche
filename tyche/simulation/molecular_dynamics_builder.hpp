/**
 * @brief
 */
#ifndef __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_BUILDER_HPP
#define __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_BUILDER_HPP

// C++ Standard Libraries
#include <memory>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/io/reader.hpp"
#include "tyche/atom/dynamic_atomic_state.hpp"
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
  MolecularDynamicsBuilder(std::shared_ptr<DynamicAtomicState> atomic_state);

  /**
   * @brief Set the integrator for the MolecularDynamics object.
   * @param map Mapping containing integrator creation parameters.
   * @return The modified builder.
   */
  MolecularDynamicsBuilder& integrator(Reader::Mapping map);

  /**
   * @brief Set force evaluation object for the MolecularDynamics object.
   * @param map Mapping containing force creation parameters.
   * @return The modified builder.
   */
  MolecularDynamicsBuilder& force(Reader::Mapping map);

  /**
   * @brief Create a Cell for the MolecularDynamics object.
   * @param map Mapping containing cell creation parameters.
   * @return The modified builder.
   */
  MolecularDynamicsBuilder& cell(Reader::Mapping map);

  /**
   * @brief Create a Writer for the MolecularDynamics object.
   * @param map Mapping containing writer creation parameters.
   * @return The modified builder.
   */
  MolecularDynamicsBuilder& output(Reader::Mapping map);

  /**
   * @brief Return the built MolecularDynamics object.
   * @return The final MolecularDynamics object.
   */
  MolecularDynamics build();

 private:
  MolecularDynamics simulation_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_BUILDER_HPP */
