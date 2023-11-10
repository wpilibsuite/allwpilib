// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "../../StructTestBase.h"
#include "frc/spline/CubicHermiteSpline.h"

using namespace frc;

struct CubicHermiteSplineStructTestData {
  using Type = CubicHermiteSpline;

  inline static const Type kTestData{
      wpi::array<double, 2>{{0.1, 0.2}}, wpi::array<double, 2>{{0.3, 0.4}},
      wpi::array<double, 2>{{0.5, 0.6}}, wpi::array<double, 2>{{0.7, 0.8}}};

  static void CheckEq(const Type& testData, const Type& data) {
    EXPECT_EQ(testData.xInitialControlVector, data.xInitialControlVector);
    EXPECT_EQ(testData.xFinalControlVector, data.xFinalControlVector);
    EXPECT_EQ(testData.yInitialControlVector, data.yInitialControlVector);
    EXPECT_EQ(testData.yFinalControlVector, data.yFinalControlVector);
  }
};

INSTANTIATE_TYPED_TEST_SUITE_P(CubicHermiteSpline, StructTest,
                               CubicHermiteSplineStructTestData);
