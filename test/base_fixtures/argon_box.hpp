/**
 * @brief
 */
#ifndef __TYCHE_TEST_BASE_FIXTURES_ARGON_BOX_HPP
#define __TYCHE_TEST_BASE_FIXTURES_ARGON_BOX_HPP

// C++ Standard Libraries
//
// Third-Party Libraries
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/atom_type_reader.hpp"
#include "tyche/atomic_state.hpp"
#include "tyche/cell.hpp"

using namespace tyche;
using namespace std::string_view_literals;

/**
 * @brief Box of Argon test fixture.
 */
class ArgonBox : public ::testing::Test {
 public:
  /**
   * @brief Arrange some given number of Argon atoms into a given box
   * configuration.
   * @param num_atoms The number of atoms to initialise. Note that this will be
   * reduced to the closest cube number.
   * @param input_cell The cubic cell that's being used to confine the system.
   */
  void SetUp(std::size_t num_atoms, std::shared_ptr<CubicCell> input_cell) {
    std::size_t atoms_per_dim = CommonSetUp(num_atoms);
    cell = input_cell;
    InitialiseCrystal(atoms_per_dim);
  }

  /**
   * @brief Arrange some given number of argon atoms into a box where the box
   * length is chosen to yield the density of Argon at STP.
   * @param num_atoms The number of atoms to initialise. Note that this will be
   * reduced to the closest cube number.
   */
  void SetUp(std::size_t num_atoms) {
    std::size_t atoms_per_dim = CommonSetUp(num_atoms);
    num_atoms = atoms_per_dim * atoms_per_dim * atoms_per_dim;

    // Choose cell length to match density of Argon
    double total_mass = num_atoms * atom_types["Ar"]->mass();
    double cell_length = std::pow(total_mass / density, 1. / 3.);
    cell = std::make_shared<CubicCell>(cell_length);

    InitialiseCrystal(atoms_per_dim);
  }

 protected:
  std::shared_ptr<CubicCell> cell;
  AtomicState atomic_state;
  std::map<std::string, std::shared_ptr<AtomType>> atom_types;

  static constexpr std::string_view toml = R"(
    [AtomTypes.Ar]      
  )"sv;
  // Density from Wikipedia of Argon under STP is 1.784 kg / litre
  static constexpr double density =
      1.784E-3 * constants::kg_to_dalton / constants::litre_to_cubic_angstrom;

  /**
   * @brief Common setup amongst all SetUp methods.
   * @param num_atoms The number of atoms to use in the box.
   * @return The closest value lower than the input num_atoms that's a perfect
   * cube.
   */
  std::size_t CommonSetUp(std::size_t num_atoms) {
    toml::table config = toml::parse(toml);
    AtomTypeReader reader(config);
    atom_types = reader.parse();

    atomic_state = AtomicState(true, true);

    return std::floor(std::pow(num_atoms, 1. / 3.));
  }

  /**
   * @brief Initialise atoms in the box in a cubic crystal configuration.
   * @param atoms_per_dim The number of atoms per spatial dimension to
   * initialise.
   */
  void InitialiseCrystal(std::size_t atoms_per_dim) {
    std::size_t num_atoms = atoms_per_dim * atoms_per_dim * atoms_per_dim;
    double delta = cell->length() / atoms_per_dim;

    std::vector<std::shared_ptr<AtomType>> types(num_atoms);
    Tensor<double, 2> pos(num_atoms, 3);

    std::size_t iatom = 0;
    for (std::size_t ix = 0; ix < atoms_per_dim; ++ix) {
      for (std::size_t iy = 0; iy < atoms_per_dim; ++iy) {
        for (std::size_t iz = 0; iz < atoms_per_dim; ++iz) {
          pos(iatom, 0) = ix * delta;
          pos(iatom, 1) = iy * delta;
          pos(iatom, 2) = iz * delta;
          types[iatom] = atom_types["Ar"];
          ++iatom;
        }
      }
    }
    atomic_state.add(std::move(types), std::move(pos));
  }
};

#endif /* #ifndef __TYCHE_TEST_BASE_FIXTURES_ARGON_BOX_HPP */
