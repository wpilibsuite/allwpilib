// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "frc/geometry/Pose3d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Transform3d.h"
#include "frc/geometry/Translation3d.h"
#include "units/angle.h"
#include "units/length.h"

namespace frc {

/**
 * Algorithm from
 * https://docs.limelightvision.io/en/latest/cs_estimating_distance.html
 * Estimates range to a target using the target's elevation. This method can
 * produce more stable results than SolvePNP when well tuned, if the full 6d
 * robot pose is not required.
 *
 * @param cameraHeight The height of the camera off the floor.
 * @param targetHeight The height of the target off the floor.
 * @param cameraPitch The pitch of the camera from the horizontal plane.
 *                    Positive values up.
 * @param targetPitch The pitch of the target in the camera's lens. Positive
 *                    values up.
 * @param targetYaw The yaw of the target in the camera's lens.
 * @return The estimated distance to the target.
 */
WPILIB_DLLEXPORT
units::meter_t CalculateDistanceToTarget(units::meter_t cameraHeight,
                                         units::meter_t targetHeight,
                                         units::radian_t cameraPitch,
                                         units::radian_t targetPitch,
                                         units::radian_t targetYaw);

/**
 * Estimate the position of the robot in the field.
 *
 * @param cameraHeight The physical height of the camera off the floor.
 * @param targetHeight The physical height of the target off the floor. This
 *                     should be the height of whatever is being targeted (i.e.
 *                     if the targeting region is set to top, this should be the
 *                     height of the top of the target).
 * @param cameraPitch The pitch of the camera from the horizontal plane.
 *                    Positive values up.
 * @param targetPitch The pitch of the target in the camera's lens. Positive
 *                    values up.
 * @param targetYaw The observed yaw of the target. Note that this *must* be
 *                  CCW-positive, and Photon returns CW-positive.
 * @param gyroAngle The current robot gyro angle, likely from odometry.
 * @param fieldToTarget A Pose3d representing the target position in the field
 *                      coordinate system.
 * @param cameraToRobot The position of the robot relative to the camera. If the
 *                      camera was mounted 3 inches behind the "origin" (usually
 *                      physical center) of the robot, this would be
 *                      frc::Transform3d{3_in, 0_in, 0_in, frc::Rotation3d{}}.
 * @return The position of the robot in the field.
 */
WPILIB_DLLEXPORT
frc::Pose3d EstimateFieldToRobot(
    units::meter_t cameraHeight, units::meter_t targetHeight,
    units::radian_t cameraPitch, units::radian_t targetPitch,
    const frc::Rotation2d& targetYaw, const frc::Rotation2d& gyroAngle,
    const frc::Pose3d& fieldToTarget, const frc::Transform3d& cameraToRobot);

/**
 * Estimates the pose of the robot in the field coordinate system, given the
 * position of the target relative to the camera, the target relative to the
 * field, and the robot relative to the camera.
 *
 * @param cameraToTarget The position of the target relative to the camera.
 * @param fieldToTarget  The position of the target in the field.
 * @param cameraToRobot  The position of the robot relative to the camera. If
 *                       the camera was mounted 3 inches behind the "origin"
 *                       (usually physical center) of the robot, this would be
 *                       frc::Transform3d{3_in, 0_in, 0_in, frc::Rotation3d{}}.
 * @return The position of the robot in the field.
 */
WPILIB_DLLEXPORT
frc::Pose3d EstimateFieldToRobot(const frc::Transform3d& cameraToTarget,
                                 const frc::Pose3d& fieldToTarget,
                                 const frc::Transform3d& cameraToRobot);

/**
 * Estimates a Transform3d that maps the camera position to the target position,
 * using the robot's gyro. Note that the gyro angle provided *must* line up with
 * the field coordinate system -- that is, it should read zero degrees when
 * pointed towards the opposing alliance station, and increase as the robot
 * rotates CCW.
 *
 * @param cameraToTargetTranslation A Translation3d that encodes the x/y
 *                                  position of the target relative to the
 *                                  camera.
 * @param fieldToTarget A Pose3d representing the target position in the field
 *                      coordinate system.
 * @param gyroAngle The current robot gyro angle, likely from odometry.
 * @return A Transform3d that takes us from the camera to the target.
 */
WPILIB_DLLEXPORT
frc::Transform3d EstimateCameraToTarget(
    const frc::Translation3d& cameraToTargetTranslation,
    const frc::Pose3d& fieldToTarget, const frc::Rotation2d& gyroAngle);

/**
 * Estimates the pose of the camera in the field coordinate system, given the
 * position of the target relative to the camera, and the target relative to
 * the field. This *only* tracks the position of the camera, not the position
 * of the robot itself.
 *
 * @param cameraToTarget The position of the target relative to the camera.
 * @param fieldToTarget  The position of the target in the field.
 * @return The position of the camera in the field.
 */
WPILIB_DLLEXPORT
frc::Pose3d EstimateFieldToCamera(const frc::Transform3d& cameraToTarget,
                                  const frc::Pose3d& fieldToTarget);

}  // namespace frc
