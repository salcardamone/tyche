/**
 * @brief
 */
#ifndef __TYCHE_IO_READER_HPP
#define __TYCHE_IO_READER_HPP

// C++ Standard Libraries
#include <map>
#include <any>
#include <cstring>
// Third-Party Libraries
//
// Project Inclusions
//

namespace tyche {

/**
 * @brief Abstract base reader class from which all other readers should derive.
 */
class Reader {
 public:
  // Generic mapping type that we'll return from our readers to be used by
  // various builders and factories. Provides a standard interface between
  // reader and constructors.
  using Mapping = std::map<std::string, std::any>;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_IO_READER_HPP */
