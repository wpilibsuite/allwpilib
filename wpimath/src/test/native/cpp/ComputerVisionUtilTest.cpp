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

TEST(ComputerVisionUtilTest, Distance) {
  auto camHeight = 1_m;
  auto targetHeight = 3_m;
  auto camPitch = 0_deg;
  auto targetPitch = 30_deg;

  auto dist = frc::CalculateDistanceToTarget(camHeight, targetHeight, camPitch,
                                             targetPitch);

  EXPECT_NEAR(3.464, dist.value(), 0.01);

  camHeight = 1_m;
  targetHeight = 2_m;
  camPitch = 20_deg;
  targetPitch = -10_deg;

  dist = frc::CalculateDistanceToTarget(camHeight, targetHeight, camPitch,
                                        targetPitch);
  EXPECT_NEAR(5.671, dist.value(), 0.01);

  camHeight = 3_m;
  targetHeight = 1_m;
  camPitch = 0_deg;
  targetPitch = -30_deg;

  dist = frc::CalculateDistanceToTarget(camHeight, targetHeight, camPitch,
                                        targetPitch);

  EXPECT_NEAR(3.464, dist.value(), 0.01);
}

TEST(ComputerVisionUtilTest, Transform) {
  auto camHeight = 1_m;
  auto targetHeight = 3_m;
  auto camPitch = 0_deg;
  auto targetPitch = 30_deg;
  frc::Rotation2d targetYaw;
  frc::Rotation2d gyroAngle;
  frc::Pose2d fieldToTarget;
  frc::Transform2d cameraToRobot;

  auto fieldToRobot = frc::EstimateFieldToRobot(
      frc::EstimateCameraToTarget(
          frc::Translation2d{
              frc::CalculateDistanceToTarget(camHeight, targetHeight, camPitch,
                                             targetPitch),
              targetYaw},
          fieldToTarget, gyroAngle),
      fieldToTarget, cameraToRobot);

  EXPECT_NEAR(-3.464, fieldToRobot.X().value(), 0.1);
  EXPECT_NEAR(0, fieldToRobot.Y().value(), 0.1);
  EXPECT_NEAR(0, fieldToRobot.Rotation().Degrees().value(), 0.1);
}
