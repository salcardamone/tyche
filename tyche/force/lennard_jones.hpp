/**
 * @brief
 */
#ifndef __TYCHE_FORCE_LENNARD_JONES_HPP
#define __TYCHE_FORCE_LENNARD_JONES_HPP

// C++ Standard Libraries
#include <cmath>
// Third-Party Libraries
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/force/force.hpp"

namespace tyche {

/**
 * @brief Lennard-Jones force evaluation for an atomic state.
 */
class LennardJones : public Force {
 public:
  /**
   * @brief Class constructor. Initialise all mixed atom type parameters here so
   * we don't have to do it for every pair during evaluation.
   * @param atom_types Mapping from atom type to an index on [0,num_atom_types).
   */
  LennardJones(
      const std::map<std::shared_ptr<AtomType>, std::size_t>& atom_types);

  /**
   * @brief Evaluate all pairwise forces arising from the Lennard-Jones
   * potential between atoms in an atomic state.
   * @param state The atomic state we're computing the forces for.
   * @return The Lennard-Jones potential.
   */
  double evaluate(DynamicAtomicState& state, const Cell& cell) override;

 protected:
  /**
   * @brief Implement the Lorentz rule for mixing of Lennard-Jones sigmas.
   * @param iatom_sigma Sigma for an atom.
   * @param jatom_sigma Sigma for an atom.
   * @return The mixed sigma.
   */
  inline double mix_sigma(double iatom_sigma, double jatom_sigma) {
    return (iatom_sigma + jatom_sigma) / 2;
  }

  /**
   * @brief Implement the Berthelot rule for mixing of Lennard-Jones epsilons.
   * @param iatom_eps Epsilon for an atom.
   * @param jatom_eps Epsilon for an atom.
   * @return The mixed epsilon.
   */
  inline double mix_eps(double iatom_eps, double jatom_eps) {
    return std::sqrt(iatom_eps * jatom_eps);
  }

  std::vector<double> eps_, sigma_;
  std::map<std::shared_ptr<AtomType>, std::size_t> atom_type_idx_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_FORCE_LENNARD_JONES_HPP */
