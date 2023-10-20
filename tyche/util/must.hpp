/**
 * @brief
 */
#ifndef __TYCHE_UTIL_MUST_HPP
#define __TYCHE_UTIL_MUST_HPP

// C++ Standard Libraries
#include <any>
#include <map>
#include <string>
#include <stdexcept>
// Third-Party Libraries
//
// Project Inclusions
//

namespace tyche {

/**
 * @brief Return a parameter from a map and throw an error if it wasn't found.
 * @tparam ReturnType The type of the returned parameter.
 * @param map The map containing the parameter.
 * @param key The name of the parameter we're looking for.
 * @return The requested parameter.
 */
template <typename ReturnType>
static ReturnType must_find(std::map<std::string, std::any>& map,
                            std::string key) {
  auto it = map.find(key);
  if (it == map.end())
    throw std::runtime_error("Couldn't find parameter " + key + " in mapping.");
  return std::any_cast<ReturnType>(it->second);
}

}  // namespace tyche

#endif /* #ifndef __TYCHE_UTIL_MUST_HPP */
