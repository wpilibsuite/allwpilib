// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/json.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

namespace wpi::util {

TEST_CASE("JsonTest ParseMaxUint64", "[wpiutil]") {
  auto j = json::parse_or_throw("18446744073709551615");
  CHECK(j.is_uint());
  CHECK((j.get_uint()) == (18446744073709551615ull));
}

TEST_CASE("JsonTest MarshalMaxUint64", "[wpiutil]") {
  json j{18446744073709551615ull};
  CHECK((j.to_string()) == ("18446744073709551615"));
}

TEST_CASE("JsonTest AssignBool", "[wpiutil]") {
  json j;
  j = true;
  REQUIRE(j.is_bool());
  CHECK(j.get_bool());
}

TEST_CASE("JsonTest AssignBoolToMap", "[wpiutil]") {
  json j;
  j["key"] = true;
  REQUIRE(j["key"].is_bool());
  CHECK(j["key"].get_bool());
}

TEST_CASE("JsonTest AssignBoolToArray", "[wpiutil]") {
  json j;
  j.emplace_back(true);
  REQUIRE(j[0].is_bool());
  CHECK(j[0].get_bool());
}

TEST_CASE("JsonTest BoolObject", "[wpiutil]") {
  json j = json::object("key", true);
  REQUIRE(j["key"].is_bool());
  CHECK(j["key"].get_bool());
}

TEST_CASE("JsonTest BoolArray", "[wpiutil]") {
  json j = json::array(true, false, true);
  REQUIRE(j[0].is_bool());
  CHECK(j[0].get_bool());
  REQUIRE(j[1].is_bool());
  CHECK_FALSE(j[1].get_bool());
  REQUIRE(j[2].is_bool());
  CHECK(j[2].get_bool());
}

}  // namespace wpi::util
