/**
 * @brief
 */
#ifndef __TYCHE_SIMULATION_SIMULATION_HPP
#define __TYCHE_SIMULATION_SIMULATION_HPP

// C++ Standard Libraries
//
// Third-Party Libraries
//
// Project Inclusions
//

namespace tyche {

/**
 * @brief Base simulation class from which others can derive.
 */
class Simulation {
 public:
  virtual ~Simulation() {}

  /**
   * @brief Abstract run method to be implemented by children classes.
   */
  virtual void run() = 0;
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_SIMULATION_SIMULATION_HPP */
