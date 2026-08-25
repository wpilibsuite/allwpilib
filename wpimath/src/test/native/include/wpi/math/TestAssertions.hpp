// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#define CHECK_DOUBLE_EQ(a, b)                                    \
  CHECK_THAT(                                                    \
      static_cast<double>(::wpi::math::test::AssertionValue(a)), \
      Catch::Matchers::WithinULP(                                \
          static_cast<double>(::wpi::math::test::AssertionValue(b)), 4))

#define CHECK_FLOAT_EQ(a, b)                                           \
  CHECK_THAT(static_cast<float>(::wpi::math::test::AssertionValue(a)), \
             Catch::Matchers::WithinULP(                               \
                 static_cast<float>(::wpi::math::test::AssertionValue(b)), 4))

#define CHECK_NEAR(a, b, tolerance)                                  \
  CHECK_THAT(                                                        \
      static_cast<double>(::wpi::math::test::AssertionValue(a)),     \
      Catch::Matchers::WithinAbs(                                    \
          static_cast<double>(::wpi::math::test::AssertionValue(b)), \
          static_cast<double>(::wpi::math::test::AssertionValue(tolerance))))

#define REQUIRE_NEAR(a, b, tolerance)                                \
  REQUIRE_THAT(                                                      \
      static_cast<double>(::wpi::math::test::AssertionValue(a)),     \
      Catch::Matchers::WithinAbs(                                    \
          static_cast<double>(::wpi::math::test::AssertionValue(b)), \
          static_cast<double>(::wpi::math::test::AssertionValue(tolerance))))

#define CHECK_UNITS_EQ(a, b) CHECK(((a) == (b)))

#define CHECK_UNITS_NEAR(a, b, tolerance)                 \
  CHECK_NEAR((::wpi::units::abs((a) - (b))).value(), 0.0, \
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
