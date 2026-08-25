// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/proto/SimpleMotorFeedforwardProto.hpp"

#include <catch2/catch_test_macros.hpp>

#include "../../ProtoTestBase.hpp"
#include "wpi/math/controller/SimpleMotorFeedforward.hpp"

using namespace wpi::math;

template <typename T>
struct SimpleMotorFeedforwardProtoTestData {
  using Type = SimpleMotorFeedforward<T>;

  inline static const Type TEST_DATA = {
      wpi::units::volts<>{0.4},
      wpi::units::volts<>{4.0} / (wpi::units::unit<T>{1} / 1_s),
      wpi::units::volts<>{0.7} / (wpi::units::unit<T>{1} / 1_s / 1_s), 25_ms};

  static void CheckEq(const Type& testData, const Type& data) {
    CHECK(testData.GetKs().value() == data.GetKs().value());
    CHECK(testData.GetKv().value() == data.GetKv().value());
    CHECK(testData.GetKa().value() == data.GetKa().value());
    CHECK(testData.GetDt().value() == data.GetDt().value());
  }
};

INSTANTIATE_CATCH_TYPED_TEST_SUITE_P(
    SimpleMotorFeedforwardMeters, ProtoTest,
    SimpleMotorFeedforwardProtoTestData<wpi::units::meters_>);
INSTANTIATE_CATCH_TYPED_TEST_SUITE_P(
    SimpleMotorFeedforwardFeet, ProtoTest,
    SimpleMotorFeedforwardProtoTestData<wpi::units::feet_>);
INSTANTIATE_CATCH_TYPED_TEST_SUITE_P(
    SimpleMotorFeedforwardRadians, ProtoTest,
    SimpleMotorFeedforwardProtoTestData<wpi::units::radians_>);
