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
  LennardJones(std::map<std::shared_ptr<AtomType>, std::size_t>& atom_types)
      : eps_(atom_types.size() * atom_types.size()),
        sigma_(atom_types.size() * atom_types.size()),
        atom_type_idx_{atom_types} {
    for (const auto& itype : atom_types) {
      for (const auto& jtype : atom_types) {
        std::size_t idx = itype.second * atom_types.size() + jtype.second;
        std::size_t jdx = jtype.second * atom_types.size() + itype.second;
        eps_[idx] = eps_[jdx] =
            mix_eps(itype.first->eps_lj(), jtype.first->eps_lj());
        sigma_[idx] = sigma_[jdx] =
            mix_sigma(itype.first->sigma_lj(), jtype.first->sigma_lj());
      }
    }
  }

  /**
   * @brief Evaluate all pairwise forces arising from the Lennard-Jones
   * potential between atoms in an atomic state.
   * @param state The atomic state we're computing the forces for.
   * @return The Lennard-Jones potential.
   */
  double evaluate(AtomicState& state) override {
    double pot = 0;

    Tensor<double, 2>::iterator forces = state.force();
    Tensor<double, 2>::const_iterator posns = state.pos();

    Tensor<double, 2>::const_iterator iatom_pos = state.pos();
    Tensor<double, 2>::iterator iatom_force = state.force();

    for (std::size_t iatom = 0; iatom < state.num_atoms(); ++iatom) {
      auto iatom_type = state.atom_type(iatom);

      Tensor<double, 2>::const_iterator jatom_pos = state.pos(iatom + 1);
      Tensor<double, 2>::iterator jatom_force = state.force(iatom + 1);

      for (std::size_t jatom = iatom + 1; jatom < state.num_atoms(); ++jatom) {
        auto jatom_type = state.atom_type(jatom);

        double dx = (iatom_pos[0] - *jatom_pos++);
        double dy = (iatom_pos[1] - *jatom_pos++);
        double dz = (iatom_pos[2] - *jatom_pos++);

        double dx_sq = dx * dx, dy_sq = dy * dy, dz_sq = dz * dz;
        double rsq = dx_sq + dy_sq + dz_sq;

        std::size_t type_idx =
            atom_type_idx_[iatom_type] * atom_type_idx_.size() +
            atom_type_idx_[jatom_type];

        double sigma = sigma_[type_idx], eps = eps_[type_idx];

        double tmp = sigma * sigma / rsq;
        double B = tmp * tmp * tmp;
        double A = B * B;

        pot += 4 * eps * (A - B);

        double f_tmp = (24 * eps / rsq) * (2 * A - B);
        double fx = f_tmp * dx, fy = f_tmp * dy, fz = f_tmp * dz;

        iatom_force[0] += fx;
        *jatom_force++ -= fx;
        iatom_force[1] += fy;
        *jatom_force++ -= fy;
        iatom_force[2] += fz;
        *jatom_force++ -= fz;
      }

      iatom_pos += 3;
      iatom_force += 3;
    }
    return pot;
  }

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
