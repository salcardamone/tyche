/**
 * @brief
 */
#ifndef __TYCHE_INTEGRATE_INTEGRATE_FACTORY_HPP
#define __TYCHE_INTEGRATE_INTEGRATE_FACTORY_HPP

// C++ Standard Libraries
#include <memory>
#include <cstring>
// Third-Party Libraries
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/integrate/integrate.hpp"
#include "tyche/integrate/velocity_verlet.hpp"

namespace tyche {

/**
 * @brief Factory pattern to return an integrator.
 */
class IntegrateFactory {
 public:
  /**
   * @brief Create something that derives from Integrate.
   * @param type The type of integrator to create.
   * @param dt Time increment for simulation step in femtoseconds.
   * @return The integrator.
   */
  static std::unique_ptr<Integrate> create(const std::string& type, double dt) {
    std::unique_ptr<Integrate> integrator;
    spdlog::info("Creating integrator of type: {}", type);
    if (type == "VelocityVerlet") {
      integrator = std::make_unique<VelocityVerlet>(dt);
    } else {
      throw std::runtime_error("Unrecognised integrator: " + type);
    }
    return integrator;
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_INTEGRATE_INTEGRATE_FACTORY_HPP */
