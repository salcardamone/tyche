/**
 * @brief
 */
#ifndef __TYCHE_ATOMIC_STATE_READER_HPP
#define __TYCHE_ATOMIC_STATE_READER_HPP

// C++ Standard Libraries
#include <map>
#include <memory>
// Third-Party Libraries
#include <toml++/toml.h>
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/tensor.hpp"
#include "tyche/atom_type.hpp"
#include "tyche/atomic_state.hpp"

namespace tyche {

/**
 * @brief Read atomic state from the [Atoms] section of the TOML configuration.
 */
class AtomicStateReader {
 public:
  /**
   * @brief Class constructor.
   * @param config_table TOML table with an "Atoms" node.
   */
  AtomicStateReader(toml::table config_table)
      : toml_{*config_table["Atoms"].as_table()} {}

  /**
   * @brief Parse the AtomicState object from the configuration TOML.
   * @param atom_types Mapping from atom type name to parsed AtomType.
   * @return The AtomicState object parsed from the configuration TOML.
   */
  AtomicState parse(
      std::map<std::string, std::shared_ptr<AtomType>>& atom_types,
      bool needs_velocity = false, bool needs_acceleration = false) {

    AtomicState state(needs_velocity, needs_acceleration);

    auto num_atoms_type = parse_atom_types();

    std::size_t num_atoms = 0;
    for (const auto& k : atom_types) {
      spdlog::info("Found {} atom/s of type {}.", num_atoms_type.at(k.first),
                   k.first);
      num_atoms += num_atoms_type.at(k.first);
    }
    spdlog::info("Found {} atom/s in total.", num_atoms);

    auto pos = parse_tensor("positions", num_atoms);
    auto vel = parse_tensor("velocities", num_atoms);
    auto force = parse_tensor("forces", num_atoms);
    auto types = create_types(atom_types, num_atoms_type);

    state.add(std::move(types), std::move(*pos), std::move(vel),
              std::move(force));

    return state;
  }

 private:
  /**
   * @brief Attempt to parse tensor data across all atom types in the
   * configuration, dumping it all into a single tensor.
   * @param id
   * @param num_atoms
   * @return
   */
  std::optional<Tensor<double, 2>> parse_tensor(std::string id,
                                                std::size_t num_atoms) {
    Tensor<double, 2> tensor(num_atoms, 3);

    std::size_t num_atoms_processed = 0;
    for (const auto& atom_type : toml_) {
      if (!toml_[atom_type.first][id]) return std::nullopt;

      const toml::array* data = toml_[atom_type.first][id].as_array();

      num_atoms_processed +=
          parse_tensor_atom_type(data, tensor, num_atoms_processed);
    }
    return std::optional<Tensor<double, 2>>(std::move(tensor));
  }

  /**
   * @brief Parse atom type information from the configuration and dump it into
   * a tensor at some specified atom offset.
   * @param data
   * @param tensor
   * @param offset
   * @return
   */
  std::size_t parse_tensor_atom_type(const toml::array* data,
                                     Tensor<double, 2>& tensor,
                                     std::size_t offset) {
    for (std::size_t iatom = 0; iatom < data->size(); ++iatom) {
      auto cart_data = data->at(iatom).as_array();
      for (std::size_t idim = 0; idim < cart_data->size(); ++idim) {
        tensor(iatom + offset, idim) =
            cart_data->at(idim).as_floating_point()->get();
      }
    }
    return data->size();
  }

  /**
   * @brief Parse the atom types within the configuration and count the number
   * of atoms of each type.
   * @return A mapping from the atom type name to the number of atoms in the
   * configuration for that particular atom type.
   */
  std::map<std::string, std::size_t> parse_atom_types() {
    std::map<std::string, std::size_t> atom_types;
    for (auto key : toml_) {
      std::ostringstream ss;
      ss << key.first;
      std::size_t num_atoms = toml_[ss.str()]["positions"].as_array()->size();
      atom_types[ss.str()] = num_atoms;
    }
    return atom_types;
  }

  /**
   * @brief Create a vector of atom type pointers; one for each atom within the
   * configuration.
   * @param atom_types
   * @param num_atoms_type
   * @return
   */
  std::vector<std::shared_ptr<AtomType>> create_types(
      std::map<std::string, std::shared_ptr<AtomType>>& atom_types,
      std::map<std::string, std::size_t>& num_atoms_type) {
    std::vector<std::shared_ptr<AtomType>> types;
    for (const auto& type : num_atoms_type) {
      for (std::size_t iatom = 0; iatom < num_atoms_type.at(type.first);
           ++iatom) {
        types.push_back(atom_types.at(type.first));
      }
    }
    return types;
  }

  toml::table toml_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_ATOMIC_STATE_READER_HPP */
