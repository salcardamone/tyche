/**
 * @brief
 */
#ifndef __TYCHE_ATOMIC_STATE_HPP
#define __TYCHE_ATOMIC_STATE_HPP

// C++ Standard Libraries
#include <map>
#include <memory>
#include <vector>
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
   */
  AtomicState() {}

  /**
   * @brief Virtual destructor.
   */
  virtual ~AtomicState() = default;
  
  /**
   * @brief Add atoms to the atomic state.
   * @param atom_types Vector of atom types; one for each atom added.
   * @param pos Positional information for each atom.
   */
  void add(std::vector<std::shared_ptr<AtomType>>&& atom_types,
           Tensor<double, 2>&& pos) {
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

 protected:
  std::map<std::shared_ptr<AtomType>, std::size_t> num_atoms_, atom_type_idx_;
  Tensor<double, 2> pos_;
  std::vector<std::shared_ptr<AtomType>> atom_types_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_ATOMIC_STATE_HPP */
