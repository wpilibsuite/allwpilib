// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../../ProtoTestBase.hpp"
#include "wpi/math/spline/QuinticHermiteSpline.hpp"

using namespace wpi::math;

struct QuinticHermiteSplineProtoTestData {
  using Type = QuinticHermiteSpline;

  inline static const Type kTestData{
      wpi::util::array<double, 3>{{0.01, 0.02, 0.03}},
      wpi::util::array<double, 3>{{0.04, 0.05, 0.06}},
      wpi::util::array<double, 3>{{0.07, 0.08, 0.09}},
      wpi::util::array<double, 3>{{0.10, 0.11, 0.11}}};

  static void CheckEq(const Type& testData, const Type& data) {
    CHECK(testData.GetInitialControlVector().x ==
          data.GetInitialControlVector().x);
    CHECK(testData.GetFinalControlVector().x == data.GetFinalControlVector().x);
    CHECK(testData.GetInitialControlVector().y ==
          data.GetInitialControlVector().y);
    CHECK(testData.GetFinalControlVector().y == data.GetFinalControlVector().y);
  }
};

INSTANTIATE_TYPED_TEST_SUITE_P(QuinticHermiteSpline, ProtoTest,
                               QuinticHermiteSplineProtoTestData);
