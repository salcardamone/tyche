/**
 * @brief
 */
#ifndef __TYCHE_ATOMIC_STATE_HPP
#define __TYCHE_ATOMIC_STATE_HPP

// C++ Standard Libraries
#include <map>
#include <memory>
// Third-Party Libraries
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/util/tensor.hpp"
#include "tyche/atom/atom_type.hpp"

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
   * @param needs_force Boolean flag dictating whether we need to
   * allocate arrays to hold atomic force information.
   */
  AtomicState(bool needs_velocity = false, bool needs_force = false)
      : needs_velocity_(needs_velocity), needs_force_(needs_force) {}

  /**
   * @brief Add atoms to the atomic state.
   * @param atom_types Vector of atom types; one for each atom added.
   * @param pos Positional information for each atom.
   * @param vel Velocity information for each atom. Optional; if velocity is
   * required for the simulation and this isn't provided, velocities will be
   * initialised with zero.
   * @param force Force information for each atom. Optional; if force is
   * required for the simulation and this isn't provided, forces will be
   * initialised with zero.
   */
  void add(std::vector<std::shared_ptr<AtomType>>&& atom_types,
           Tensor<double, 2>&& pos,
           std::optional<Tensor<double, 2>>&& vel = std::nullopt,
           std::optional<Tensor<double, 2>>&& force = std::nullopt) {
    // Move the atom types and positional information into the state
    atom_types_ = std::move(atom_types);
    pos_ = std::move(pos);

    // Count up the number of atoms of each atom type we have
    std::size_t num_atom_types = 0;
    for (const auto& atom_type : atom_types_) {
      if (!num_atoms_.count(atom_type)) {
        num_atoms_.insert({atom_type, 0});
        atom_type_idx_.insert({atom_type, num_atom_types++});
      }
      num_atoms_.at(atom_type) += 1;
    }

    // Initialise velocities and forces as zero if we need this information for
    // the simulation and data not provided. Else just move the provided data.
    if (needs_velocity_) {
      vel_ = std::move(vel.value_or(Tensor<double, 2>(num_atoms(), 3)));
    }
    if (needs_force_) {
      force_ = std::move(force.value_or(Tensor<double, 2>(num_atoms(), 3)));
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
      std::optional<std::shared_ptr<AtomType>> atom_type = std::nullopt) const {
    return atom_type ? num_atoms_.at(*atom_type)
                     : std::accumulate(num_atoms_.begin(), num_atoms_.end(), 0,
                                       [](std::size_t value, auto& elem) {
                                         return value + elem.second;
                                       });
  }

  /**
   * @brief Return constant iterator for an atom's positional information.
   * @param iatom The index of the atom.
   * @return Constant iterator to atom's positional information.
   */
  const Tensor<double, 2>::const_iterator pos(std::size_t iatom = 0) const {
    return pos_.begin() + 3 * iatom;
  }

  /**
   * @brief Return iterator for an atom's positional information.
   * @param iatom The index of the atom.
   * @return Iterator to atom's positional information.
   */
  Tensor<double, 2>::iterator pos(std::size_t iatom = 0) {
    return pos_.begin() + 3 * iatom;
  }

  /**
   * @brief Return constant iterator for an atom's velocity information.
   * @param iatom The index of the atom.
   * @return Constant iterator to atom's velocity information.
   */
  const Tensor<double, 2>::const_iterator vel(std::size_t iatom = 0) const {
    return vel_.begin() + 3 * iatom;
  }

  /**
   * @brief Return iterator for an atom's velocity information.
   * @param iatom The index of the atom.
   * @return Iterator to atom's velocity information.
   */
  Tensor<double, 2>::iterator vel(std::size_t iatom = 0) {
    return vel_.begin() + 3 * iatom;
  }

  /**
   * @brief Return constant iterator for an atom's force information.
   * @param iatom The index of the atom.
   * @return Constant iterator to atom's force information.
   */
  const Tensor<double, 2>::const_iterator force(std::size_t iatom = 0) const {
    return force_.begin() + 3 * iatom;
  }

  /**
   * @brief Return iterator for an atom's force information.
   * @param iatom The index of the atom.
   * @return Iterator to atom's force information.
   */
  Tensor<double, 2>::iterator force(std::size_t iatom = 0) {
    return force_.begin() + 3 * iatom;
  }

  /**
   * @brief Zero the tensor containing atomic forces.
   */
  void zero_forces() { force_.zero(); }

  /**
   * @brief Compute the kinetic energy of either a single atom, or the entire
   * atomic state.
   * @param iatom Index of the atom to compute the kinetic energy for. If
   * nullopt, then will compute sum of atomic kinetic energies.
   * @return The kinetic energy requested.
   */
  double kinetic(std::optional<std::size_t> iatom = std::nullopt) const {
    if (iatom) {
      return 0.5 * atom_types_[*iatom]->mass() *
             vel_.inner_product<0>(*iatom, *iatom);
    }

    double kin = 0;
    Tensor<double, 2>::const_iterator velocity = vel();
    for (std::size_t jatom = 0; jatom < num_atoms(); ++jatom) {
      kin += atom_types_[jatom]->mass() * vel_.inner_product<0>(jatom, jatom);
      velocity += 3;
    }
    return 0.5 * kin;
  }

  /**
   * @brief Compute the average kinetic energy across the atomic state.
   * @return The average kinetic energy across the atomic state.
   */
  double average_kinetic() const { return kinetic() / num_atoms(); }

  /**
   * @brief Return atom type corresponding to given atom index.
   * @param iatom The index of the atom.
   * @return Shared pointer to the corresponding atom type.
   */
  std::shared_ptr<AtomType> atom_type(std::size_t iatom) const {
    return atom_types_[iatom];
  }

  /**
   * @brief Retrieve the number of distinct atom types in the system
   * @return The number of atom types.
   */
  std::size_t num_atom_types() const { return atom_types_.size(); }

  /**
   * @brief Get the mapping from atom type to unique index of the atom type,
   * on [0,num_atom_types), which can be used for indexing into arrays that span
   * the number of atom types.
   * @return The map from atom type to unique index.
   */
  const std::map<std::shared_ptr<AtomType>, std::size_t>& atom_type_idx()
      const {
    return atom_type_idx_;
  }

  /**
   * @brief Get the unique index associated with a particular atom type.
   * @param atom_type The atom type to find the index of.
   * @return The unique index of the atom type.
   */
  std::size_t atom_type_idx(std::shared_ptr<AtomType> atom_type) const {
    return atom_type_idx_.at(atom_type);
  }

 private:
  bool needs_velocity_, needs_force_;
  std::map<std::shared_ptr<AtomType>, std::size_t> num_atoms_, atom_type_idx_;
  Tensor<double, 2> pos_, vel_, force_;
  std::vector<std::shared_ptr<AtomType>> atom_types_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_ATOMIC_STATE_HPP */