// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/json.hpp"

#include <gtest/gtest.h>

namespace wpi::util {

TEST(JsonTest, ParseMaxUint64) {
  auto j = json::parse_or_throw("18446744073709551615");
  EXPECT_TRUE(j.is_uint());
  EXPECT_EQ(j.get_uint(), 18446744073709551615ull);
}

TEST(JsonTest, MarshalMaxUint64) {
  json j{18446744073709551615ull};
  EXPECT_EQ(j.to_string(), "18446744073709551615");
}

TEST(JsonTest, AssignBool) {
  json j;
  j = true;
  ASSERT_TRUE(j.is_bool());
  EXPECT_TRUE(j.get_bool());
}

TEST(JsonTest, AssignBoolToMap) {
  json j;
  j["key"] = true;
  ASSERT_TRUE(j["key"].is_bool());
  EXPECT_TRUE(j["key"].get_bool());
}

TEST(JsonTest, AssignBoolToArray) {
  json j;
  j.emplace_back(true);
  ASSERT_TRUE(j[0].is_bool());
  EXPECT_TRUE(j[0].get_bool());
}

TEST(JsonTest, BoolObject) {
  json j = json::object("key", true);
  ASSERT_TRUE(j["key"].is_bool());
  EXPECT_TRUE(j["key"].get_bool());
}

TEST(JsonTest, BoolArray) {
  json j = json::array(true, false, true);
  ASSERT_TRUE(j[0].is_bool());
  EXPECT_TRUE(j[0].get_bool());
  ASSERT_TRUE(j[1].is_bool());
  EXPECT_FALSE(j[1].get_bool());
  ASSERT_TRUE(j[2].is_bool());
  EXPECT_TRUE(j[2].get_bool());
}

}  // namespace wpi::util
