// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "../../ProtoTestBase.h"
#include "frc/controller/DifferentialDriveFeedforward.h"

using namespace frc;

struct DifferentialDriveFeedforwardProtoTestData {
  using Type = DifferentialDriveFeedforward;

  inline static const Type kTestData{
      decltype(1_V / 1_mps){0.174}, decltype(1_V / 1_mps_sq){0.229},
      decltype(1_V / 1_mps){4.4}, decltype(1_V / 1_mps_sq){4.5}};

  static void CheckEq(const Type& testData, const Type& data) {
    EXPECT_EQ(testData.m_kVLinear.value(), data.m_kVLinear.value());
    EXPECT_EQ(testData.m_kALinear.value(), data.m_kALinear.value());
    EXPECT_EQ(testData.m_kVAngular.value(), data.m_kVAngular.value());
    EXPECT_EQ(testData.m_kAAngular.value(), data.m_kAAngular.value());
  }
};

INSTANTIATE_TYPED_TEST_SUITE_P(DifferentialDriveFeedforward, ProtoTest,
                               DifferentialDriveFeedforwardProtoTestData);
