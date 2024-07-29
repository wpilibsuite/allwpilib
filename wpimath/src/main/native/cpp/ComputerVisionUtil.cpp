// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/ComputerVisionUtil.h"

#include "units/math.h"

namespace frc {

frc::Pose3d ObjectToRobotPose(const frc::Pose3d& objectInField,
                              const frc::Transform3d& cameraToObject,
                              const frc::Transform3d& robotToCamera) {
  const auto objectToCamera = cameraToObject.Inverse();
  const auto cameraToRobot = robotToCamera.Inverse();
  return objectInField + objectToCamera + cameraToRobot;
}

}  // namespace frc
