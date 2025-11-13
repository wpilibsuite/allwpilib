// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/proto/SimpleMotorFeedforwardProto.hpp"

#include <gtest/gtest.h>

#include "../../ProtoTestBase.hpp"
#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/velocity.hpp"

using namespace wpi::math;

template <typename T>
struct SimpleMotorFeedforwardProtoTestData {
  using Type = SimpleMotorFeedforward<T>;

  inline static const Type kTestData = {
      wpi::units::volt_t{0.4},
      wpi::units::volt_t{4.0} / (wpi::units::unit_t<T>{1} / 1_s),
      wpi::units::volt_t{0.7} / (wpi::units::unit_t<T>{1} / 1_s / 1_s), 25_ms};

  static void CheckEq(const Type& testData, const Type& data) {
    EXPECT_EQ(testData.GetKs().value(), data.GetKs().value());
    EXPECT_EQ(testData.GetKv().value(), data.GetKv().value());
    EXPECT_EQ(testData.GetKa().value(), data.GetKa().value());
    EXPECT_EQ(testData.GetDt().value(), data.GetDt().value());
  }
};

INSTANTIATE_TYPED_TEST_SUITE_P(
    SimpleMotorFeedforwardMeters, ProtoTest,
    SimpleMotorFeedforwardProtoTestData<wpi::units::meters>);
INSTANTIATE_TYPED_TEST_SUITE_P(
    SimpleMotorFeedforwardFeet, ProtoTest,
    SimpleMotorFeedforwardProtoTestData<wpi::units::feet>);
INSTANTIATE_TYPED_TEST_SUITE_P(
    SimpleMotorFeedforwardRadians, ProtoTest,
    SimpleMotorFeedforwardProtoTestData<wpi::units::radians>);
