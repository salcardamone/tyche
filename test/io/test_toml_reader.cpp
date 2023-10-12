/**
 * @brief
 */
// C++ Standard Libraries
//
// Third-Party Libraries
#include <gtest/gtest.h>
#include <toml++/toml.h>
// Project Inclusions
#include "tyche/io/toml_reader.hpp"
#include "tyche/util/tensor.hpp"

using namespace tyche;
using namespace std::string_view_literals;

/**
 * @brief Container for the data we're to parse.
 */
class TOMLData {
 public:
  Tensor<double, 2> matrix;
};

/**
 * @brief Dummy TOML reader implementation to provide a concrete parse().
 */
class TOMLReaderImpl : public TOMLReader<TOMLData> {
 public:
  /**
   * @brief Parse TOML table into data.
   * @param config The TOML table to parse.
   * @return Container with all parsed data from the table.
   */
  TOMLData parse(toml::table& config) {
    TOMLData data;
    data.matrix = parse_matrix<double>(*config["matrix"].as_array());
    return data;
  }
};

/**
 * @brief Test fixture for TOMLReader testing.
 */
class TestTOMLReader : public ::testing::Test {
 public:
  /**
   * @brief Parse data from TOML into TOMLData container.
   */
  void SetUp() override {
    toml::table config = toml::parse(toml_data);
    TOMLReaderImpl reader;
    data = reader.parse(config);
  }

  TOMLData data;

 private:
  static constexpr std::string_view toml_data = R"(
  matrix = [
      [0.0, 1.0, 2.0, 3.0],
      [5.0, 6.0, 7.0, 8.0]
  ]
  )"sv;
};

/**
 * @brief Verify that we can parse a matrix from the TOML.
 */
TEST_F(TestTOMLReader, MatrixParsing) {
  ASSERT_EQ(data.matrix(0, 0), 0);
  ASSERT_EQ(data.matrix(0, 1), 1);
  ASSERT_EQ(data.matrix(0, 2), 2);
  ASSERT_EQ(data.matrix(0, 3), 3);
  ASSERT_EQ(data.matrix(1, 0), 5);
  ASSERT_EQ(data.matrix(1, 1), 6);
  ASSERT_EQ(data.matrix(1, 2), 7);
  ASSERT_EQ(data.matrix(1, 3), 8);
}
