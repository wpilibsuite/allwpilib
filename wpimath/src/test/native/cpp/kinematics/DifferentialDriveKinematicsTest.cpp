// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include "frc/kinematics/ChassisSpeeds.h"
#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "gtest/gtest.h"
#include "units/angular_velocity.h"
#include "units/length.h"
#include "units/velocity.h"

using namespace frc;

static constexpr double kEpsilon = 1E-9;

TEST(DifferentialDriveKinematicsTest, InverseKinematicsFromZero) {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const ChassisSpeeds chassisSpeeds;
  const auto wheelSpeeds = kinematics.ToWheelSpeeds(chassisSpeeds);

  EXPECT_NEAR(wheelSpeeds.left.value(), 0, kEpsilon);
  EXPECT_NEAR(wheelSpeeds.right.value(), 0, kEpsilon);
}

TEST(DifferentialDriveKinematicsTest, ForwardKinematicsFromZero) {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const DifferentialDriveWheelSpeeds wheelSpeeds;
  const auto chassisSpeeds = kinematics.ToChassisSpeeds(wheelSpeeds);

  EXPECT_NEAR(chassisSpeeds.vx.value(), 0, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.vy.value(), 0, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.omega.value(), 0, kEpsilon);
}

TEST(DifferentialDriveKinematicsTest, InverseKinematicsForStraightLine) {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const ChassisSpeeds chassisSpeeds{3.0_mps, 0_mps, 0_rad_per_s};
  const auto wheelSpeeds = kinematics.ToWheelSpeeds(chassisSpeeds);

  EXPECT_NEAR(wheelSpeeds.left.value(), 3, kEpsilon);
  EXPECT_NEAR(wheelSpeeds.right.value(), 3, kEpsilon);
}

TEST(DifferentialDriveKinematicsTest, ForwardKinematicsForStraightLine) {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const DifferentialDriveWheelSpeeds wheelSpeeds{3.0_mps, 3.0_mps};
  const auto chassisSpeeds = kinematics.ToChassisSpeeds(wheelSpeeds);

  EXPECT_NEAR(chassisSpeeds.vx.value(), 3, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.vy.value(), 0, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.omega.value(), 0, kEpsilon);
}

TEST(DifferentialDriveKinematicsTest, InverseKinematicsForRotateInPlace) {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const ChassisSpeeds chassisSpeeds{
      0.0_mps, 0.0_mps, units::radians_per_second_t{std::numbers::pi}};
  const auto wheelSpeeds = kinematics.ToWheelSpeeds(chassisSpeeds);

  EXPECT_NEAR(wheelSpeeds.left.value(), -0.381 * std::numbers::pi, kEpsilon);
  EXPECT_NEAR(wheelSpeeds.right.value(), +0.381 * std::numbers::pi, kEpsilon);
}

TEST(DifferentialDriveKinematicsTest, ForwardKinematicsForRotateInPlace) {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const DifferentialDriveWheelSpeeds wheelSpeeds{
      units::meters_per_second_t{+0.381 * std::numbers::pi},
      units::meters_per_second_t{-0.381 * std::numbers::pi}};
  const auto chassisSpeeds = kinematics.ToChassisSpeeds(wheelSpeeds);

  EXPECT_NEAR(chassisSpeeds.vx.value(), 0, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.vy.value(), 0, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.omega.value(), -std::numbers::pi, kEpsilon);
}
