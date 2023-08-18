/**
 * @brief
 */
#ifndef __TYCHE_ATOMIC_STATE_HPP
#define __TYCHE_ATOMIC_STATE_HPP

// C++ Standard Libraries
#include <map>
#include <memory>
// Third-Party Libraries
#include <toml++/toml.h>
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/tensor.hpp"
#include "tyche/atom_type.hpp"

namespace tyche {

/**
 * @brief Wrapper for all atomic state within the simulation. Holds the
 * appropriate arrays depending on what type of simulation we're undertaking.
 */
class AtomicState {
 public:
  /**
   * @brief Class constructor.
   * @param needs_velocity Boolean flag dictating whether we need to allocate
   * arrays to hold atomic velocity information.
   * @param needs_acceleration Boolean flag dictating whether we need to
   * allocate arrays to hold atomic acceleration information.
   */
  AtomicState(bool needs_velocity = false, bool needs_acceleration = false)
      : needs_velocity_(needs_velocity),
        needs_acceleration_(needs_acceleration) {}

  /**
   * @brief Add atom positional information for a given atom type.
   * @param atom_type The AtomType we're adding atoms of.
   * @param pos 2D array of positional information
   * @param vel 2D array of velocity information. Optional; if the simulation
   * needs velocity information and this isn't provided, will allocate empty
   * tensor.
   * @param acc 2D array of acceleration information. Optional; if the
   * simulation needs velocity information and this isn't provided, will
   * allocate empty tensor.
   */
  void add(std::shared_ptr<AtomType> atom_type, Tensor<double, 2>&& pos,
           std::optional<Tensor<double, 2>> vel = std::nullopt,
           std::optional<Tensor<double, 2>> acc = std::nullopt) {
    // Atom type isn't in the map, so we can add it safely
    if (num_atoms_.find(atom_type) == num_atoms_.end()) {
      if (pos.size(0) != 3) {
        spdlog::error(
            "Atomic State position Tensor needs to have 3 rows. Passed Tensor"
            "with {} rows.",
            pos.size(0));
        std::exit(EXIT_FAILURE);
      }

      num_atoms_[atom_type] = pos.size(1);
      pos_[atom_type] = std::move(pos);
      spdlog::info("Adding {} atoms of Atom Type {} to the Atomic State.",
                   num_atoms_[atom_type], atom_type->id());

      if (needs_velocity_) {
        vel_[atom_type] =
            vel.value_or(Tensor<double, 2>(3, num_atoms_[atom_type]));
      }
      if (needs_acceleration_) {
        acc_[atom_type] =
            acc.value_or(Tensor<double, 2>(3, num_atoms_[atom_type]));
      }

      // Not really sure whether we need to handle this case; just error for now
    } else {
      spdlog::error("Atom Type {} has already been added to the Atomic State",
                    atom_type->id());
      std::exit(EXIT_FAILURE);
    }
  }

  /**
   * @brief Retrieve the number of atoms of a given atom type in the atomic
   * state. If no atom type is provided, just return the total number of atoms
   * across the atomic state.
   * @param atom_type The atom type we're querying the number of atoms of.
   * Optional; if no argument is provided, returns the total number of atoms
   * across the atomic state.
   */
  std::size_t num_atoms(
      std::optional<std::shared_ptr<AtomType>> atom_type = std::nullopt) {
    return atom_type ? num_atoms_.at(*atom_type)
                     : std::accumulate(num_atoms_.begin(), num_atoms_.end(), 0,
                                       [](std::size_t value, auto& elem) {
                                         return value + elem.second;
                                       });
  }

  /**
   * @brief Constant accessor for positional information of atoms of given atom
   * type.
   * @param atom_type The AtomType we're querying the positions of.
   * @return Constant reference to positional tensor for AtomType.
   */
  const Tensor<double, 2>& pos(std::shared_ptr<AtomType> atom_type) const {
    return pos_.at(atom_type);
  }

 private:
  bool needs_velocity_, needs_acceleration_;
  std::map<std::shared_ptr<AtomType>, std::size_t> num_atoms_;
  // Note that these are 2D tensors where the rows enumerate spatial
  // dimensions and columns the atom index
  std::map<std::shared_ptr<AtomType>, Tensor<double, 2>> pos_, vel_, acc_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_ATOMIC_STATE_HPP */
