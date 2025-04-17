// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "ProtoTestBase.h"
#include "frc/controller/SimpleMotorFeedforward.h"
#include "frc/controller/proto/SimpleMotorFeedforwardProto.h"
#include "units/acceleration.h"
#include "units/velocity.h"

using namespace frc;

template <typename T, typename I>
struct SimpleMotorFeedforwardProtoTestData {
  using Type = SimpleMotorFeedforward<T, I>;

  inline static const Type kTestData = {
      units::unit_t<I>{0.4},
      units::unit_t<I>{4.0} / (units::unit_t<T>{1} / 1_s),
      units::unit_t<I>{0.7} / (units::unit_t<T>{1} / 1_s / 1_s), 25_ms};

  static void CheckEq(const Type& testData, const Type& data) {
    EXPECT_EQ(testData.GetKs().value(), data.GetKs().value());
    EXPECT_EQ(testData.GetKv().value(), data.GetKv().value());
    EXPECT_EQ(testData.GetKa().value(), data.GetKa().value());
    EXPECT_EQ(testData.GetDt().value(), data.GetDt().value());
  }
};

using SimpleMotorFeedforwardProtoTestTypes = ::testing::Types<
    SimpleMotorFeedforwardProtoTestData<units::meters, units::volts>,
    SimpleMotorFeedforwardProtoTestData<units::feet, units::volts>,
    SimpleMotorFeedforwardProtoTestData<units::radians, units::volts>>;

INSTANTIATE_TYPED_TEST_SUITE_P(SimpleMotorFeedforward, ProtoTest,
                               SimpleMotorFeedforwardProtoTestTypes);
