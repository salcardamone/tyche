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
static const std::map<std::string, toml::table> atom_type_defaults{
    {"H", toml::table{{"mass", 1.0},
                      {"num_electrons", 1},
                      {"nuclear_charge", 1}}},
    {"O", toml::table{{"mass", 16.0},
                      {"num_electrons", 8},
                      {"nuclear_charge", 8}}},
    {"Ar", toml::table{{"mass", 39.948},
                       {"num_electrons", 18},
                       {"nuclear_charge", 18}}}};
}  // namespace tyche

#endif /* #ifndef __TYCHE_DEFAULT_ATOM_TYPES_HPP */
