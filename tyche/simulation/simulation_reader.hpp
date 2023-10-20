/**
 * @brief
 */
#ifndef __TYCHE_SIMULATION_SIMULATION_READER_HPP
#define __TYCHE_SIMULATION_SIMULATION_READER_HPP

// C++ Standard Libraries
#include <string>
#include <memory>
// Third-Party Libraries
#include <toml++/toml.h>
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/io/reader.hpp"
#include "tyche/io/toml_reader.hpp"
#include "tyche/atom/atomic_state.hpp"
#include "tyche/simulation/simulation.hpp"
#include "tyche/simulation/simulation_factory.hpp"

namespace tyche {

/**
 * @brief Reader for simulation.
 *
 * This is a bit different from the other readers (AtomType and AtomicState)
 * since Simulation is polymorphic, hence we need to return a base pointer
 * regardless of the type of simulation we're instantiated.
 *
 * Furthermore, a Simulation is pretty much just a wrapper around other objects
 * (integrator, cell, forces, etc.) that are specific to the simulation type.
 * Hence, SimulationReader just parses the entirety of the
 * configuration file into a nested mapping, and passes the appropriate mapping
 * onto the component creational routines.
 *
 * The motivation is that we want to decouple the component creational routines
 * from anything to do with the configuration file format, e.g., we want to get
 * rid of any mention of TOML as soon as possible.
 */
class SimulationReader : public TOMLReader<std::unique_ptr<Simulation>> {
 public:
  /**
   * @brief Class constructor.
   * @param atomic_state The atomic state we're simulating. This is cast to the
   * base AtomicState, and coerced into its derived implementation in the
   * factory that SimulationReader defers to.
   */
  SimulationReader(std::shared_ptr<AtomicState> atomic_state)
      : atomic_state_{atomic_state} {}

  /**
   * @brief Parse the simulation from the TOML config.
   * @param config "Simulation" node of the TOML.
   * @return The instantiated Simulation.
   */
  std::unique_ptr<Simulation> parse(toml::table& config) {
    // There should only be a single node at the "Simulation" level, hence parse
    // it and we can use it in the SimulationFactory
    if (config.size() != 1) {
      throw std::runtime_error(
          "Multiple Simulation types specified in the configuration.");
    }
    std::string simulation_type{config.begin()->first};
    spdlog::info("Found Simulation type: {}", simulation_type);

    Reader::Mapping config_mapping = parse_table(config);
    config_mapping =
        Reader::remove_prefix(config_mapping, simulation_type + ".");
    config_mapping["type"] = simulation_type;

    return SimulationFactory::create(config_mapping, atomic_state_);
  }

 private:
  std::shared_ptr<AtomicState> atomic_state_;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SIMULATION_SIMULATION_READER_HPP */
