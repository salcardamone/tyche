/**
 * @brief
 */
// C++ Standard Libraries
//
// Third-Party Libraries
#include <gtest/gtest.h>
// Project Inclusions
#include "tyche/system/cell.hpp"

using namespace tyche;

/**
 * @brief Make sure we can't improperly initialise cubic cell.
 */
TEST(TestCubicCell, NegativeCellLength) {
  EXPECT_THROW(CubicCell(-1.0), std::runtime_error);
}

/**
 * @brief Check cubic cell periodic boundary condition application.
 */
TEST(TestCubicCell, PeriodicBoundaries) {
  CubicCell cell(2.0);
  ASSERT_EQ(cell.volume(), 8);

  // One dimension on boundary
  double x = 2.0, y = 0.0, z = 0.0;
  cell.pbc(x, y, z);
  ASSERT_EQ(x, 0.0);
  ASSERT_EQ(y, 0.0);
  ASSERT_EQ(z, 0.0);

  // All dimensions on boundaries
  x = 2.0;
  y = 2.0;
  z = 2.0;
  cell.pbc(x, y, z);
  ASSERT_EQ(x, 0.0);
  ASSERT_EQ(y, 0.0);
  ASSERT_EQ(z, 0.0);

  // Past boundaries in positive direction
  x = 2.5;
  y = 0.5;
  z = 8.25;
  cell.pbc(x, y, z);
  ASSERT_EQ(x, 0.5);
  ASSERT_EQ(y, 0.5);
  ASSERT_EQ(z, 0.25);

  // Past boundaries in negative direction
  x = -1.0;
  y = -2.5;
  z = -8.5;
  cell.pbc(x, y, z);
  ASSERT_EQ(x, 1.0);
  ASSERT_EQ(y, 1.5);
  ASSERT_EQ(z, 1.5);
}
