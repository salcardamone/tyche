/**
 * @brief
 */
#ifndef __TYCHE_SYSTEM_CELL_FACTORY_HPP
#define __TYCHE_SYSTEM_CELL_FACTORY_HPP

// C++ Standard Libraries
#include <any>
#include <memory>
#include <cstring>
#include <optional>
// Third-Party Libraries
#include <toml++/toml.h>
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/util/maybe.hpp"
#include "tyche/system/cell.hpp"

namespace tyche {

/**
 * @brief Factory pattern to create and return a Cell instance.
 */
class CellFactory {
 public:
  /**
   * @brief Create something that derives from Cell using a TOML configuration.
   * @param cell_config Table of cell paramterisation.
   * @return The cell.
   */
  static std::unique_ptr<Cell> create(std::map<std::string, std::any> config) {
    auto type = maybe_find<std::string>(config, "type");
    spdlog::info("Creating cell of type: {}", type.value());

    std::unique_ptr<Cell> cell;
    try {
      if (type.value() == "Unbounded") {
        cell = std::make_unique<UnboundedCell>();
      } else if (type.value() == "Cubic") {
        // Note that although we're maybe finding doubles, they'll be coerced
        // into optionals of the appropriate numeric type in the constructor
        cell =
            std::make_unique<CubicCell>(maybe_find<double>(config, "length"));
      } else {
        throw std::runtime_error("Unrecognised cell: " + type.value());
      }
    } catch (std::bad_optional_access& err) {
      throw std::runtime_error("Required cell parameters were not found.");
    }

    return cell;
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SYSTEM_CELL_FACTORY_HPP */
