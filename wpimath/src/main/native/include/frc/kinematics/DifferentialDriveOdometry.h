// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "DifferentialDriveKinematics.h"
#include "frc/geometry/Pose2d.h"
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
class DifferentialDriveOdometry {
 public:
  /**
   * Constructs a DifferentialDriveOdometry object.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param initialPose The starting position of the robot on the field.
   */
  explicit DifferentialDriveOdometry(const Rotation2d& gyroAngle,
                                     const Pose2d& initialPose = Pose2d());

  /**
   * Resets the robot's position on the field.
   *
   * You NEED to reset your encoders (to zero) when calling this method.
   *
   * The gyroscope angle does not need to be reset here on the user's robot
   * code. The library automatically takes care of offsetting the gyro angle.
   *
   * @param pose The position on the field that your robot is at.
   * @param gyroAngle The angle reported by the gyroscope.
   */
  void ResetPosition(const Pose2d& pose, const Rotation2d& gyroAngle) {
    m_pose = pose;
    m_previousAngle = pose.Rotation();
    m_gyroOffset = m_pose.Rotation() - gyroAngle;

    m_prevLeftDistance = 0_m;
    m_prevRightDistance = 0_m;
  }

  /**
   * Returns the position of the robot on the field.
   * @return The pose of the robot.
   */
  const Pose2d& GetPose() const { return m_pose; }

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
                       units::meter_t rightDistance);

 private:
  Pose2d m_pose;

  Rotation2d m_gyroOffset;
  Rotation2d m_previousAngle;

  units::meter_t m_prevLeftDistance = 0_m;
  units::meter_t m_prevRightDistance = 0_m;
};
}  // namespace frc
