// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "frc/geometry/Pose3d.h"
#include "frc/geometry/Transform3d.h"

namespace frc {

/**
 * Returns the robot's pose in the field coordinate system given an object's
 * field-relative pose, the transformation from the camera's pose to the
 * object's pose (obtained via computer vision), and the transformation from the
 * robot's pose to the camera's pose.
 *
 * The object could be a target or a fiducial marker.
 *
 * @param objectInField An object's field-relative pose.
 * @param cameraToObject The transformation from the camera's pose to the
 *   object's pose. This comes from computer vision.
 * @param robotToCamera The transformation from the robot's pose to the camera's
 *   pose. This can either be a constant for a rigidly mounted camera, or
 *   variable if the camera is mounted to a turret. If the camera was mounted 3
 *   inches in front of the "origin" (usually physical center) of the robot,
 *   this would be frc::Transform3d{3_in, 0_in, 0_in, frc::Rotation3d{}}.
 * @return The robot's field-relative pose.
 */
WPILIB_DLLEXPORT
constexpr frc::Pose3d ObjectToRobotPose(const frc::Pose3d& objectInField,
                                        const frc::Transform3d& cameraToObject,
                                        const frc::Transform3d& robotToCamera) {
  const auto objectToCamera = cameraToObject.Inverse();
  const auto cameraToRobot = robotToCamera.Inverse();
  return objectInField + objectToCamera + cameraToRobot;
}

}  // namespace frc
