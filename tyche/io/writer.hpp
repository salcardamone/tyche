/**
 * @brief
 */
#ifndef __TYCHE_IO_WRITER_HPP
#define __TYCHE_IO_WRITER_HPP

// C++ Standard Libraries
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <filesystem>
// Third-Party Libraries
//
// Project Inclusions
//

namespace tyche {

/**
 * @brief Abstract base writer class from which other writers derive.
 */
class Writer {
 public:
  /**
   * @brief Class constructor.
   * @param filename Writer output file.
   */
  Writer(std::filesystem::path filename) : ofs_{filename} {
    ofs_.setf(std::ios::fixed, std::ios::floatfield);
  }

  /**
   * @brief Virtual destructor.
   */
  virtual ~Writer() {
    ofs_.flush();
    ofs_.close();
  }

  /**
   * @brief Write something.
   * @param comment Optional comment to include in the writing.
   */
  virtual void write(std::optional<std::string> comment = std::nullopt) = 0;

 protected:
  std::ofstream ofs_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_IO_WRITER_HPP */
