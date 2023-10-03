/**
 * @brief
 */
#ifndef __TYCHE_SYSTEM_CELL_FACTORY_HPP
#define __TYCHE_SYSTEM_CELL_FACTORY_HPP

// C++ Standard Libraries
#include <memory>
#include <cstring>
#include <optional>
// Third-Party Libraries
#include <toml++/toml.h>
#include <spdlog/spdlog.h>
// Project Inclusions
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
  static std::unique_ptr<Cell> create(toml::table cell_config) {
    auto type = cell_config["type"].value<std::string>();
    if (type == std::nullopt) {
      throw std::runtime_error("Cell type must be specified.");
    }
    spdlog::info("Creating cell of type: {}", *type);

    std::unique_ptr<Cell> cell;
    if (*type == "Unbounded") {
      cell = std::make_unique<UnboundedCell>();
    } else if (*type == "Cubic") {
      auto length = cell_config["length"].value<double>();
      if (length == std::nullopt) {
        throw std::runtime_error("Length of cubic cell must be provided.");
      }
      cell = std::make_unique<CubicCell>(*length);
    } else {
      throw std::runtime_error("Unrecognised cell: " + *type);
    }
    return cell;
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SYSTEM_CELL_FACTORY_HPP */
