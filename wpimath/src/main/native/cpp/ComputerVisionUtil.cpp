// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/ComputerVisionUtil.h"

#include "units/math.h"

namespace frc {

units::meter_t CalculateDistanceToTarget(units::meter_t cameraHeight,
                                         units::meter_t targetHeight,
                                         units::radian_t cameraPitch,
                                         units::radian_t targetPitch,
                                         units::radian_t targetYaw) {
  return (targetHeight - cameraHeight) /
         (units::math::tan(cameraPitch + targetPitch) *
          units::math::cos(targetYaw));
}

frc::Pose2d EstimateFieldToRobot(
    units::meter_t cameraHeight, units::meter_t targetHeight,
    units::radian_t cameraPitch, units::radian_t targetPitch,
    const frc::Rotation2d& targetYaw, const frc::Rotation2d& gyroAngle,
    const frc::Pose2d& fieldToTarget, const frc::Transform2d& cameraToRobot) {
  return EstimateFieldToRobot(
      EstimateCameraToTarget(
          frc::Translation2d{
              CalculateDistanceToTarget(cameraHeight, targetHeight, cameraPitch,
                                        targetPitch, targetYaw.Radians()),
              targetYaw},
          fieldToTarget, gyroAngle),
      fieldToTarget, cameraToRobot);
}

frc::Pose2d EstimateFieldToRobot(const frc::Transform2d& cameraToTarget,
                                 const frc::Pose2d& fieldToTarget,
                                 const frc::Transform2d& cameraToRobot) {
  return EstimateFieldToCamera(cameraToTarget, fieldToTarget)
      .TransformBy(cameraToRobot);
}

frc::Transform2d EstimateCameraToTarget(
    const frc::Translation2d& cameraToTargetTranslation,
    const frc::Pose2d& fieldToTarget, const frc::Rotation2d& gyroAngle) {
  // Map our camera at the origin out to our target, in the robot reference
  // frame. Gyro angle is needed because there's a circle of possible camera
  // poses for which the camera has the same yaw from camera to target.
  return frc::Transform2d{cameraToTargetTranslation,
                          -gyroAngle - fieldToTarget.Rotation()};
}

frc::Pose2d EstimateFieldToCamera(const frc::Transform2d& cameraToTarget,
                                  const frc::Pose2d& fieldToTarget) {
  auto targetToCamera = cameraToTarget.Inverse();
  return fieldToTarget.TransformBy(targetToCamera);
}

}  // namespace frc
