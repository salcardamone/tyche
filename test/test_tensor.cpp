/**
 * @brief Unit testing of tensor implementation.
 */
// C++ Standard Libraries
//
// Third-Party Libraries
#include <gtest/gtest.h>
// Project Inclusions
#include "tyche/tensor.hpp"

using namespace tyche;

/**
 * @brief Check that we can create tensors of different number of dimensions and
 * sizes.
 */
TEST(TensorTests, Dimensions) {
  {
    Tensor<double, 3> tensor_3d(3, 4, 5);
    ASSERT_EQ(tensor_3d.size(0), 3);
    ASSERT_EQ(tensor_3d.size(1), 4);
    ASSERT_EQ(tensor_3d.size(2), 5);
    ASSERT_EQ(tensor_3d.num_elements(), 60);
  }
  {
    Tensor<float, 4> tensor_4d(3, 4, 5, 6);
    ASSERT_EQ(tensor_4d.size(0), 3);
    ASSERT_EQ(tensor_4d.size(1), 4);
    ASSERT_EQ(tensor_4d.size(2), 5);
    ASSERT_EQ(tensor_4d.size(3), 6);
    ASSERT_EQ(tensor_4d.num_elements(), 360);
  }
}

/**
 * @brief Check the accessor methods for the tensor.
 */
TEST(TensorTests, Access) {
  {
    Tensor<double, 4> tensor_4d(3, 4, 5, 6);
    tensor_4d(1, 1, 1, 1) = 3;
    ASSERT_EQ(tensor_4d(1, 1, 1, 1), 3);
    ASSERT_EQ(*(tensor_4d.begin() + 157), 3);
  }
  {
    Tensor<double, 3> tensor_3d(3, 3, 3);
    tensor_3d(2, 1, 0) = 1;
    tensor_3d(2, 1, 1) = 2;
    tensor_3d(2, 1, 2) = 3;

    auto tensor_it = tensor_3d.offset(2, 1, 0);

    ASSERT_EQ(*tensor_it++, 1);
    ASSERT_EQ(*tensor_it++, 2);
    ASSERT_EQ(*tensor_it, 3);
  }
}

/**
 * @brief Check constructors work as expected.
 */
TEST(TensorTests, Constructors) {
  // Move constructor
  {
    Tensor<double, 4> tensor_4d(3, 4, 5, 6);
    tensor_4d(1, 1, 1, 1) = 10;
    auto moved = std::move(tensor_4d);
    ASSERT_EQ(moved(1, 1, 1, 1), 10);
  }
  // Copy constructor
  {
    Tensor<double, 4> tensor_4d(3, 4, 5, 6);
    tensor_4d(1, 1, 1, 1) = 10;
    auto copied = std::ref(tensor_4d);
    ASSERT_EQ(tensor_4d(1, 1, 1, 1), 10);
    ASSERT_EQ(copied(1, 1, 1, 1), 10);
  }
}

/**
 * @brief Test whether we can resize a tensor as expected.
 */
TEST(TensorTests, Resize) {
  Tensor<double, 4> tensor_4d(1, 2, 3, 4);
  tensor_4d.resize(2, 2, 3, 4);
  ASSERT_EQ(tensor_4d.size(0), 2);
  ASSERT_EQ(tensor_4d.size(1), 2);
  ASSERT_EQ(tensor_4d.size(2), 3);
  ASSERT_EQ(tensor_4d.size(3), 4);
  ASSERT_EQ(tensor_4d.num_elements(), 48);
}

/**
 * @brief Test whether we can concatenate two 2D matrices as expected.
 */
TEST(TensorTests, Concatenation) {
  Tensor<double, 2> tensor_a(std::vector<double>{1, 2, 3, 4, 5, 6}, 3, 2);
  Tensor<double, 2> tensor_b(std::vector<double>{1, 2, 3, 4, 5, 6, 7, 8, 9}, 3,
                             3);
  tensor_a.concatenate(tensor_b);

  ASSERT_EQ(tensor_a.size(0), 3);
  ASSERT_EQ(tensor_a.size(1), 5);
  ASSERT_EQ(tensor_a.num_elements(), 15);

  ASSERT_EQ(tensor_a(0, 0), 1);
  ASSERT_EQ(tensor_a(0, 1), 2);
  ASSERT_EQ(tensor_a(0, 2), 1);
  ASSERT_EQ(tensor_a(0, 3), 2);
  ASSERT_EQ(tensor_a(0, 4), 3);

  ASSERT_EQ(tensor_a(1, 0), 3);
  ASSERT_EQ(tensor_a(1, 1), 4);
  ASSERT_EQ(tensor_a(1, 2), 4);
  ASSERT_EQ(tensor_a(1, 3), 5);
  ASSERT_EQ(tensor_a(1, 4), 6);

  ASSERT_EQ(tensor_a(2, 0), 5);
  ASSERT_EQ(tensor_a(2, 1), 6);
  ASSERT_EQ(tensor_a(2, 2), 7);
  ASSERT_EQ(tensor_a(2, 3), 8);
  ASSERT_EQ(tensor_a(2, 4), 9);
}
