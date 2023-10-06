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
#include "tyche/atom/atomic_state.hpp"
#include "tyche/atom/atomic_state_writer.hpp"
#include "tyche/system/cell.hpp"
#include "tyche/integrate/integrate.hpp"
#include "tyche/force/force.hpp"
#include "tyche/simulation/simulation.hpp"

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
    for (std::size_t istep = 0; istep < integrator_->num_steps(); ++istep) {
      integrator_->step(*atomic_state_, *forces_, *cell_);
      write(istep);
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
  /**
   * @brief Collection of control variables and corresponding writer.
   */
  struct WriterConfig {
    //< Frequency of writing; number of steps between writes
    std::size_t frequency;
    //< The writer we write to
    std::unique_ptr<Writer> writer;
  };

  std::shared_ptr<AtomicState> atomic_state_;
  std::unique_ptr<Cell> cell_;
  std::unique_ptr<Integrate> integrator_;
  std::unique_ptr<Forces> forces_;
  std::vector<WriterConfig> writers_;

  /**
   * @brief Write to all writers registered to the simulation.
   * @param istep The current step of the simulation.
   */
  void write(std::size_t istep) {
    std::string comment = "Step " + std::to_string(istep);
    for (auto& writer : writers_) {
      if (!(istep % writer.frequency)) {
        writer.writer->write(comment);
      }
    }
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SIMULATION_MOLECULAR_DYNAMICS_HPP */
