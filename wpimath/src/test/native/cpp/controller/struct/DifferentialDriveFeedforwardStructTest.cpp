// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "../../StructTestBase.hpp"
#include "wpi/math/controller/DifferentialDriveFeedforward.hpp"

using namespace wpi::math;

struct DifferentialDriveFeedforwardStructTestData {
  using Type = DifferentialDriveFeedforward;

  inline static const Type kTestData{
      decltype(1_V / 1_mps){0.174}, decltype(1_V / 1_mps_sq){0.229},
      decltype(1_V / 1_mps){4.4}, decltype(1_V / 1_mps_sq){4.5}};

  static void CheckEq(const Type& testData, const Type& data) {
    EXPECT_EQ(testData.kVLinear.value(), data.kVLinear.value());
    EXPECT_EQ(testData.kALinear.value(), data.kALinear.value());
    EXPECT_EQ(testData.kVAngular.value(), data.kVAngular.value());
    EXPECT_EQ(testData.kAAngular.value(), data.kAAngular.value());
  }
};

INSTANTIATE_TYPED_TEST_SUITE_P(DifferentialDriveFeedforward, StructTest,
                               DifferentialDriveFeedforwardStructTestData);
