// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/UidVector.hpp"

#include <vector>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

namespace wpi::util {

TEST_CASE("UidVectorTest Empty", "[wpiutil]") {
  UidVector<int, 4> v;
  REQUIRE(v.empty());

  v.emplace_back(1);
  REQUIRE_FALSE(v.empty());
}

TEST_CASE("UidVectorTest Erase", "[wpiutil]") {
  UidVector<int, 4> v;
  size_t uid = v.emplace_back(1);
  v.erase(uid);
  REQUIRE(v.empty());
}

TEST_CASE("UidVectorTest Clear", "[wpiutil]") {
  UidVector<int, 4> v;
  v.emplace_back(1);
  v.emplace_back(2);
  v.clear();
  REQUIRE(v.empty());
}

TEST_CASE("UidVectorTest Iterate", "[wpiutil]") {
  UidVector<int, 4> v;
  v.emplace_back(2);
  v.emplace_back(1);
  std::vector<int> out;
  for (auto&& val : v) {
    out.push_back(val);
  }
  REQUIRE((out.size()) == (2u));
  CHECK((out[0]) == (2));
  CHECK((out[1]) == (1));
}

}  // namespace wpi::util
