/**
 * @brief
 */
// C++ Standard Libraries
#include <map>
#include <memory>
#include <cstdint>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/util/tensor.hpp"
#include "tyche/atom/atom_type.hpp"
#include "tyche/system/cell.hpp"
#include "tyche/atom/dynamic_atomic_state.hpp"
#include "tyche/force/lennard_jones.hpp"

namespace tyche {

// ========================================================================== //

LennardJones::LennardJones(
    const std::map<std::shared_ptr<AtomType>, std::size_t>& atom_types)
    : eps_(atom_types.size() * atom_types.size()),
      sigma_(atom_types.size() * atom_types.size()),
      atom_type_idx_{atom_types} {
  for (const auto& itype : atom_types) {
    for (const auto& jtype : atom_types) {
      std::size_t idx = itype.second * atom_types.size() + jtype.second;
      std::size_t jdx = jtype.second * atom_types.size() + itype.second;

      double eps_itype = itype.first->get<double>("eps_lj").value();
      double eps_jtype = jtype.first->get<double>("eps_lj").value();

      double sigma_itype = itype.first->get<double>("sigma_lj").value();
      double sigma_jtype = jtype.first->get<double>("sigma_lj").value();

      eps_[idx] = eps_[jdx] = mix_eps(eps_itype, eps_jtype);
      sigma_[idx] = sigma_[jdx] = mix_sigma(sigma_itype, sigma_jtype);
    }
  }
}

// ========================================================================== //

double LennardJones::evaluate(DynamicAtomicState& state, const Cell& cell) {
  double pot = 0;

  Tensor<double, 2>::const_iterator iatom_pos = state.pos();
  Tensor<double, 2>::iterator iatom_force = state.force();

  for (std::size_t iatom = 0; iatom < state.num_atoms(); ++iatom) {
    auto iatom_type = state.atom_type(iatom);

    Tensor<double, 2>::const_iterator jatom_pos = state.pos(iatom + 1);
    Tensor<double, 2>::iterator jatom_force = state.force(iatom + 1);

    for (std::size_t jatom = iatom + 1; jatom < state.num_atoms(); ++jatom) {
      auto jatom_type = state.atom_type(jatom);

      double dx = (*jatom_pos++ - iatom_pos[0]);
      double dy = (*jatom_pos++ - iatom_pos[1]);
      double dz = (*jatom_pos++ - iatom_pos[2]);

      cell.min_image(dx, dy, dz);

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

      iatom_force[0] -= fx;
      *jatom_force++ += fx;
      iatom_force[1] -= fy;
      *jatom_force++ += fy;
      iatom_force[2] -= fz;
      *jatom_force++ += fz;
    }

    iatom_pos += 3;
    iatom_force += 3;
  }
  return pot;
}

// ========================================================================== //

}  // namespace tyche
