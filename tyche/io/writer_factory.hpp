/**
 * @brief
 */
#ifndef __TYCHE_IO_WRITER_FACTORY_HPP
#define __TYCHE_IO_WRITER_FACTORY_HPP

// C++ Standard Libraries
#include <any>
#include <map>
#include <memory>
#include <optional>
#include <filesystem>
// Third-Party Libraries
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/util/maybe.hpp"
#include "tyche/io/writer.hpp"
#include "tyche/atom/atomic_state_writer.hpp"

namespace tyche {

/**
 * @brief Factory to create instances of classes derived from Writer.
 */
class WriterFactory {
 public:
  /**
   * @brief Create a writer from a map of parameters and their values.
   * @param config Mapping from writer parameter keys to values.
   * @param atomic_state Optional AtomicState instance to be passed to any
   * writers deriving from AtomicStateWriter.
   * @return The writer.
   */
  static std::unique_ptr<Writer> create(
      std::map<std::string, std::any> config,
      std::optional<std::shared_ptr<AtomicState>> atomic_state = std::nullopt) {
    auto type = maybe_find<std::string>(config, "type");
    spdlog::info("Creating writer of type: " + type.value());

    std::unique_ptr<Writer> writer;
    try {
      auto path = maybe_find<std::string>(config, "path");
      if (type.value() == "xyz") {
        writer = std::make_unique<AtomicStateWriterXYZ>(
            std::filesystem::path(path.value()), atomic_state.value());
      } else {
        throw std::runtime_error("Unrecognised writer: " + type.value());
      }
    } catch (std::bad_optional_access& err) {
      throw std::runtime_error("Required writer parameters were not found.");
    }
    return writer;
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_IO_WRITER_FACTORY_HPP */
