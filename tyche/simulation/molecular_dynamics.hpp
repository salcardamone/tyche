/**
 * @brief
 */
#ifndef __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_HPP
#define __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_HPP

// C++ Standard Libraries
#include <memory>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/simulation/simulation.hpp"
#include "tyche/atom/atomic_state.hpp"
#include "tyche/system/cell.hpp"
#include "tyche/integrate/integrate.hpp"
#include "tyche/force/force.hpp"

namespace tyche {

// Forward-declaration of the builder for MolecularDynamics objects
class MolecularDynamicsBuilder;

/**
 * @brief Molecular dynamics simulation.
 */
class MolecularDynamics : public Simulation {
 public:
  /**
   * @brief Class constructor.
   */
  MolecularDynamics() {
    // We're only going to invoke Forces::add when we build the
    // MolecularDynamics object, rather than assign a new Forces to the
    // unique_ptr as we do with the other member variables. Hence we need to
    // explicitly construct a Forces object
    forces_ = std::make_unique<Forces>();
  }

  /**
   * @brief Run the simulation as parameterised.
   */
  void run() override {
    forces_->evaluate(*atomic_state_, *cell_);
    for (std::size_t istep = 0; istep < num_steps_; ++istep) {
      integrator_->step(*atomic_state_, *forces_, *cell_);
    }
  }

  /**
   * @brief Create a new instance of the MolecularDynamicsBuilder.
   * @return A new MolecularDynamicsBuilder instance.
   */
  static MolecularDynamicsBuilder create(
      std::shared_ptr<AtomicState> atomic_state);

  friend MolecularDynamicsBuilder;

 private:
  std::size_t num_steps_;
  std::shared_ptr<AtomicState> atomic_state_;
  std::unique_ptr<Cell> cell_;
  std::unique_ptr<Integrate> integrator_;
  std::unique_ptr<Forces> forces_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_HPP */
