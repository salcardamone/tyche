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
#include <spdlog/spdlog.h>
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

  /**
   * @brief Empty constructor; everything is zero-sized -- defer meaningful
   * creation for being moved to at a later point.
   */
  Tensor() : dim_size_{0}, data_(0) {}

  Tensor(const Tensor<DataType, NumDims>&) = default;
  Tensor(Tensor<DataType, NumDims>&&) = default;
  Tensor& operator=(const Tensor<DataType, NumDims>&) = default;

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
   * @brief Return the dimensionality of the Tensor.
   * @return The dimensionality of the Tensor.
   */
  constexpr std::size_t dims() { return NumDims; }

  /**
   * @brief Zero the contents of the Tensor.
   */
  void zero() { std::fill(data_.begin(), data_.end(), 0); }

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

  /**
   * @brief Resize each of the tensor's dimensions; if the tensor ends up larger
   * than before, zeros will be used as fill elements.
   * @param args Parameter pack containing the new dimension indices.
   */
  template <typename... Args>
  void resize(Args... args) {
    verify_dims(args...);

    dim_size_ = {static_cast<std::size_t>(args)...};

    stride_[NumDims - 1] = 1;
    for (int idim = NumDims - 2; idim >= 0; --idim) {
      stride_[idim] = stride_[idim + 1] * dim_size_[idim + 1];
    }

    data_.resize(num_elements(), 0);
  }

  /**
   * @brief Horizontally concatenate this tensor with the argument tensor.
   * This is only valid when the tensors are 2D matrices.
   * @param other Tensor to concatenate with this Tensor.
   */
  void concatenate(Tensor<DataType, NumDims>& other) {
    static_assert(NumDims == 2, "Tensor concatenation requires 2D Tensors.");
    assert(size(0) == other.size(0));

    std::array<std::size_t, NumDims> new_size{dim_size_};
    new_size[1] += other.size(1);
    auto old_size = dim_size_;

    resize(new_size[0], new_size[1]);

    // Fill in additional space between rows for concatenation of new matrix,
    // then dump the data from the other matrix into that space
    for (std::size_t irow = 0; irow < new_size[0]; ++irow) {
      // Where to start placing B in resized A
      auto start_it = data_.begin() + irow * new_size[1] + old_size[1];
      // Where final element of A is in resized A
      auto end_it = start_it + old_size[1] * (old_size[0] - irow - 1);
      // Where to place the data in resized A
      auto dest_it = start_it + other.size(1);

      // We can't copy directly into the resized A else we'll overwrite the
      // stuff we're trying to copy, hence we duplicate the elements to copy
      std::vector<DataType> temp(start_it, end_it);
      // Create additional space for the concatenation
      std::copy_if(
          temp.begin(), temp.end(), dest_it,
          [irow, new_size](DataType x) { return irow < new_size[0] - 1; });
      // Copy the other matrix's data into the space
      std::copy(other.begin() + irow * other.size(1),
                other.begin() + (irow + 1) * other.size(1), start_it);
    }
  }

  /**
   * @brief Compute the inner product in a 2D Tensor between either rows or
   * columns.
   * @tparam Dim The dimension we're indexing into; when Dim = 0, this
   * corresponds to the slowest changing dimension, i.e. the rows, while when
   * Dim = NumDims - 1, it corresponds to the fastest changing dimension, i.e.
   * the columns.
   * @param idx Index into the appropriate dimension to extract the first vector
   * from the Tensor.
   * @param jdx Index into the appropriate dimension to extract the second
   * vector from the Tensor.
   * @return The inner product between the two vectors.
   */
  template <std::size_t Dim>
  DataType inner_product(std::size_t idx, std::size_t jdx) {
    static_assert(NumDims == 2,
                  "Inner product requires the tensor dimensionality by two.");
    constexpr std::size_t inverted_dim = NumDims - 1 - Dim;
    // Stride to move to the next vector along specified dimension
    const std::size_t stride = stride_[Dim];
    // Increment to move to the next element along the specified dimension
    const std::size_t incr = stride_[inverted_dim];

    double dot = 0;
    const_iterator i_it = begin() + idx * stride, j_it = begin() + jdx * stride;
    for (std::size_t kdx = 0; kdx < size(inverted_dim); ++kdx) {
      dot += *i_it * *j_it;
      i_it += incr;
      j_it += incr;
    }
    return dot;
  }

  /**
   * @brief Compute the inner product between vectors in two 2D Tensors.
   * @tparam DimA The dimension we're indexing into for the first tensor.
   * @tparam DimB The dimension we're indexing into for the second tensor.
   * @param tensor_a The first Tensor.
   * @param tensor_b The second Tensor.
   * @param a_idx Index into the appropriate dimension to extract the vector
   * from the first Tensor.
   * @param b_idx Index into the appropriate dimension to extract the vector
   * from the second Tensor.
   * @return The inner product between the two vectors.
   */
  template <std::size_t DimA, std::size_t DimB>
  static DataType inner_product(Tensor<DataType, NumDims>& tensor_a,
                                Tensor<DataType, NumDims>& tensor_b,
                                std::size_t a_idx, std::size_t b_idx) {
    static_assert(NumDims == 2,
                  "Inner product requires the tensor dimensionality by two.");
    constexpr std::size_t inverted_dim_a = NumDims - 1 - DimA;
    constexpr std::size_t inverted_dim_b = NumDims - 1 - DimB;
    assert(tensor_a.size(inverted_dim_a) == tensor_b.size(inverted_dim_b));

    const std::size_t stride_a = tensor_a.stride_[DimA],
                      incr_a = tensor_a.stride_[inverted_dim_a];
    const std::size_t stride_b = tensor_b.stride_[DimB],
                      incr_b = tensor_b.stride_[inverted_dim_b];

    double dot = 0;
    const_iterator a_it = tensor_a.begin() + a_idx * stride_a;
    const_iterator b_it = tensor_b.begin() + b_idx * stride_b;
    for (std::size_t c_idx = 0; c_idx < tensor_a.size(inverted_dim_a);
         ++c_idx) {
      dot += *a_it * *b_it;
      a_it += incr_a;
      b_it += incr_b;
    }

    return dot;
  }

  /**
   * @brief << operator overload for output stream iterators. Dump some
   * information to the output stream iterator.
   *
   * Requires the Tensor be 2D, and dumps contents in matrix format.
   * @param os The output stream iterator.
   * @param tensor The Tensor to dump information about.
   * @return The modified output stream iterator.
   */
  friend std::ostream& operator<<(std::ostream& os,
                                  Tensor<DataType, NumDims>& tensor) {
    static_assert(
        NumDims == 2,
        "Printing of a Tensor requires the tensor dimensionality be two.");

    os << "Tensor has size : " << tensor.size(0) << " x " << tensor.size(1)
       << '\n';
    for (std::size_t irow = 0; irow < tensor.size(0); ++irow) {
      for (std::size_t icol = 0; icol < tensor.size(1); ++icol) {
        os << tensor(irow, icol) << ", ";
      }
      os << '\n';
    }
    return os;
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

}  // namespace tyche

/**
 * @brief Formatter for the Tensor object that allows us to print information
 * with spdlog.
 */
template <typename DataType, std::size_t NumDims>
struct fmt::formatter<tyche::Tensor<DataType, NumDims>>
    : fmt::formatter<std::string> {
  auto format(tyche::Tensor<DataType, NumDims>& tensor, format_context& ctx)
      -> decltype(ctx.out()) {
    std::ostringstream ss;
    ss << tensor;
    return format_to(ctx.out(), "{}", ss.str());
  }
};

#endif /* #ifndef __TYCHE_TENSOR_HPP */
