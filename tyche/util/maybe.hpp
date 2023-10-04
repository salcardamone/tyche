/**
 * @brief
 */
#ifndef __TYCHE_UTIL_MAYBE_HPP
#define __TYCHE_UTIL_MAYBE_HPP

// C++ Standard Libraries
#include <any>
#include <map>
#include <sstream>
#include <cstring>
#include <optional>
// Third-Party Libraries
//
// Project Inclusions
//

namespace tyche {

/**
 * @brief Return an optional parameter depending on whether that parameter was
 * contained in a map or not.
 * @tparam ReturnType The type of the returned parameter.
 * @param map The map potentially containined the parameter.
 * @param key The name of the parameter we're looking for.
 * @return An optional potentially containined the requested parameter.
 */
template <typename ReturnType>
static std::optional<ReturnType> maybe_find(std::map<std::string, std::any> map,
                                            std::string key) {
  auto it = map.find(key);
  if (it == map.end()) return std::nullopt;
  return std::optional<ReturnType>(std::any_cast<ReturnType>(it->second));
}

}  // namespace tyche

#endif /* #ifndef __TYCHE_UTIL_MAYBE_HPP */
