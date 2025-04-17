// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpimath/src/test/native/cpp/ProtoTestBase.h"
#include "frc/controller/ElevatorFeedforward.h"
#include "frc/controller/proto/ElevatorFeedforwardProto.h"
#include "units/acceleration.h"
#include "units/velocity.h"

using namespace frc;

template <typename I>
struct ElevatorFeedforwardProtoTestData {
  using Type = ElevatorFeedforward<I>;

  inline static const Type kTestData = {
      units::unit_t<I>{1.91}, units::unit_t<I>{2.29},
      units::unit_t<I>{35.04} / (units::meter_t{1} / 1_s),
      units::unit_t<I>{1.74} / (units::meter_t{1} / 1_s / 1_s), 25_ms};

  static void CheckEq(const Type& testData, const Type& data) {
    EXPECT_EQ(testData.GetKs().value(), data.GetKs().value());
    EXPECT_EQ(testData.GetKg().value(), data.GetKg().value());
    EXPECT_EQ(testData.GetKv().value(), data.GetKv().value());
    EXPECT_EQ(testData.GetKa().value(), data.GetKa().value());
    EXPECT_EQ(testData.GetDt().value(), data.GetDt().value());
  }
};

using ElevatorFeedforwardProtoTestTypes =
    ::testing::Types<ElevatorFeedforwardProtoTestData<units::volt>,
                     ElevatorFeedforwardProtoTestData<units::ampere>>;

INSTANTIATE_TYPED_TEST_SUITE_P(ElevatorFeedforward, ProtoTest,
                               ElevatorFeedforwardProtoTestTypes);
