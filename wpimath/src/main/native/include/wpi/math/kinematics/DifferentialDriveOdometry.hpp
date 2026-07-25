// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"
#include "wpi/math/kinematics/DifferentialDriveWheelAccelerations.hpp"
#include "wpi/math/kinematics/DifferentialDriveWheelPositions.hpp"
#include "wpi/math/kinematics/DifferentialDriveWheelVelocities.hpp"
#include "wpi/math/kinematics/Odometry.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Class for differential drive odometry. Odometry allows you to track the
 * robot's position on the field over the course of a match using readings from
 * 2 encoders and a gyroscope.
 *
 * Teams can use odometry during the autonomous period for complex tasks like
 * path following. Furthermore, odometry can be used for latency compensation
 * when using computer-vision systems.
 */
class WPILIB_DLLEXPORT DifferentialDriveOdometry
    : public Odometry<DifferentialDriveKinematics,
                      DifferentialDriveWheelPositions,
                      DifferentialDriveWheelVelocities,
                      DifferentialDriveWheelAccelerations> {
 public:
  /**
   * Constructs a DifferentialDriveOdometry object.
   *
   * IF leftDistance and rightDistance are unspecified,
   * You NEED to reset your encoders (to zero).
   *
   * @param gyroAngle The angle reported by the gyroscope. This does not need to
   * be offset to match the robot's orientation on the field.
   * @param leftDistance The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   * @param initialPose The starting position of the robot on the field.
   */
  explicit DifferentialDriveOdometry(const Rotation2d& gyroAngle,
                                     wpi::units::meter_t leftDistance,
                                     wpi::units::meter_t rightDistance,
                                     const Pose2d& initialPose = Pose2d{});

  /**
   * Resets the robot's position on the field.
   *
   * IF leftDistance and rightDistance are unspecified,
   * You NEED to reset your encoders (to zero).
   *
   * The gyroscope angle does not need to be reset here on the user's robot
   * code. The library automatically takes care of offsetting the gyro angle.
   *
   * @param pose The position on the field that your robot is at.
   * @param gyroAngle The angle reported by the gyroscope. This does not need to
   * be offset to match the robot's orientation on the field.
   * @param leftDistance The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   */
  void ResetPosition(const Rotation2d& gyroAngle,
                     wpi::units::meter_t leftDistance,
                     wpi::units::meter_t rightDistance, const Pose2d& pose) {
    Odometry::ResetPosition(gyroAngle, {leftDistance, rightDistance}, pose);
  }

  /**
   * Updates the robot position on the field using distance measurements from
   * encoders.
   *
   * @param gyroAngle The angle reported by the gyroscope. This does not need to
   * be offset to match the robot's orientation on the field.
   * @param leftDistance The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   * @return The new pose of the robot.
   */
  const Pose2d& Update(const Rotation2d& gyroAngle,
                       wpi::units::meter_t leftDistance,
                       wpi::units::meter_t rightDistance) {
    return Odometry::Update(gyroAngle, {leftDistance, rightDistance});
  }
};
}  // namespace wpi::math
