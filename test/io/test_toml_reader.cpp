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
  std::vector<std::any> strings, array_of_tables;
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
    data.strings =
        std::any_cast<std::vector<std::any>>(parse_table(config)["strings"]);
    data.array_of_tables = std::any_cast<std::vector<std::any>>(
        parse_table(config)["array_of_tables"]);
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
  strings = [
      'foo', 'bar', 'baz'
  ]
  array_of_tables = [
      { a = 'hello', b = 1 },
      { c = 'world', d = 2.0 }
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

/**
 * @brief Verify that we can parse an array of strings from the TOML.
 */
TEST_F(TestTOMLReader, ArrayParsing) {
  ASSERT_EQ(data.strings.size(), 3);
  ASSERT_EQ(std::any_cast<std::string>(data.strings[0]), "foo");
  ASSERT_EQ(std::any_cast<std::string>(data.strings[1]), "bar");
  ASSERT_EQ(std::any_cast<std::string>(data.strings[2]), "baz");
}

/**
 * @brief Verify that we can parse an array of tables from the TOML.
 */
TEST_F(TestTOMLReader, ArrayOfTablesParsing) {
  auto table_a =
      std::any_cast<std::map<std::string, std::any>>(data.array_of_tables[0]);
  auto table_b =
      std::any_cast<std::map<std::string, std::any>>(data.array_of_tables[1]);
  ASSERT_EQ(std::any_cast<std::string>(table_a["a"]), "hello");
  ASSERT_EQ(std::any_cast<double>(table_a["b"]), 1);
  ASSERT_EQ(std::any_cast<std::string>(table_b["c"]), "world");
  ASSERT_EQ(std::any_cast<double>(table_b["d"]), 2.0);
}
