/**
 * @brief
 */
#ifndef __TYCHE_SYSTEM_THERMODYNAMICS_WRITER_HPP
#define __TYCHE_SYSTEM_THERMODYNAMICS_WRITER_HPP

// C++ Standard Libraries
#include <iomanip>
// Third-Party Libraries
//
// Project Inclusions
#include "tyche/atom/dynamic_atomic_state.hpp"
#include "tyche/atom/atomic_state_writer.hpp"
#include "tyche/system/thermostat.hpp"
#include "tyche/integrate/integrate.hpp"

namespace tyche {

/**
 * @brief Writer for thermodynamic information.
 */
class ThermodynamicsWriter : public AtomicStateWriter<DynamicAtomicState> {
 public:
  /**
   * @brief Class constructor.
   * @param filename Writer output file.
   * @param atomic_state The state we'll be writing about.
   * @param integrator The integrator we're propagating the atomic state with.
   */
  ThermodynamicsWriter(std::filesystem::path filename,
                       std::shared_ptr<DynamicAtomicState> atomic_state,
                       std::shared_ptr<Integrate> integrator)
      : AtomicStateWriter(filename, atomic_state), integrator_(integrator) {
    output() << "Timestep"
             << "Temperature / K" << std::endl;
  }

  /**
   * @brief Add thermodynamic information to the .therm file.
   * @param comment Unused -- abstract base method requires is.
   */
  void write(std::optional<std::string> comment = std::nullopt) override {
    output() << integrator_->current_step()
             << Thermostat::temperature(*atomic_state_) << std::endl;
  }

 protected:
  std::shared_ptr<Integrate> integrator_;

 private:
  /**
   * @brief Set some stream configuration and return the output stream.
   * @return The output stream configured for output.
   */
  std::ofstream& output() {
    constexpr int width = 20;
    ofs_ << std::left << std::setw(width);
    return ofs_;
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SYSTEM_THERMODYNAMICS_WRITER_HPP */
