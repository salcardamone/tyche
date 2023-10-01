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
#include "tyche/util/constants.hpp"

namespace tyche {

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
