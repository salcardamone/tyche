/**
 * @brief Builder for the AtomType class.
 */
#ifndef __TYCHE_ATOM_TYPE_BUILDER_HPP
#define __TYCHE_ATOM_TYPE_BUILDER_HPP

// Standard Libraries
#include <optional>
// Third-party Libraries
#include <spdlog/spdlog.h>
// Project Inclusions
#include "atom_type.hpp"
#include "atom_type_defaults.hpp"

namespace tyche {

/**
 * @brief Builder for the AtomType class.
 */
class AtomTypeBuilder {
 public:
  /**
   * @brief Class constructor.
   * @param atom_type_id The string identifier for the atom type; typically
   * element symbol.
   */
  AtomTypeBuilder(std::string atom_type_id) : atom_type_{atom_type_id} {}

  /**
   * @brief Set the mass in the built AtomType.
   * @param mass The mass to set.
   * @return The AtomTypeBuilder object.
   */
  AtomTypeBuilder& mass(std::optional<double>&& mass) {
    set_param<double>("mass", atom_type_.mass_, mass);
    return *this;
  }

  /**
   * @brief Set the number of electrons in the built AtomType.
   * @param num_electrons The number of electrons to set.
   * @return The AtomTypeBuilder object.
   */
  AtomTypeBuilder& num_electrons(std::optional<uint32_t>&& num_electrons) {
    set_param<uint32_t>("num_electrons", atom_type_.num_electrons_,
                        num_electrons);
    return *this;
  }

  /**
   * @brief Set the nuclear charge in the built AtomType.
   * @param nuclear_charge The nuclear_charge to set.
   * @return The AtomTypeBuilder object.
   */
  AtomTypeBuilder& nuclear_charge(std::optional<uint32_t>&& nuclear_charge) {
    set_param<uint32_t>("nuclear_charge", atom_type_.nuclear_charge_,
                        nuclear_charge);
    return *this;
  }

  /**
   *
   */
  AtomTypeBuilder& others(std::map<std::string, std::any>& others) {
    atom_type_.others_.insert(others.begin(), others.end());
    return *this;
  }

  /**
   * @brief Return the AtomType object that we've constructed.
   * @return The built AtomType object.
   */
  AtomType build() { return std::move(atom_type_); }

 private:
  AtomType atom_type_;

  /**
   * @brief Set a member variable of the AtomType.
   *
   * If the provided optional value is std::nullopt, then attempt to retrieve a
   * value from the default atom types built into the program.
   * @tparam ValType The type of the parameter we're setting.
   * @param param_name The name of the parameter we're setting.
   * @param setter Reference to the AtomType's member variable we're setting.
   * @param val The optional value we're trying to use to set the parameter.
   */
  template <typename ValType>
  void set_param(std::string param_name, ValType& setter,
                 std::optional<ValType>& val) {
    setter = *val;

    if (val == std::nullopt) {
      try {
        setter =
            *atom_type_defaults.at(atom_type_.id_)[param_name].value<ValType>();
      } catch (const std::out_of_range& e) {
        spdlog::error(
            "No default {} paramter for atom type: {}. You must provide a "
            "value.",
            param_name, atom_type_.id_);
        std::exit(EXIT_FAILURE);
      }
    }
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_ATOM_TYPE_BUILDER_HPP */
