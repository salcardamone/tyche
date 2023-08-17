/**
 * @brief Unit testing of tensor implementation.
 */
// C++ Standard Libraries
//
// Third-Party Libraries
#include <gtest/gtest.h>
// Project Inclusions
#include "tensor.hpp"

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
