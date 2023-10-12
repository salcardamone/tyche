/**
 * @brief
 */
#ifndef __TYCHE_IO_TOML_READER_HPP
#define __TYCHE_IO_TOML_READER_HPP

// C++ Standard Libraries
#include <vector>
#include <cstring>
#include <optional>
// Third-Party Libraries
#include <toml++/toml.h>
#include <spdlog/spdlog.h>
// Project Inclusions
#include "tyche/io/reader.hpp"
#include "tyche/util/tensor.hpp"

namespace tyche {

/**
 * @brief Reader for a TOML file.
 * @tparam ReturnType The class we're reading.
 */
template <class ReturnType>
class TOMLReader : public Reader {
 public:
  using Reader::Mapping;
  /**
   * @brief Abstract base parser. Take a TOML table and return an object
   * associated with the reader.
   * @param config The TOML table we're reading from.
   * @return The completely constructed object we've read.
   */
  virtual ReturnType parse(toml::table& config) = 0;

 protected:
  /**
   * @brief Parse a TOML table into a mapping from keys into their values.
   * @param config The table we're parsing.
   * @param prefix Optional string to prepend to key values.
   * @return Mapping from keys to values. The values are std::any, either a
   * string or double. Double is chosen for any numeric type because it can be
   * coerced into an integer.
   */
  Mapping parse_table(toml::table config,
                      std::optional<std::string> prefix = std::nullopt) {
    Mapping mapping;

    config.for_each([&](const toml::key& key, auto&& val) {
      // If we have a prefix, then make sure we prepend it to the key
      std::string key_val("");
      if (prefix != std::nullopt) {
        key_val += prefix.value() + ".";
      }
      key_val += key.str();
      if constexpr (toml::is_number<decltype(val)>) {
        mapping[key_val] = static_cast<double>(val.get());
      } else if constexpr (toml::is_string<decltype(val)>) {
        mapping[key_val] = val.get();
      } else if constexpr (toml::is_table<decltype(val)>) {
        // Recursively descend tables, using current key value as prefix for all
        // keys therein
        auto subtable_map =
            parse_table(val, std::optional<std::string>(key_val));
        mapping.insert(subtable_map.begin(), subtable_map.end());
      } else {
        spdlog::error("Unrecognised type of TOML entry.");
      }
    });
    return mapping;
  }

  /**
   * @brief Parse a TOML array of TOML tables into an iterable of mappings from
   * keys into their values.
   * @param array The array we're parsing containing TOML tables.
   * @return An iterable of mappings from keys to values.
   */
  std::vector<Mapping> parse_array_of_tables(toml::array array) {
    std::vector<Mapping> mappings;
    for (auto&& table : array) {
      mappings.push_back(parse_table(*table.as_table()));
    }
    return mappings;
  }

  /**
   * @brief Parse a TOML array of TOML arrays into a 2D tensor.
   * @tparam DataType The datatype stored within the matrix.
   * @param array The array we're parsing containing TOML arrays.
   * @return A 2D tensor containing the data.
   */
  template <typename DataType>
  Tensor<DataType, 2> parse_matrix(toml::array array) {
    std::size_t num_rows = array.size(), num_cols = 0;

    // Make sure the matrix is well-formed
    for (auto&& array_row : array) {
      toml::array* row = array_row.as_array();
      if (!row->is_homogeneous()) {
        throw std::runtime_error("Matrix data must be homogeneous.");
      }
      std::size_t num_cols_row = row->size();
      if (num_cols == 0) {
        num_cols = num_cols_row;
      } else {
        if (num_cols != num_cols_row) {
          throw std::runtime_error(
              "Rows in matrix don't all have the same number of elements.");
        }
      }
    }

    // Now just flatten the data and dump it into a std::vector
    auto data = array.flatten();
    std::vector<DataType> data_vector(num_rows * num_cols);
    for (std::size_t idata = 0; idata < data.size(); ++idata) {
      data_vector[idata] = *data[idata].value<DataType>();
    }
    return Tensor<DataType, 2>(std::move(data_vector), num_rows, num_cols);
  }
};

}  // namespace tyche

#endif /* #ifndef __TYCHE_IO_TOML_READER_HPP */
