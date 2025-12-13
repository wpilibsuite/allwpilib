// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/util/ComputerVisionUtil.hpp"

#include <gtest/gtest.h>

TEST(ComputerVisionUtilTest, ObjectToRobotPose) {
  wpi::math::Pose3d robot{1_m, 2_m, 0_m,
                          wpi::math::Rotation3d{0_deg, 0_deg, 30_deg}};
  wpi::math::Transform3d cameraToObject{
      wpi::math::Translation3d{1_m, 1_m, 1_m},
      wpi::math::Rotation3d{0_deg, -20_deg, 45_deg}};
  wpi::math::Transform3d robotToCamera{
      wpi::math::Translation3d{1_m, 0_m, 2_m},
      wpi::math::Rotation3d{0_deg, 0_deg, 25_deg}};
  wpi::math::Pose3d object = robot + robotToCamera + cameraToObject;

  EXPECT_EQ(robot, wpi::math::ObjectToRobotPose(object, cameraToObject,
                                                robotToCamera));
}
