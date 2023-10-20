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
#include "tyche/util/must.hpp"
#include "tyche/io/writer.hpp"
#include "tyche/atom/atomic_state_writer.hpp"
#include "tyche/system/thermodynamics_writer.hpp"

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
   * @param integrator Optional Integrator instance to be passed to any writers
   * needing that state.
   * @return The writer.
   */
  static std::unique_ptr<Writer> create(
      std::map<std::string, std::any> config,
      std::optional<std::shared_ptr<AtomicState>> atomic_state = std::nullopt,
      std::optional<std::shared_ptr<Integrate>> integrator = std::nullopt) {
    auto type = must_find<std::string>(config, "type");
    auto path = must_find<std::string>(config, "path");
    spdlog::info("Creating writer of type: " + type + ", writing to: " + path);

    std::unique_ptr<Writer> writer;
    if (type == "xyz") {
      writer = std::make_unique<AtomicStateWriterXYZ>(
          std::filesystem::path(path), atomic_state.value());
    } else if (type == "therm") {
      writer = std::make_unique<ThermodynamicsWriter>(
          std::filesystem::path(path),
          std::dynamic_pointer_cast<DynamicAtomicState>(atomic_state.value()),
          integrator.value());
    } else {
      throw std::runtime_error("Unrecognised writer: " + type);
    }
    return writer;
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_IO_WRITER_FACTORY_HPP */
