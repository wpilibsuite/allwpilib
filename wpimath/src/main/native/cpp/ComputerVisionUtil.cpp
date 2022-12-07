// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/ComputerVisionUtil.h"

#include "frc/geometry/Rotation3d.h"
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

frc::Pose3d EstimateFieldToRobot(
    units::meter_t cameraHeight, units::meter_t targetHeight,
    units::radian_t cameraPitch, units::radian_t targetPitch,
    const frc::Rotation2d& targetYaw, const frc::Rotation2d& gyroAngle,
    const frc::Pose3d& fieldToTarget, const frc::Transform3d& cameraToRobot) {
  auto distanceAlongGround =
      CalculateDistanceToTarget(cameraHeight, targetHeight, cameraPitch,
                                targetPitch, targetYaw.Radians());
  auto range =
      units::math::hypot(distanceAlongGround, targetHeight - cameraHeight);
  return EstimateFieldToRobot(
      EstimateCameraToTarget(
          Translation3d{range,
                        Rotation3d{0_rad, targetPitch, targetYaw.Radians()}},
          fieldToTarget, gyroAngle),
      fieldToTarget, cameraToRobot);
}

frc::Pose3d EstimateFieldToRobot(const frc::Transform3d& cameraToTarget,
                                 const frc::Pose3d& fieldToTarget,
                                 const frc::Transform3d& cameraToRobot) {
  return EstimateFieldToCamera(cameraToTarget, fieldToTarget)
      .TransformBy(cameraToRobot);
}

frc::Transform3d EstimateCameraToTarget(
    const frc::Translation3d& cameraToTargetTranslation,
    const frc::Pose3d& fieldToTarget, const frc::Rotation2d& gyroAngle) {
  // Map our camera at the origin out to our target, in the robot reference
  // frame. Gyro angle is needed because there's a circle of possible camera
  // poses for which the camera has the same yaw from camera to target.
  return Transform3d{cameraToTargetTranslation,
                     Rotation3d{0_rad, 0_rad, -gyroAngle.Radians()} -
                         fieldToTarget.Rotation()};
}

frc::Pose3d EstimateFieldToCamera(const frc::Transform3d& cameraToTarget,
                                  const frc::Pose3d& fieldToTarget) {
  auto targetToCamera = cameraToTarget.Inverse();
  return fieldToTarget.TransformBy(targetToCamera);
}

}  // namespace frc
