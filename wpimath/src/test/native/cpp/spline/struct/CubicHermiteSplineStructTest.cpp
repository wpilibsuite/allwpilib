// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../../StructTestBase.hpp"
#include "wpi/math/spline/CubicHermiteSpline.hpp"

using namespace wpi::math;

struct CubicHermiteSplineStructTestData {
  using Type = CubicHermiteSpline;

  inline static const Type kTestData{wpi::util::array<double, 2>{{0.1, 0.2}},
                                     wpi::util::array<double, 2>{{0.3, 0.4}},
                                     wpi::util::array<double, 2>{{0.5, 0.6}},
                                     wpi::util::array<double, 2>{{0.7, 0.8}}};

  static void CheckEq(const Type& testData, const Type& data) {
    CHECK(testData.GetInitialControlVector().x ==
          data.GetInitialControlVector().x);
    CHECK(testData.GetFinalControlVector().x == data.GetFinalControlVector().x);
    CHECK(testData.GetInitialControlVector().y ==
          data.GetInitialControlVector().y);
    CHECK(testData.GetFinalControlVector().y == data.GetFinalControlVector().y);
  }
};

INSTANTIATE_TYPED_TEST_SUITE_P(CubicHermiteSpline, StructTest,
                               CubicHermiteSplineStructTestData);
