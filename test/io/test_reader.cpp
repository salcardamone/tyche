/**
 * @brief
 */
// C++ Standard Libraries
#include <map>
#include <any>
#include <iostream>
// Third-Party Libraries
#include <gtest/gtest.h>
// Project Inclusions
#include "tyche/io/reader.hpp"

using namespace tyche;

/**
 * @brief Verify that we can strip prefixes from keys in a map.
 */
TEST(TestReader, MapKeyPrefixRemoval) {
  Reader::Mapping map{{"foo", 1}, {"foo.bar", 2}, {"foo.bar.baz", 3}};
  Reader::Mapping submap = Reader::remove_prefix(map, "foo.");
  ASSERT_EQ(submap.size(), 2);
  ASSERT_EQ(std::any_cast<int>(submap["bar"]), 2);
  ASSERT_EQ(std::any_cast<int>(submap["bar.baz"]), 3);
}
