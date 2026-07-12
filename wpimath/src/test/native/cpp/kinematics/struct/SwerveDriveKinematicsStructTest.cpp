// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../../StructTestBase.hpp"
#include "wpi/math/kinematics/SwerveDriveKinematics.hpp"

using namespace wpi::math;

struct SwerveDriveKinematicsStructTestData {
  using Type = SwerveDriveKinematics<4>;

  inline static const Type kTestData{wpi::math::Translation2d{1.0_m, 0.9_m},
                                     wpi::math::Translation2d{1.1_m, -0.8_m},
                                     wpi::math::Translation2d{-1.2_m, 0.7_m},
                                     wpi::math::Translation2d{-1.3_m, -0.6_m}};

  static void CheckEq(const Type& testData, const Type& data) {
    CHECK(testData.GetModules() == data.GetModules());
  }
};

INSTANTIATE_TYPED_TEST_SUITE_P(SwerveDriveKinematics, StructTest,
                               SwerveDriveKinematicsStructTestData);
