// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpimath/ComputerVisionUtil.h"

TEST(ComputerVisionUtilTest, ObjectToRobotPose) {
  wpimath::Pose3d robot{1_m, 2_m, 0_m,
                        wpimath::Rotation3d{0_deg, 0_deg, 30_deg}};
  wpimath::Transform3d cameraToObject{
      wpimath::Translation3d{1_m, 1_m, 1_m},
      wpimath::Rotation3d{0_deg, -20_deg, 45_deg}};
  wpimath::Transform3d robotToCamera{wpimath::Translation3d{1_m, 0_m, 2_m},
                                     wpimath::Rotation3d{0_deg, 0_deg, 25_deg}};
  wpimath::Pose3d object = robot + robotToCamera + cameraToObject;

  EXPECT_EQ(robot,
            wpimath::ObjectToRobotPose(object, cameraToObject, robotToCamera));
}
