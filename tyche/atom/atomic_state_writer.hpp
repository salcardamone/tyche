/**
 * @brief
 */
#ifndef __TYCHE_ATOMIC_STATE_WRITER_HPP
#define __TYCHE_ATOMIC_STATE_WRITER_HPP

// C++ Standard Libraries
#include <memory>
#include <cstring>
#include <optional>
#include <filesystem>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/io/writer.hpp"
#include "tyche/util/tensor.hpp"
#include "tyche/atom/atomic_state.hpp"

namespace tyche {

/**
 * @brief Abstract atomic state writer class to be overridden.
 */
template <class AtomicStateType>
class AtomicStateWriter : public Writer {
 public:
  /**
   * @brief Class constructor.
   * @param filename Writer output file.
   * @param atomic_state The state we'll be writing.
   */
  AtomicStateWriter(std::filesystem::path filename,
                    std::shared_ptr<AtomicStateType> atomic_state)
      : Writer(filename), atomic_state_(atomic_state) {}

 protected:
  std::shared_ptr<AtomicStateType> atomic_state_;
};

/**
 * @brief Atomic state writer class for .xyz files.
 */
class AtomicStateWriterXYZ : public AtomicStateWriter<AtomicState> {
 public:
  /**
   * @brief Class constructor.
   * @param filename The path to the .xyz file we're writing to.
   * @param atomic_state The state we'll be writing.
   */
  AtomicStateWriterXYZ(std::filesystem::path filename,
                       std::shared_ptr<AtomicState> atomic_state)
      : AtomicStateWriter(filename, atomic_state) {}

  /**
   * @brief Append the atomic state to the .xyz file.
   * @param comment The comment that comes after the number of atoms in the .xyz
   * file.
   */
  void write(std::optional<std::string> comment = std::nullopt) {
    ofs_ << atomic_state_->num_atoms() << '\n';
    if (comment != std::nullopt) {
      ofs_ << *comment << '\n';
    }
    Tensor<double, 2>::const_iterator pos = atomic_state_->pos(0);
    for (std::size_t iatom = 0; iatom < atomic_state_->num_atoms(); ++iatom) {
      ofs_ << atomic_state_->atom_type(iatom)->id() << '\t' << *pos++ << '\t'
           << *pos++ << '\t' << *pos++ << '\n';
    }
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_ATOMIC_STATE_WRITER_HPP */
