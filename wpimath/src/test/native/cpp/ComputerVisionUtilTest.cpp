// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/ComputerVisionUtil.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Transform2d.h"
#include "gtest/gtest.h"
#include "units/angle.h"
#include "units/length.h"

TEST(ComputerVisionUtilTest, CalculateDistanceToTarget) {
  auto cameraHeight = 1_m;
  auto targetHeight = 3_m;
  auto cameraPitch = 0_deg;
  auto targetPitch = 30_deg;
  auto targetYaw = 0_deg;

  auto distanceAlongGround = frc::CalculateDistanceToTarget(
      cameraHeight, targetHeight, cameraPitch, targetPitch, targetYaw);
  EXPECT_NEAR(3.464, distanceAlongGround.value(), 0.01);

  cameraHeight = 1_m;
  targetHeight = 2_m;
  cameraPitch = 20_deg;
  targetPitch = -10_deg;

  distanceAlongGround = frc::CalculateDistanceToTarget(
      cameraHeight, targetHeight, cameraPitch, targetPitch, targetYaw);
  EXPECT_NEAR(5.671, distanceAlongGround.value(), 0.01);

  cameraHeight = 3_m;
  targetHeight = 1_m;
  cameraPitch = 0_deg;
  targetPitch = -30_deg;

  distanceAlongGround = frc::CalculateDistanceToTarget(
      cameraHeight, targetHeight, cameraPitch, targetPitch, targetYaw);
  EXPECT_NEAR(3.464, distanceAlongGround.value(), 0.01);

  cameraHeight = 1_m;
  targetHeight = 3_m;
  cameraPitch = 0_deg;
  targetPitch = 30_deg;
  targetYaw = 30_deg;

  distanceAlongGround = frc::CalculateDistanceToTarget(
      cameraHeight, targetHeight, cameraPitch, targetPitch, targetYaw);
  EXPECT_NEAR(4, distanceAlongGround.value(), 0.01);
}

TEST(ComputerVisionUtilTest, EstimateFieldToRobot) {
  auto cameraHeight = 1_m;
  auto targetHeight = 3_m;
  auto cameraPitch = 0_deg;
  auto targetPitch = 30_deg;
  frc::Rotation2d targetYaw;
  frc::Rotation2d gyroAngle;
  frc::Pose3d fieldToTarget;
  frc::Transform3d cameraToRobot;

  auto distanceAlongGround =
      frc::CalculateDistanceToTarget(cameraHeight, targetHeight, cameraPitch,
                                     targetPitch, targetYaw.Radians());
  auto range =
      units::math::hypot(distanceAlongGround, targetHeight - cameraHeight);
  auto fieldToRobot = frc::EstimateFieldToRobot(
      frc::EstimateCameraToTarget(
          frc::Translation3d{
              range, frc::Rotation3d{0_rad, targetPitch, targetYaw.Radians()}},
          fieldToTarget, gyroAngle),
      fieldToTarget, cameraToRobot);

  EXPECT_NEAR(-3.464, fieldToRobot.X().value(), 0.1);
  EXPECT_NEAR(0, fieldToRobot.Y().value(), 0.1);
  EXPECT_NEAR(2.0, fieldToRobot.Z().value(), 0.1);
  EXPECT_NEAR(0, fieldToRobot.Rotation().Z().value(), 0.1);

  gyroAngle = -30_deg;

  distanceAlongGround =
      frc::CalculateDistanceToTarget(cameraHeight, targetHeight, cameraPitch,
                                     targetPitch, targetYaw.Radians());
  range = units::math::hypot(distanceAlongGround, targetHeight - cameraHeight);
  fieldToRobot = frc::EstimateFieldToRobot(
      frc::EstimateCameraToTarget(
          frc::Translation3d{
              range, frc::Rotation3d{0_rad, targetPitch, targetYaw.Radians()}},
          fieldToTarget, gyroAngle),
      fieldToTarget, cameraToRobot);

  EXPECT_NEAR(-3.0, fieldToRobot.X().value(), 0.1);
  EXPECT_NEAR(1.732, fieldToRobot.Y().value(), 0.1);
  EXPECT_NEAR(2.0, fieldToRobot.Z().value(), 0.1);
  EXPECT_NEAR(-30.0, units::degree_t{fieldToRobot.Rotation().Z()}.value(), 0.1);
}
