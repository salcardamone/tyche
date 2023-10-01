/**
 * @brief
 */
#ifndef __TYCHE_ATOMIC_STATE_WRITER_HPP
#define __TYCHE_ATOMIC_STATE_WRITER_HPP

// C++ Standard Libraries
#include <fstream>
#include <filesystem>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/util/tensor.hpp"
#include "tyche/atom/atomic_state.hpp"

namespace tyche {

/**
 * @brief Abstract atomic state writer class to be overridden.
 */
class AtomicStateWriter {
 public:
  /**
   * @brief Add the atomic state to whatever is to be written.
   * @param state The atomic state to write.
   */
  virtual void add(AtomicState& state) = 0;
};

/**
 * @brief Atomic state writer class for .xyz files.
 */
class AtomicStateWriterXYZ : public AtomicStateWriter {
 public:
  /**
   * @brief Class constructor.
   * @param filename The path to the file we're writing to.
   */
  AtomicStateWriterXYZ(std::filesystem::path filename) : ofs_{filename} {
    ofs_.setf(std::ios::fixed, std::ios::floatfield);
  }

  /**
   * @brief Class destructor.
   */
  ~AtomicStateWriterXYZ() { ofs_.close(); }

  /**
   * @brief Dump the atomic state to the .xyz file.
   * @param state The atomic state to write.
   */
  void add(AtomicState& state) override {
    Tensor<double, 2>::const_iterator pos = state.pos(0);
    for (std::size_t iatom = 0; iatom < state.num_atoms(); ++iatom) {
      ofs_ << state.atom_type(iatom)->id() << '\t' << *pos++ << '\t' << *pos++
           << '\t' << *pos++ << '\n';
    }
  }

  /**
   * @brief Dump the atomic state as well as metadata to the .xyz.
   * @param state The atomic state to write.
   * @param comment The comment for the snapshot.
   */
  void add(AtomicState& state, std::string comment) {
    ofs_ << state.num_atoms() << '\n';
    ofs_ << comment << '\n';
    add(state);
  }

 private:
  std::ofstream ofs_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_ATOMIC_STATE_WRITER_HPP */
