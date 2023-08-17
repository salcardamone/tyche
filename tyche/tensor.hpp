/**
 * @brief Tensor implementation.
 */
#ifndef __TYCHE_TENSOR_HPP
#define __TYCHE_TENSOR_HPP

// C++ Standard Libraries
#include <array>
#include <vector>
#include <numeric>
#include <cassert>
#include <iostream>
#include <functional>
// Third-Party Libraries
//
// Project Inclusions
//

namespace tyche {

/**
 * @brief Multidimensional tensor class with compile-time number of dimensions.
 * @tparam DataType The underlying datatype the tensor stores.
 * @tparam NumDims The dimensionality of the tensor.
 */
template <typename DataType, std::size_t NumDims>
class Tensor {
 public:
  // Leak the underlying container types so we can just iterate on the
  // std::vector if required
  using container = std::vector<DataType>;
  using iterator = typename container::iterator;
  using const_iterator = typename container::const_iterator;

  /**
   * @brief Class constructor.
   * @params args Parameter pack containing the dimension sizes.
   */
  template <typename... Args>
  Tensor(Args... args)
      : dim_size_{static_cast<std::size_t>(args)...}, data_(num_elements()) {
    verify_dims(args...);

    stride_[NumDims - 1] = 1;
    for (int idim = NumDims - 2; idim >= 0; --idim) {
      stride_[idim] = stride_[idim + 1] * dim_size_[idim + 1];
    }
  }

  /**
   * @brief Class constructor. Fill the underlying data container with data from
   * a provided container.
   * @param data Data for the tensor; must be of the same size as the tensor
   * size as deduced from remaining arguments.
   * @param args Parameter ack containing the dimension sizes.
   */
  template <typename... Args>
  Tensor(std::vector<DataType>&& data, Args... args) : Tensor(args...) {
    assert(data.size() == num_elements());
    data_ = std::move(data);
  }

  Tensor(Tensor<DataType,NumDims>&) = default;
  Tensor(const Tensor<DataType,NumDims>&) = default;
  Tensor(Tensor<DataType,NumDims>&&) = default;
  
  /**
   * @brief Constant getter from the underlying std::vector at query position.
   * @param args Parameter pack containing the dimension indices.
   * @return Value stored at query dimension indices.
   */
  template <typename... Args>
  const DataType& operator()(Args... args) const {
    verify_dims(args...);
    return data_[flat_idx(args...)];
  }

  /**
   * @brief Non-constant getter from the underlying std::vector at query
   * position.
   * @param args Parameter pack containing the dimension indices.
   * @return Reference to value stored at query dimension indices.
   */
  template <typename... Args>
  DataType& operator()(Args... args) {
    verify_dims(args...);
    return data_[flat_idx(args...)];
  }

  /**
   * @brief Return iterator to beginning of data_ member.
   * @return Iterator to beginning of data_member.
   */
  iterator begin() { return data_.begin(); }

  /**
   * @brief Return iterator to end of data_ member.
   * @return Iterator to end of data_member.
   */
  iterator end() { return data_.end(); }

  /**
   * @brief Return constant iterator to beginning of data_ member.
   * @return Constant iterator to beginning of data_member.
   */
  const_iterator begin() const { return data_.begin(); }

  /**
   * @brief Return constant iterator to end of data_ member.
   * @return Constant iterator to end of data_member.
   */
  const_iterator end() const { return data_.end(); }

  /**
   * @brief Return iterator to some point within the tensor.
   * @param args Parameter pack containing the dimension indices.
   * @return Iterator to some point within the tensor.
   */
  template <typename... Args>
  iterator offset(Args... args) {
    verify_dims(args...);
    return begin() + flat_idx(args...);
  }

  /**
   * @brief Return constant iterator to some point within the tensor.
   * @param args Parameter pack containing the dimension indices.
   * @return Constant iterator to some point within the tensor.
   */
  template <typename... Args>
  const_iterator offset(Args... args) const {
    verify_dims(args...);
    return begin() + flat_idx(args...);
  }

  /**
   * @brief Query the number of elements contained within the tensor.
   * @return The number of elements containined within the tensor.
   */
  std::size_t num_elements() const {
    return std::accumulate(std::begin(dim_size_), std::end(dim_size_), 1,
                           std::multiplies<DataType>());
  }

  /**
   * @brief Return the size of a query dimension.
   * @param idim The dimension to query.
   * @return The number of elements along a given dimension.
   */
  std::size_t size(std::size_t idim) const {
    assert(idim < NumDims);
    return dim_size_[idim];
  }
  
 private:
  std::array<std::size_t, NumDims> dim_size_, stride_;
  std::vector<DataType> data_;

  /**
   * @brief Make sure the parameter pack length matches the dimensionality of
   * the tensor.
   * @param args Parameter pack containing the dimension indices.
   */
  template <typename... Args>
  constexpr void verify_dims(Args... args) const {
    static_assert(sizeof...(args) == NumDims,
                  "Number of indices provided does not match the "
                  "dimensionality of the tensor.");
  }

  /**
   * @brief Return the flattened index into the tensor from the individual
   * dimension indices.
   * @param args Parameter pack containing the dimension indices.
   * @return The flattened index into the underlying data_ member.
   */
  template <typename... Args>
  std::size_t flat_idx(Args... args) const {
    std::array<std::size_t, NumDims> indices{static_cast<std::size_t>(args)...};
    std::size_t idx = 0;
    for (std::size_t idim = 0; idim < NumDims; ++idim) {
      idx += indices[idim] * stride_[idim];
    }
    return idx;
  }
};

} // namespace tyche
  
#endif /* #ifndef __TYCHE_TENSOR_HPP */
