/**
 * @brief
 */
#ifndef __TYCHE_ATOMIC_STATE_READER_HPP
#define __TYCHE_ATOMIC_STATE_READER_HPP

// C++ Standard Libraries
#include <map>
#include <memory>
#include <ranges>
// Third-Party Libraries
#include <toml++/toml.h>
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/util/tensor.hpp"
#include "tyche/io/toml_reader.hpp"
#include "tyche/atom/atom_type.hpp"
#include "tyche/atom/atomic_state.hpp"
#include "tyche/atom/dynamic_atomic_state.hpp"

namespace tyche {

/**
 * @brief Methods and state common to all atomic state readers.
 */
template <class ReturnType>
class BaseAtomicStateReader : public TOMLReader<ReturnType> {
 public:
  /**
   * @brief Class constructor.
   * @param atom_types Mapping from atom type name to its instance.
   */
  BaseAtomicStateReader(
      std::map<std::string, std::shared_ptr<AtomType>>& atom_types)
      : atom_types_{atom_types} {}

 protected:
  using NumAtomsMapping = std::map<std::string, std::size_t>;

  std::map<std::string, std::shared_ptr<AtomType>> atom_types_;

  /**
   * @brief Parse tensors from each atom type and concatenate together
   * vertically.
   *
   * If the tensor doesn't exist (i.e. the key isn't specified for
   * that atom type), then will create a blank matrix in that space.
   * @param config The "Atoms" node in the TOML configuration.
   * @param field The field containing the tensor.
   * @param num_atoms_per_type Mapping from atom type name to number of atoms of
   * that type.
   * @return A (num_atoms x 3) tensor with data retrieved from TOML for each
   * atom type.
   */
  Tensor<double, 2> parse_atomic_cartesian_tensor(
      toml::table& config, std::string field,
      NumAtomsMapping& num_atoms_per_type) {
    Tensor<double, 2> tensor(0, 3);
    for (auto atom_type : std::views::keys(atom_types_)) {
      Tensor<double, 2> atom_type_tensor;
      if (config[atom_type][field]) {
        atom_type_tensor = this->template parse_matrix<double>(
            *config[atom_type][field].as_array());
      } else {
        atom_type_tensor = Tensor<double, 2>(num_atoms_per_type[atom_type], 3);
      }
      tensor.concatenate<1>(atom_type_tensor);
    }
    return tensor;
  }

  /**
   * @brief Parse the number of atoms associated with each atom type from the
   * TOML.
   * @param config The "Atoms" node in the TOML configuration.
   * @return A mapping from atom type name to number of atoms of that type.
   */
  NumAtomsMapping parse_num_atoms_per_type(toml::table& config) {
    NumAtomsMapping num_atoms_per_type;
    auto keys = std::views::keys(atom_types_);
    for (const auto& key : keys) {
      num_atoms_per_type[key] = config[key]["positions"].as_array()->size();
    }
    return num_atoms_per_type;
  }

  /**
   * @brief Retrieve the total number of atoms in the configuration.
   * @param num_atoms_per_type Mapping from atom type name to number of atoms of
   * that type.
   * @return The total number of atoms in the system.
   */
  std::size_t total_num_atoms(NumAtomsMapping& num_atoms_per_type) {
    std::size_t num_atoms = 0;
    for (const auto& k : atom_types_) {
      spdlog::info("Found {} atom/s of type {}.",
                   num_atoms_per_type.at(k.first), k.first);
      num_atoms += num_atoms_per_type.at(k.first);
    }
    spdlog::info("Found {} atom/s in total.", num_atoms);
    return num_atoms;
  }

  /**
   * @brief Create an iterable of atom types for each atom in the system.
   * @param num_atoms_per_type Mapping from atom type name to number of atoms of
   * that type.
   * @return Iterable with an atom type for each atom.
   */
  std::vector<std::shared_ptr<AtomType>> create_types(
      NumAtomsMapping& num_atoms_per_type) {
    std::vector<std::shared_ptr<AtomType>> types;
    for (const auto& type : num_atoms_per_type) {
      for (std::size_t iatom = 0; iatom < num_atoms_per_type.at(type.first);
           ++iatom) {
        types.push_back(atom_types_.at(type.first));
      }
    }
    return types;
  }
};

/**
 * @brief Reader for static atomic state.
 */
class AtomicStateReader : public BaseAtomicStateReader<AtomicState> {
 public:
  /**
   * @brief Class constructor.
   * @param atom_types Mapping from atom type name to its instance.
   */
  AtomicStateReader(
      std::map<std::string, std::shared_ptr<AtomType>>& atom_types)
      : BaseAtomicStateReader(atom_types) {}

  /**
   * @brief Parse the static atomic state from the TOML config.
   * @param config "Atoms" node of the TOML.
   * @return The instantiated AtomicState.
   */
  AtomicState parse(toml::table& config) {
    AtomicState atomic_state;

    auto num_atoms_per_type = parse_num_atoms_per_type(config);
    // Parse and create all the data we need to create the AtomicState.
    auto types = create_types(num_atoms_per_type);
    Tensor<double, 2> pos =
        parse_atomic_cartesian_tensor(config, "positions", num_atoms_per_type);

    atomic_state.add(std::move(types), std::move(pos));

    return atomic_state;
  }
};

/**
 * @brief Reader for dynamic atomic state.
 */
class DynamicAtomicStateReader
    : public BaseAtomicStateReader<DynamicAtomicState> {
 public:
  /**
   * @brief Class constructor.
   * @param atom_types Mapping from atom type name to its instance.
   */
  DynamicAtomicStateReader(
      std::map<std::string, std::shared_ptr<AtomType>>& atom_types)
      : BaseAtomicStateReader(atom_types) {}

  /**
   * @brief Parse the dynamic atomic state from the TOML config.
   * @param config "Atoms" node of the TOML.
   * @return The instantiated DynamicAtomicState.
   */
  DynamicAtomicState parse(toml::table& config) {
    DynamicAtomicState atomic_state;

    auto num_atoms_per_type = parse_num_atoms_per_type(config);
    // Parse and create all the data we need to create the AtomicState.
    auto types = create_types(num_atoms_per_type);
    Tensor<double, 2> pos =
        parse_atomic_cartesian_tensor(config, "positions", num_atoms_per_type);
    Tensor<double, 2> vel =
        parse_atomic_cartesian_tensor(config, "velocities", num_atoms_per_type);
    Tensor<double, 2> force =
        parse_atomic_cartesian_tensor(config, "forces", num_atoms_per_type);

    atomic_state.add(std::move(types), std::move(pos), std::move(vel),
                     std::move(force));

    return atomic_state;
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_ATOMIC_STATE_READER_HPP */
