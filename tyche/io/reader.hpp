/**
 * @brief
 */
#ifndef __TYCHE_IO_READER_HPP
#define __TYCHE_IO_READER_HPP

// C++ Standard Libraries
#include <map>
#include <any>
#include <string>
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

  /**
   * @brief Take a map and for each key which contains a substring, strip that
   * substring from the key and add it as a key in a new map, keeping the same
   * value.
   * @param map The map we're looking for keys in.
   * @param substring The substring we're to strip from keys.
   * @return The new map with stripped keys.
   */
  static Mapping remove_prefix(const Mapping& map, std::string substring) {
    Mapping result;
    for (const auto& entry : map) {
      std::string key = entry.first;
      if (key.find(substring) != std::string::npos) {
        key.erase(0, substring.size());
        result[key] = entry.second;
      }
    }
    return result;
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_IO_READER_HPP */
