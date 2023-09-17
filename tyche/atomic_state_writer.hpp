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
#include "tyche/tensor.hpp"
#include "tyche/atomic_state.hpp"

namespace tyche {

/**
 * @brief
 */
class AtomicStateWriter {
 public:
  /**
   * @brief
   */
  AtomicStateWriter(std::filesystem::path filename) : ofs_{filename} {
    ofs_.setf(std::ios::fixed, std::ios::floatfield);
  }

  /**
   * @brief
   */
  ~AtomicStateWriter() { ofs_.close(); }

  /**
   * @brief
   */
  void add(AtomicState& state, std::string comment) {
    ofs_ << state.num_atoms() << '\n';
    ofs_ << comment << '\n';
    Tensor<double, 2>::const_iterator pos = state.pos(0);
    for (std::size_t iatom = 0; iatom < state.num_atoms(); ++iatom) {
      ofs_ << state.atom_type(iatom)->id() << '\t' << *pos++ << '\t' << *pos++
           << '\t' << *pos++ << '\n';
    }
  }

 private:
  std::ofstream ofs_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_ATOMIC_STATE_WRITER_HPP */
