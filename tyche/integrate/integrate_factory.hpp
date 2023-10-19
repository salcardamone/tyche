/**
 * @brief
 */
#ifndef __TYCHE_INTEGRATE_INTEGRATE_FACTORY_HPP
#define __TYCHE_INTEGRATE_INTEGRATE_FACTORY_HPP

// C++ Standard Libraries
#include <memory>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/io/reader.hpp"
#include "tyche/integrate/integrate.hpp"

namespace tyche {

/**
 * @brief Factory pattern to return an integrator.
 */
class IntegrateFactory {
 public:
  /**
   * @brief Create an integrator from a map of parameters and their values.
   * @param config Mapping from Integrator parameter keys to values.
   * @return The integrator.
   */
  static std::unique_ptr<Integrate> create(Reader::Mapping config);

 private:
  /**
   * @brief Create an integrator that uses the Velocity Verlet convention.
   * @param config Mapping from Integrator parameter keys to values.
   * @param timestep The integration timestep.
   * @param num_steps The number of steps to take for the simulation.
   * @return The integrator.
   */
  static std::unique_ptr<Integrate> select_velocity_verlet(
      Reader::Mapping config, double timestep, std::size_t num_steps);
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_INTEGRATE_INTEGRATE_FACTORY_HPP */
