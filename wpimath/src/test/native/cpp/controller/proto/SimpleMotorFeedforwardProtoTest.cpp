// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "../../ProtoTestBase.h"
#include "frc/controller/SimpleMotorFeedforward.h"
#include "frc/controller/proto/SimpleMotorFeedforwardProto.h"
#include "units/acceleration.h"
#include "units/velocity.h"

using namespace frc;

struct SimpleMotorFeedforwardProtoTestData {
  using Type = SimpleMotorFeedforward<units::meters>;

  inline static const Type kTestData = {units::volt_t{0.4},
                                        units::volt_t{4.0} / 1_mps,
                                        units::volt_t{0.7} / 1_mps_sq, 25_ms};

  static void CheckEq(const Type& testData, const Type& data) {
    EXPECT_EQ(testData.GetKs().value(), data.GetKs().value());
    EXPECT_EQ(testData.GetKv().value(), data.GetKv().value());
    EXPECT_EQ(testData.GetKa().value(), data.GetKa().value());
    EXPECT_EQ(testData.GetDt().value(), data.GetDt().value());
  }
};

INSTANTIATE_TYPED_TEST_SUITE_P(SimpleMotorFeedforwardMeters, ProtoTest,
                               SimpleMotorFeedforwardProtoTestData);
