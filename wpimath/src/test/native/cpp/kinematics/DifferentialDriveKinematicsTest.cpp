// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"

#include <numbers>

#include <gtest/gtest.h>

#include "wpi/math/kinematics/ChassisSpeeds.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/velocity.hpp"

using namespace wpi::math;

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
      0.0_mps, 0.0_mps, wpi::units::radians_per_second_t{std::numbers::pi}};
  const auto wheelSpeeds = kinematics.ToWheelSpeeds(chassisSpeeds);

  EXPECT_NEAR(wheelSpeeds.left.value(), -0.381 * std::numbers::pi, kEpsilon);
  EXPECT_NEAR(wheelSpeeds.right.value(), +0.381 * std::numbers::pi, kEpsilon);
}

TEST(DifferentialDriveKinematicsTest, ForwardKinematicsForRotateInPlace) {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const DifferentialDriveWheelSpeeds wheelSpeeds{
      wpi::units::meters_per_second_t{+0.381 * std::numbers::pi},
      wpi::units::meters_per_second_t{-0.381 * std::numbers::pi}};
  const auto chassisSpeeds = kinematics.ToChassisSpeeds(wheelSpeeds);

  EXPECT_NEAR(chassisSpeeds.vx.value(), 0, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.vy.value(), 0, kEpsilon);
  EXPECT_NEAR(chassisSpeeds.omega.value(), -std::numbers::pi, kEpsilon);
}

TEST(DifferentialDriveKinematicsTest, InverseAccelerationsForZeros) {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const ChassisAccelerations chassisAccelerations;
  const auto wheelAccelerations =
      kinematics.ToWheelAccelerations(chassisAccelerations);

  EXPECT_NEAR(wheelAccelerations.left.value(), 0, kEpsilon);
  EXPECT_NEAR(wheelAccelerations.right.value(), 0, kEpsilon);
}

TEST(DifferentialDriveKinematicsTest, ForwardAccelerationsForZeros) {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const DifferentialDriveWheelAccelerations wheelAccelerations;
  const auto chassisAccelerations =
      kinematics.ToChassisAccelerations(wheelAccelerations);

  EXPECT_NEAR(chassisAccelerations.ax.value(), 0, kEpsilon);
  EXPECT_NEAR(chassisAccelerations.ay.value(), 0, kEpsilon);
  EXPECT_NEAR(chassisAccelerations.alpha.value(), 0, kEpsilon);
}

TEST(DifferentialDriveKinematicsTest, InverseAccelerationsForStraightLine) {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const ChassisAccelerations chassisAccelerations{3.0_mps_sq, 0_mps_sq,
                                                  0_rad_per_s_sq};
  const auto wheelAccelerations =
      kinematics.ToWheelAccelerations(chassisAccelerations);

  EXPECT_NEAR(wheelAccelerations.left.value(), 3, kEpsilon);
  EXPECT_NEAR(wheelAccelerations.right.value(), 3, kEpsilon);
}

TEST(DifferentialDriveKinematicsTest, ForwardAccelerationsForStraightLine) {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const DifferentialDriveWheelAccelerations wheelAccelerations{3.0_mps_sq,
                                                               3.0_mps_sq};
  const auto chassisAccelerations =
      kinematics.ToChassisAccelerations(wheelAccelerations);

  EXPECT_NEAR(chassisAccelerations.ax.value(), 3, kEpsilon);
  EXPECT_NEAR(chassisAccelerations.ay.value(), 0, kEpsilon);
  EXPECT_NEAR(chassisAccelerations.alpha.value(), 0, kEpsilon);
}

TEST(DifferentialDriveKinematicsTest, InverseAccelerationsForRotateInPlace) {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const ChassisAccelerations chassisAccelerations{
      0.0_mps_sq, 0.0_mps_sq,
      units::radians_per_second_squared_t{std::numbers::pi}};
  const auto wheelAccelerations =
      kinematics.ToWheelAccelerations(chassisAccelerations);

  EXPECT_NEAR(wheelAccelerations.left.value(), -0.381 * std::numbers::pi,
              kEpsilon);
  EXPECT_NEAR(wheelAccelerations.right.value(), +0.381 * std::numbers::pi,
              kEpsilon);
}

TEST(DifferentialDriveKinematicsTest, ForwardAccelerationsForRotateInPlace) {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const DifferentialDriveWheelAccelerations wheelAccelerations{
      units::meters_per_second_squared_t{+0.381 * std::numbers::pi},
      units::meters_per_second_squared_t{-0.381 * std::numbers::pi}};
  const auto chassisAccelerations =
      kinematics.ToChassisAccelerations(wheelAccelerations);

  EXPECT_NEAR(chassisAccelerations.ax.value(), 0, kEpsilon);
  EXPECT_NEAR(chassisAccelerations.ay.value(), 0, kEpsilon);
  EXPECT_NEAR(chassisAccelerations.alpha.value(), -std::numbers::pi, kEpsilon);
}
