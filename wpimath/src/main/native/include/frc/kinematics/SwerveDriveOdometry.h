// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <cstddef>
#include <ctime>

#include <wpi/timestamp.h>

#include "SwerveDriveKinematics.h"
#include "SwerveModuleState.h"
#include "frc/geometry/Pose2d.h"
#include "units/time.h"

namespace frc {

/**
 * Class for swerve drive odometry. Odometry allows you to track the robot's
 * position on the field over a course of a match using readings from your
 * swerve drive encoders and swerve azimuth encoders.
 *
 * Teams can use odometry during the autonomous period for complex tasks like
 * path following. Furthermore, odometry can be used for latency compensation
 * when using computer-vision systems.
 */
template <size_t NumModules>
class SwerveDriveOdometry {
 public:
  /**
   * Constructs a SwerveDriveOdometry object.
   *
   * @param kinematics The swerve drive kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param initialPose The starting position of the robot on the field.
   */
  SwerveDriveOdometry(SwerveDriveKinematics<NumModules> kinematics,
                      const Rotation2d& gyroAngle,
                      const Pose2d& initialPose = Pose2d());

  /**
   * Resets the robot's position on the field.
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
  }

  /**
   * Returns the position of the robot on the field.
   * @return The pose of the robot.
   */
  const Pose2d& GetPose() const { return m_pose; }

  /**
   * Updates the robot's position on the field using forward kinematics and
   * integration of the pose over time. This method takes in the current time as
   * a parameter to calculate period (difference between two timestamps). The
   * period is used to calculate the change in distance from a velocity. This
   * also takes in an angle parameter which is used instead of the
   * angular rate that is calculated from forward kinematics.
   *
   * @param currentTime The current time.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param moduleStates The current state of all swerve modules. Please provide
   *                     the states in the same order in which you instantiated
   *                     your SwerveDriveKinematics.
   *
   * @return The new pose of the robot.
   */
  template <typename... ModuleStates>
  const Pose2d& UpdateWithTime(units::second_t currentTime,
                               const Rotation2d& gyroAngle,
                               ModuleStates&&... moduleStates);

  /**
   * Updates the robot's position on the field using forward kinematics and
   * integration of the pose over time. This method automatically calculates
   * the current time to calculate period (difference between two timestamps).
   * The period is used to calculate the change in distance from a velocity.
   * This also takes in an angle parameter which is used instead of the
   * angular rate that is calculated from forward kinematics.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param moduleStates The current state of all swerve modules. Please provide
   *                     the states in the same order in which you instantiated
   *                     your SwerveDriveKinematics.
   *
   * @return The new pose of the robot.
   */
  template <typename... ModuleStates>
  const Pose2d& Update(const Rotation2d& gyroAngle,
                       ModuleStates&&... moduleStates) {
    return UpdateWithTime(wpi::Now() * 1.0e-6_s, gyroAngle, moduleStates...);
  }

 private:
  SwerveDriveKinematics<NumModules> m_kinematics;
  Pose2d m_pose;

  units::second_t m_previousTime = -1_s;
  Rotation2d m_previousAngle;
  Rotation2d m_gyroOffset;
};

}  // namespace frc

#include "SwerveDriveOdometry.inc"
