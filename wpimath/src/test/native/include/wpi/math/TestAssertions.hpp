// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#define CHECK_DOUBLE_EQ(a, b) CHECK((a) == Catch::Approx((b)).epsilon(0x1p-50))

#define CHECK_FLOAT_EQ(a, b) CHECK((a) == Catch::Approx((b)).epsilon(0x1p-21))

#define CHECK_NEAR(a, b, tolerance) \
  CHECK((a) == Catch::Approx((b)).epsilon(0.0).margin(tolerance))

#define REQUIRE_NEAR(a, b, tolerance) \
  REQUIRE((a) == Catch::Approx((b)).epsilon(0.0).margin(tolerance))

#define CHECK_UNITS_EQ(a, b) CHECK_DOUBLE_EQ((a).value(), (b).value())

#define CHECK_UNITS_NEAR(a, b, tolerance) \
  CHECK_NEAR((a).value(), (b).value(), tolerance)
