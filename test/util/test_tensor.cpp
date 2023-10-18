/**
 * @brief Unit testing of tensor implementation.
 */
// C++ Standard Libraries
//
// Third-Party Libraries
#include <gtest/gtest.h>
// Project Inclusions
#include "tyche/util/tensor.hpp"

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
  // Row-concatenation
  {
    Tensor<double, 2> tensor_a(std::vector<double>{1, 2, 3, 4, 5, 6}, 3, 2);
    Tensor<double, 2> tensor_b(std::vector<double>{1, 2, 3, 4, 5, 6, 7, 8, 9},
                               3, 3);
    tensor_a.concatenate<0>(tensor_b);

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
  // Column-concatenation
  {
    Tensor<double, 2> tensor_a(std::vector<double>{1, 2, 3, 4, 5, 6}, 3, 2);
    Tensor<double, 2> tensor_b(std::vector<double>{7, 8, 9, 10}, 2, 2);
    tensor_a.concatenate<1>(tensor_b);

    ASSERT_EQ(tensor_a.size(0), 5);
    ASSERT_EQ(tensor_a.size(1), 2);
    ASSERT_EQ(tensor_a.num_elements(), 10);

    ASSERT_EQ(tensor_a(0, 0), 1);
    ASSERT_EQ(tensor_a(0, 1), 2);

    ASSERT_EQ(tensor_a(1, 0), 3);
    ASSERT_EQ(tensor_a(1, 1), 4);

    ASSERT_EQ(tensor_a(2, 0), 5);
    ASSERT_EQ(tensor_a(2, 1), 6);

    ASSERT_EQ(tensor_a(3, 0), 7);
    ASSERT_EQ(tensor_a(3, 1), 8);

    ASSERT_EQ(tensor_a(4, 0), 9);
    ASSERT_EQ(tensor_a(4, 1), 10);
  }
}

/**
 * @brief Various inner product methods.
 */
TEST(TensorTests, InnerProduct) {
  // Inner products within a single tensor
  {
    Tensor<double, 2> tensor(std::vector<double>{1, 2, 3, 4, 5, 6, 7, 8, 9}, 3,
                             3);
    // 1 * 4 + 2 * 5 + 3 * 6 = 4 + 10 + 18
    ASSERT_EQ(tensor.inner_product<0>(0, 1), 32);
    // 1 * 7 + 2 * 8 + 3 * 9 = 7 + 16 + 27
    ASSERT_EQ(tensor.inner_product<0>(0, 2), 50);
    // 4 * 7 + 5 * 8 + 6 * 9 = 28 + 40 + 54
    ASSERT_EQ(tensor.inner_product<0>(1, 2), 122);

    // 1 * 2 + 4 * 5 + 7 * 8 = 2 + 20 + 56
    ASSERT_EQ(tensor.inner_product<1>(0, 1), 78);
    // 1 * 3 + 4 * 6 + 7 * 9 = 3 + 24 + 63
    ASSERT_EQ(tensor.inner_product<1>(0, 2), 90);
    // 2 * 3 + 5 * 6 + 8 * 9 = 6 + 30 + 72
    ASSERT_EQ(tensor.inner_product<1>(1, 2), 108);
  }

  // Inner products between tensors
  {
    auto row_row = Tensor<double, 2>::inner_product<0, 0>;
    auto col_col = Tensor<double, 2>::inner_product<1, 1>;
    auto row_col = Tensor<double, 2>::inner_product<0, 1>;
    auto col_row = Tensor<double, 2>::inner_product<1, 0>;

    Tensor<double, 2> tensor_a(std::vector<double>{1, 3, 6, 4}, 2, 2);
    Tensor<double, 2> tensor_b(std::vector<double>{2, 1, 7, 9}, 2, 2);

    // 1 * 2 + 3 * 1 = 2 + 3
    ASSERT_EQ((row_row(tensor_a, tensor_b, 0, 0)), 5);
    // 1 * 7 + 3 * 9 = 7 + 27
    ASSERT_EQ((row_row(tensor_a, tensor_b, 0, 1)), 34);
    // 6 * 2 + 4 * 1 = 12 + 4
    ASSERT_EQ((row_row(tensor_a, tensor_b, 1, 0)), 16);
    // 6 * 7 + 4 * 9 = 42 + 36
    ASSERT_EQ((row_row(tensor_a, tensor_b, 1, 1)), 78);

    // 1 * 2 + 6 * 7 = 2 + 42
    ASSERT_EQ((col_col(tensor_a, tensor_b, 0, 0)), 44);
    // 1 * 1 + 6 * 9 = 1 + 54
    ASSERT_EQ((col_col(tensor_a, tensor_b, 0, 1)), 55);
    // 3 * 2 + 4 * 7 = 6 + 28
    ASSERT_EQ((col_col(tensor_a, tensor_b, 1, 0)), 34);
    // 3 * 1 + 4 * 9 = 3 + 36
    ASSERT_EQ((col_col(tensor_a, tensor_b, 1, 1)), 39);

    // 1 * 2 + 3 * 7 = 2 + 21
    ASSERT_EQ((row_col(tensor_a, tensor_b, 0, 0)), 23);
    // 1 * 1 + 3 * 9 = 1 + 27
    ASSERT_EQ((row_col(tensor_a, tensor_b, 0, 1)), 28);
    // 6 * 2 + 4 * 7 = 12 + 28
    ASSERT_EQ((row_col(tensor_a, tensor_b, 1, 0)), 40);
    // 6 * 1 + 4 * 9 = 6 + 36
    ASSERT_EQ((row_col(tensor_a, tensor_b, 1, 1)), 42);

    // 1 * 2 + 6 * 1 = 2 + 6
    ASSERT_EQ((col_row(tensor_a, tensor_b, 0, 0)), 8);
    // 1 * 7 + 6 * 9 = 7 + 54
    ASSERT_EQ((col_row(tensor_a, tensor_b, 0, 1)), 61);
    // 3 * 2 + 4 * 1 = 6 + 4
    ASSERT_EQ((col_row(tensor_a, tensor_b, 1, 0)), 10);
    // 3 * 7 + 4 * 9 = 21 + 36
    ASSERT_EQ((col_row(tensor_a, tensor_b, 1, 1)), 57);
  }
}
