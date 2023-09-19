/**
 * @brief Some default AtomType configurations we can fall back to.
 */
#ifndef __TYCHE_DEFAULT_ATOM_TYPES_HPP
#define __TYCHE_DEFAULT_ATOM_TYPES_HPP

// Standard Libraries
#include <map>
#include <string>
// Third-party Libraries
#include <toml++/toml.h>
// Project Inclusions
//

namespace tyche {

namespace constants {
static constexpr double avogadro = 6.02214076E23;

static constexpr double kg_to_dalton = 6.0221736E26;
static constexpr double m_to_angstrom = 1E10;
static constexpr double sec_to_fs = 1E15;

static constexpr double kjmol_to_joule = 1000.0 / avogadro;
static constexpr double ev_to_joule = 1.602176565E-19;

// Energy is Mass Distance^2 / Time^2
static constexpr double joule_to_internal =
    kg_to_dalton * (m_to_angstrom * m_to_angstrom) / (sec_to_fs * sec_to_fs);
static constexpr double ev_to_internal = ev_to_joule * joule_to_internal;
}  // namespace constants

// This maps from some typical atom type identifiers (the chemical element
// symbol) to some basic parameters that we don't necessarily expect the user to
// set.
// Lennard-Jones parameters are taken from
// https://openkim.org/id/MO_959249795837_003
static const std::map<std::string, toml::table> atom_type_defaults{
    {"H", toml::table{{"mass", 1.0},
                      {"num_electrons", 1},
                      {"nuclear_charge", 1},
                      {"sigma_lj", 0.5523570},
                      {"eps_lj", 4.4778900 * constants::ev_to_internal}}},
    {"O", toml::table{{"mass", 16.0},
                      {"num_electrons", 8},
                      {"nuclear_charge", 8},
                      {"sigma_lj", 1.1759900},
                      {"eps_lj", 5.1264700 * constants::ev_to_internal}}},
    {"Ar", toml::table{{"mass", 39.948},
                       {"num_electrons", 18},
                       {"nuclear_charge", 18},
                       {"sigma_lj", 3.405},
                       {"eps_lj", 0.0123529 * constants::ev_to_internal}}}};
}  // namespace tyche

#endif /* #ifndef __TYCHE_DEFAULT_ATOM_TYPES_HPP */
