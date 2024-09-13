// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "frc/geometry/Pose2d.h"
#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/kinematics/DifferentialDriveWheelPositions.h"
#include "frc/kinematics/DifferentialDriveWheelSpeeds.h"
#include "frc/kinematics/Odometry.h"
#include "units/length.h"

namespace frc {
/**
 * Class for differential drive odometry. Odometry allows you to track the
 * robot's position on the field over the course of a match using readings from
 * 2 encoders and a gyroscope.
 *
 * Teams can use odometry during the autonomous period for complex tasks like
 * path following. Furthermore, odometry can be used for latency compensation
 * when using computer-vision systems.
 *
 * It is important that you reset your encoders to zero before using this class.
 * Any subsequent pose resets also require the encoders to be reset to zero.
 */
class WPILIB_DLLEXPORT DifferentialDriveOdometry
    : public Odometry<DifferentialDriveWheelSpeeds,
                      DifferentialDriveWheelPositions> {
 public:
  /**
   * Constructs a DifferentialDriveOdometry object.
   *
   * IF leftDistance and rightDistance are unspecified,
   * You NEED to reset your encoders (to zero).
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftDistance The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   * @param initialPose The starting position of the robot on the field.
   */
  explicit DifferentialDriveOdometry(const Rotation2d& gyroAngle,
                                     units::meter_t leftDistance,
                                     units::meter_t rightDistance,
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
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftDistance The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   */
  void ResetPosition(const Rotation2d& gyroAngle, units::meter_t leftDistance,
                     units::meter_t rightDistance, const Pose2d& pose) {
    Odometry<DifferentialDriveWheelSpeeds,
             DifferentialDriveWheelPositions>::ResetPosition(gyroAngle,
                                                             {leftDistance,
                                                              rightDistance},
                                                             pose);
  }

  /**
   * Updates the robot position on the field using distance measurements from
   * encoders. This method is more numerically accurate than using velocities to
   * integrate the pose and is also advantageous for teams that are using lower
   * CPR encoders.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftDistance The distance traveled by the left encoder.
   * @param rightDistance The distance traveled by the right encoder.
   * @return The new pose of the robot.
   */
  const Pose2d& Update(const Rotation2d& gyroAngle, units::meter_t leftDistance,
                       units::meter_t rightDistance) {
    return Odometry<DifferentialDriveWheelSpeeds,
                    DifferentialDriveWheelPositions>::Update(gyroAngle,
                                                             {leftDistance,
                                                              rightDistance});
  }

 private:
  DifferentialDriveKinematics m_kinematicsImpl{units::meter_t{1}};
};
}  // namespace frc
