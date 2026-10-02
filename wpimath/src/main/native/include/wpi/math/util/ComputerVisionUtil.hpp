// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/geometry/Transform3d.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

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
 *   this would be wpi::math::Transform3d{3_in, 0_in, 0_in,
 * wpi::math::Rotation3d{}}.
 * @return The robot's field-relative pose.
 */
WPILIB_DLLEXPORT
constexpr wpi::math::Pose3d ObjectToRobotPose(
    const wpi::math::Pose3d& objectInField,
    const wpi::math::Transform3d& cameraToObject,
    const wpi::math::Transform3d& robotToCamera) {
  const auto objectToCamera = cameraToObject.Inverse();
  const auto cameraToRobot = robotToCamera.Inverse();
  return objectInField + objectToCamera + cameraToRobot;
}

}  // namespace wpi::math
