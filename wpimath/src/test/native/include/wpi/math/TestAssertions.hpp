// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/units/math.hpp"

#define CHECK_DOUBLE_EQ(a, b) CHECK((a) == Catch::Approx((b)).epsilon(0x1p-50))

#define CHECK_FLOAT_EQ(a, b) CHECK((a) == Catch::Approx((b)).epsilon(0x1p-21))

#define CHECK_NEAR(a, b, tolerance) \
  CHECK((a) == Catch::Approx((b)).epsilon(0.0).margin(tolerance))

#define REQUIRE_NEAR(a, b, tolerance) \
  REQUIRE((a) == Catch::Approx((b)).epsilon(0.0).margin(tolerance))

#define CHECK_UNITS_EQ(a, b) CHECK(((a) == (b)))

#define CHECK_UNITS_NEAR(a, b, tolerance)                       \
  CHECK_NEAR((::wpi::units::math::abs((a) - (b))).value(), 0.0, \
             ::wpi::math::test::AssertionValue(tolerance))

namespace wpi::math::test {

template <typename T>
auto AssertionValue(const T& value) {
  if constexpr (requires { value.value(); }) {
    return value.value();
  } else {
    return value;
  }
}

}  // namespace wpi::math::test

#define CHECK_EIGEN_EQ(a, b)                                                   \
  do {                                                                         \
    const auto expected = (a);                                                 \
    const auto actual = (b);                                                   \
    REQUIRE(expected.rows() == actual.rows());                                 \
    REQUIRE(expected.cols() == actual.cols());                                 \
    for (int row = 0; row < expected.rows(); ++row) {                          \
      for (int col = 0; col < expected.cols(); ++col) {                        \
        CHECK_DOUBLE_EQ(::wpi::math::test::AssertionValue(expected(row, col)), \
                        ::wpi::math::test::AssertionValue(actual(row, col)));  \
      }                                                                        \
    }                                                                          \
  } while (false)
