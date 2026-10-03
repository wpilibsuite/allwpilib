// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// Floating-point assertions mirroring the semantics of the GoogleTest macros
// these tests were originally written against: *_EQ compares within 4 ULP and
// *_NEAR compares within an absolute tolerance.

#define CHECK_DOUBLE_EQ(a, b)        \
  CHECK_THAT(static_cast<double>(a), \
             Catch::Matchers::WithinULP(static_cast<double>(b), 4))

#define REQUIRE_DOUBLE_EQ(a, b)        \
  REQUIRE_THAT(static_cast<double>(a), \
               Catch::Matchers::WithinULP(static_cast<double>(b), 4))

#define CHECK_FLOAT_EQ(a, b)        \
  CHECK_THAT(static_cast<float>(a), \
             Catch::Matchers::WithinULP(static_cast<float>(b), 4))

#define CHECK_NEAR(a, b, tolerance)                             \
  CHECK_THAT(static_cast<double>(a),                            \
             Catch::Matchers::WithinAbs(static_cast<double>(b), \
                                        static_cast<double>(tolerance)))

#define REQUIRE_NEAR(a, b, tolerance)                             \
  REQUIRE_THAT(static_cast<double>(a),                            \
               Catch::Matchers::WithinAbs(static_cast<double>(b), \
                                          static_cast<double>(tolerance)))
