/**
 * @brief
 */
#ifndef __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_HPP
#define __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_HPP

// C++ Standard Libraries
#include <memory>
#include <vector>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/io/writer.hpp"
#include "tyche/atom/dynamic_atomic_state.hpp"
#include "tyche/atom/atomic_state_writer.hpp"
#include "tyche/system/cell.hpp"
#include "tyche/integrate/integrate.hpp"
#include "tyche/force/force.hpp"
#include "tyche/simulation/simulation.hpp"
#include "tyche/system/thermostat.hpp"

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
  MolecularDynamics();

  /**
   * @brief Run the simulation as parameterised.
   */
  void run() override;

  /**
   * @brief Create a new instance of the MolecularDynamicsBuilder.
   * @return A new MolecularDynamicsBuilder instance.
   */
  static MolecularDynamicsBuilder create(
      std::shared_ptr<DynamicAtomicState> atomic_state);

  friend MolecularDynamicsBuilder;

 private:
  /**
   * @brief Collection of control variables and corresponding writer.
   */
  struct WriterConfig {
    //< Frequency of writing; number of steps between writes
    std::size_t frequency;
    //< The writer we write to
    std::unique_ptr<Writer> writer;
  };

  std::shared_ptr<DynamicAtomicState> atomic_state_;
  std::unique_ptr<Cell> cell_;
  std::shared_ptr<Integrate> integrator_;
  std::unique_ptr<Forces> forces_;
  std::vector<WriterConfig> writers_;

  /**
   * @brief Write to all writers registered to the simulation.
   * @param istep The current step of the simulation.
   */
  void write(std::size_t istep);
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_HPP */
