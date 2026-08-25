// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "../../StructTestBase.hpp"
#include "wpi/math/controller/DifferentialDriveFeedforward.hpp"

using namespace wpi::math;

struct DifferentialDriveFeedforwardStructTestData {
  using Type = DifferentialDriveFeedforward;

  inline static const Type TEST_DATA{
      decltype(1_V / 1_mps){0.174}, decltype(1_V / 1_mps2){0.229},
      decltype(1_V / 1_mps){4.4}, decltype(1_V / 1_mps2){4.5}};

  static void CheckEq(const Type& testData, const Type& data) {
    CHECK(testData.kvLinear.value() == data.kvLinear.value());
    CHECK(testData.kaLinear.value() == data.kaLinear.value());
    CHECK(testData.kvAngular.value() == data.kvAngular.value());
    CHECK(testData.kaAngular.value() == data.kaAngular.value());
  }
};

INSTANTIATE_CATCH_TYPED_TEST_SUITE_P(
    DifferentialDriveFeedforward, StructTest,
    DifferentialDriveFeedforwardStructTestData);
