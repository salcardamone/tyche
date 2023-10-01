/**
 * @brief
 */
#ifndef __TYCHE_CONSTANTS_HPP
#define __TYCHE_CONSTANTS_HPP

// C++ Standard Libraries
//
// Third-Party Libraries
//
// Project Inclusions
//

namespace tyche::constants {

static constexpr double avogadro = 6.02214076E23;
// Units: Joules / Kelvin
static constexpr double boltzmann = 1.380649E-23;

static constexpr double kg_to_dalton = 6.0221736E26;
static constexpr double m_to_angstrom = 1E10;
static constexpr double sec_to_fs = 1E15;
static constexpr double litre_to_cubic_angstrom =
    (m_to_angstrom * m_to_angstrom * m_to_angstrom) / 1E3;

static constexpr double kjmol_to_joule = 1000.0 / avogadro;
static constexpr double ev_to_joule = 1.602176565E-19;

// Units: Mass x Distance^2 / Time^2
static constexpr double joule_to_internal =
    kg_to_dalton * (m_to_angstrom * m_to_angstrom) / (sec_to_fs * sec_to_fs);
static constexpr double ev_to_internal = ev_to_joule * joule_to_internal;

}  // namespace tyche::constants

#endif /* #ifndef __TYCHE_CONSTANTS_HPP */
