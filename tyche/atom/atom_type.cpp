/**
 * @brief Definition of various AtomType methods.
 */
// Standard Libraries
//
// Third-party Libraries
//
// Project Inclusions
#include "atom_type.hpp"
#include "atom_type_builder.hpp"

namespace tyche {

// Note that this needs to be defined here so that we have information
// about the AtomTypeBuilder constructor.
AtomTypeBuilder AtomType::create(std::string id) { return AtomTypeBuilder(id); }

}  // namespace tyche
