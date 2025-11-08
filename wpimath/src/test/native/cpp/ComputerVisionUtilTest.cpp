// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/util/ComputerVisionUtil.hpp"

TEST(ComputerVisionUtilTest, ObjectToRobotPose) {
  frc::Pose3d robot{1_m, 2_m, 0_m, frc::Rotation3d{0_deg, 0_deg, 30_deg}};
  frc::Transform3d cameraToObject{frc::Translation3d{1_m, 1_m, 1_m},
                                  frc::Rotation3d{0_deg, -20_deg, 45_deg}};
  frc::Transform3d robotToCamera{frc::Translation3d{1_m, 0_m, 2_m},
                                 frc::Rotation3d{0_deg, 0_deg, 25_deg}};
  frc::Pose3d object = robot + robotToCamera + cameraToObject;

  EXPECT_EQ(robot,
            frc::ObjectToRobotPose(object, cameraToObject, robotToCamera));
}
