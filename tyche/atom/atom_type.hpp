/**
 * @brief Atom type definitions.
 */
#ifndef __TYCHE_ATOM_TYPE_HPP
#define __TYCHE_ATOM_TYPE_HPP

// C++ Standard Libraries
#include <any>
#include <map>
#include <cstring>
#include <cstdint>
#include <sstream>
#include <optional>
#include <stdexcept>
// Third-party Libraries
//
// Project Inclusions
#include "tyche/util/must.hpp"

namespace tyche {

// Forward-declaration of the builder for AtomType objects
class AtomTypeBuilder;

/**
 * @brief Wrapper around an atom type's parameters.
 *
 * An AtomType is composed of some "fundamental" parameters that will be
 * required by most simulation methods, and can have defaults assigned because
 * they're unlikely to vary too frequently. These fundamental parameters have
 * individual member variables in the class and getters.
 *
 * The AtomType also has a map containing simulation-specific parameters, or
 * parameters it's silly to try to provide defaults for. These parameters can be
 * retrieved from the `get` getter which optionally returns the parameter if
 * it has been provided during configuration. These parameters shouldn't be
 * retrieved within computational hotspots since we have to search a map each
 * time.
 */
class AtomType {
 public:
  friend AtomTypeBuilder;

  /**
   * @brief Class constructor.
   * @param id Identifier for the atom type.
   */
  AtomType(std::string id) : id_{id} {}

  /**
   * @brief Getter for the atom type identifier.
   * @return Atom type identifier.
   */
  const std::string id() const { return id_; }

  /**
   * @brief Getter for the atom type mass.
   * @return Atom type mass.
   */
  const double mass() const { return mass_; }

  /**
   * @brief Getter for the nuclear charge of the atom type.
   * @return Atom type nuclear charge.
   */
  const uint32_t nuclear_charge() const { return nuclear_charge_; }

  /**
   * @brief Getter for the number of electrons in of the atom type.
   * @return Atom type number of electrons.
   */
  const uint32_t num_electrons() const { return num_electrons_; }

  /**
   * @brief Getter for non-fundamental parameters that may have been read from
   * the configuration.
   * @tparam DataType Type of value to try and retrieve from the non-fundamental
   * parameters.
   * @param name The name of the parameter to try and retrieve from the
   * non-fundamental parameters.
   * @return The value associated with the requested parameter.
   */
  template <typename DataType>
  DataType get(std::string name) {
    DataType val;
    try {
      val = must_find<DataType>(others_, name);
    } catch (std::runtime_error& err) {
      throw std::runtime_error("Atom Type " + id() +
                               " parameter lookup failed. " + err.what());
    }
    return val;
  }

  /**
   * @brief Create a new instance of the AtomTypeBuilder.
   * @param id The atom type identifier; typically the element name.
   * @return A new AtomType builder.
   */
  static AtomTypeBuilder create(std::string id);

  /**
   * @brief Overload of the streaming operator; dump some information about the
   * instance.
   * @param os The output stream we're dumping information to.
   * @param atom_type The AtomType object we're dumping information about.
   * @return The modified output stream operator.
   */
  friend std::ostream& operator<<(std::ostream& os, const AtomType& atom_type) {
    os << "Atom Type Identifier: " << atom_type.id()
       << ", Mass: " << atom_type.mass()
       << ", Num. Electrons: " << atom_type.num_electrons()
       << ", Nuclear Charge: " << atom_type.nuclear_charge();
    return os;
  }

 private:
  std::string id_;
  double mass_;
  uint32_t nuclear_charge_, num_electrons_;
  std::map<std::string, std::any> others_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_ATOM_TYPE_HPP */
