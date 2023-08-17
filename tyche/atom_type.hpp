/**
 * @brief Atom type definitions.
 */
#ifndef __TYCHE_ATOM_TYPE_HPP
#define __TYCHE_ATOM_TYPE_HPP

// C++ Standard Libraries
#include <iostream>
#include <sstream>
#include <string>
// Third-party Libraries
#include <spdlog/spdlog.h>
// Project Inclusions
//

namespace tyche {

// Forward-declaration of the builder for AtomType objects
class AtomTypeBuilder;

/**
 * @brief Wrapper around an atom type's various parameters.
 */
class AtomType {
 public:
  AtomType(std::string id) : id_{id} {};

  AtomType(AtomType&) = delete;
  AtomType(const AtomType&) = default;
  AtomType& operator=(const AtomType&) = default;

  AtomType(AtomType&&) = default;
  AtomType& operator=(AtomType&&) = default;

  const double mass() const { return mass_; }
  const uint32_t nuclear_charge() const { return nuclear_charge_; }
  const uint32_t num_electrons() const { return num_electrons_; }

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
    os << "Atom Type Identifier: " << atom_type.id_
       << ", Mass: " << atom_type.mass_
       << ", Num. Electrons: " << atom_type.num_electrons_
       << ", Nuclear Charge: " << atom_type.nuclear_charge_;
    return os;
  }

  friend AtomTypeBuilder;

 private:
  std::string id_;
  double mass_;
  uint32_t num_electrons_, nuclear_charge_;
};

}  // namespace tyche

/**
 * @brief Formatter for the AtomType object that allows us to print information
 * with spdlog.
 */
template <>
struct fmt::formatter<tyche::AtomType> : fmt::formatter<std::string> {
  auto format(tyche::AtomType& atom_type, format_context& ctx)
      -> decltype(ctx.out()) {
    std::ostringstream ss;
    ss << atom_type;
    return format_to(ctx.out(), "{}", ss.str());
  }
};

#endif /* #ifndef __TYCHE_ATOM_TYPE_HPP */
