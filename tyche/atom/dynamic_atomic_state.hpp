/**
 * @brief
 */
#ifndef __TYCHE_ATOM_DYNAMIC_ATOMIC_STATE_HPP
#define __TYCHE_ATOM_DYNAMIC_ATOMIC_STATE_HPP

// C++ Standard Libraries
#include <map>
#include <memory>
#include <vector>
#include <optional>
// Third-Party Libraries
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/util/tensor.hpp"
#include "tyche/atom/atom_type.hpp"
#include "tyche/atom/atomic_state.hpp"

namespace tyche {

/**
 * @brief
 */
class DynamicAtomicState : public AtomicState {
 public:
  /**
   * @brief Class constructor.
   */
  DynamicAtomicState() : AtomicState() {}

  /**
   * @brief Add atoms to the atomic state.
   * @param atom_types Vector of atom types; one for each atom added.
   * @param pos Positional information for each atom.
   * @param vel Velocity information for each atom. Optional; if this isn't
   * provided, velocities will be initialised with zero.
   * @param force Force information for each atom. Optional; if this isn't
   * provided, forces will be initialised with zero.
   */
  void add(std::vector<std::shared_ptr<AtomType>>&& atom_types,
           Tensor<double, 2>&& pos,
           std::optional<Tensor<double, 2>>&& vel = std::nullopt,
           std::optional<Tensor<double, 2>>&& force = std::nullopt) {
    AtomicState::add(std::move(atom_types), std::move(pos));

    // Initialise velocities and forces as zero if we need this information for
    // the simulation and data not provided. Else just move the provided data.
    vel_ = std::move(vel.value_or(Tensor<double, 2>(num_atoms(), 3)));
    force_ = std::move(force.value_or(Tensor<double, 2>(num_atoms(), 3)));
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

 protected:
  Tensor<double, 2> vel_, force_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_ATOM_DYNAMIC_ATOMIC_STATE_HPP */
