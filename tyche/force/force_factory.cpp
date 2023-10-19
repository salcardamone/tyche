/**
 * @brief
 */
// C++ Standard Libraries
//
// Third-Party Libraries
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/util/must.hpp"
#include "tyche/force/force_factory.hpp"
#include "tyche/force/lennard_jones.hpp"

namespace tyche {

// ========================================================================== //

std::unique_ptr<Force> ForceFactory::create(
    Reader::Mapping config,
    const std::map<std::shared_ptr<AtomType>, std::size_t>& atom_type) {
  auto type = must_find<std::string>(config, "type");
  spdlog::info("Creating force of type: {}", type);

  std::unique_ptr<Force> force;
  if (type == "LennardJones") {
    force = std::make_unique<LennardJones>(atom_type);
  } else {
    throw std::runtime_error("Unrecognised force: " + type);
  }
  return force;
}

// ========================================================================== //

}  // namespace tyche
