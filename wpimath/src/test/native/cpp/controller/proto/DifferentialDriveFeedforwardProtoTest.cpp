// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../../ProtoTestBase.hpp"
#include "wpi/math/controller/DifferentialDriveFeedforward.hpp"

using namespace wpi::math;

struct DifferentialDriveFeedforwardProtoTestData {
  using Type = DifferentialDriveFeedforward;

  inline static const Type kTestData{
      decltype(1_V / 1_mps){0.174}, decltype(1_V / 1_mps_sq){0.229},
      decltype(1_V / 1_mps){4.4}, decltype(1_V / 1_mps_sq){4.5}};

  static void CheckEq(const Type& testData, const Type& data) {
    CHECK(testData.kVLinear.value() == data.kVLinear.value());
    CHECK(testData.kALinear.value() == data.kALinear.value());
    CHECK(testData.kVAngular.value() == data.kVAngular.value());
    CHECK(testData.kAAngular.value() == data.kAAngular.value());
  }
};

INSTANTIATE_TYPED_TEST_SUITE_P(DifferentialDriveFeedforward, ProtoTest,
                               DifferentialDriveFeedforwardProtoTestData);
