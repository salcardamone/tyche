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
      std::map<std::string, std::shared_ptr<AtomType>>& atom_types) {
    AtomicState state;

    // Loop over all atom types within the [Atoms] section of the TOML and
    // add the atoms therein to the AtomicState
    for (auto key : toml_) {
      std::ostringstream ss;
      ss << key.first;
      auto atom_type = atom_types.at(ss.str());
      auto pos = parse_tensor(ss.str(), "positions");
      auto vel = parse_tensor(ss.str(), "velocities");
      auto acc = parse_tensor(ss.str(), "accelerations");
      state.add(atom_type, std::move(*pos), vel, acc);
    }
    return state;
  }

 private:
  /**
   * @brief Parse the tensor data of all atoms belonging to a particular
   * AtomType into a Tensor.
   * @param atom_type Identifier string for the atom type we're parsing.
   * @param id The tensor type; can be "positions", "velocities", or
   * "accelerations".
   * @return 2D tensor containing atomic tensor data; rows enumerate spatial
   * dimension, column the atom index. Optional; will return nothing if the
   * tensor type isn't available in the TOML.
   */
  std::optional<Tensor<double, 2>> parse_tensor(std::string atom_type,
                                                std::string id) {
    if (!toml_[atom_type][id]) return std::nullopt;

    const toml::array* data = toml_[atom_type][id].as_array();
    Tensor<double, 2> tensor(3, data->size());

    for (std::size_t iatom = 0; iatom < data->size(); ++iatom) {
      auto cart_data = data->at(iatom).as_array();
      for (std::size_t idim = 0; idim < cart_data->size(); ++idim) {
        tensor(idim, iatom) = cart_data->at(idim).as_floating_point()->get();
      }
    }

    return tensor;
  }

  toml::table toml_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_ATOMIC_STATE_READER_HPP */
