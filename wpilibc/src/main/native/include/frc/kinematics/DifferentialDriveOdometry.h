/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/units.h>

#include "DifferentialDriveKinematics.h"
#include "frc/geometry/Pose2d.h"
#include "frc2/Timer.h"

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
 * There are two ways of tracking the robot's position on the field with this
 * class: one involving using encoder velocities and the other involving encoder
 * positions. It is very important that only one type of odometry is used with
 * each instantiation of this class.
 *
 * Note: If you are using the encoder positions / distances method, it is
 * important that you reset your encoders to zero before using this class. Any
 * subsequent pose resets also require the encoders to be reset to zero.
 */
class DifferentialDriveOdometry {
 public:
  /**
   * Constructs a DifferentialDriveOdometry object.
   *
   * @param kinematics The differential drive kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param initialPose The starting position of the robot on the field.
   */
  explicit DifferentialDriveOdometry(DifferentialDriveKinematics kinematics,
                                     const Rotation2d& gyroAngle,
                                     const Pose2d& initialPose = Pose2d());

  /**
   * Resets the robot's position on the field.
   *
   * If you are using the encoder distances method instead of the velocity
   * method, you NEED to reset your encoders (to zero) when calling this method.
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
   * Updates the robot's position on the field using forward kinematics and
   * integration of the pose over time. This method takes in the current time as
   * a parameter to calculate period (difference between two timestamps). The
   * period is used to calculate the change in distance from a velocity. This
   * also takes in an angle parameter which is used instead of the
   * angular rate that is calculated from forward kinematics.
   *
   * @param currentTime The current time.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelSpeeds The current wheel speeds.
   *
   * @return The new pose of the robot.
   */
  const Pose2d& UpdateWithTime(units::second_t currentTime,
                               const Rotation2d& gyroAngle,
                               const DifferentialDriveWheelSpeeds& wheelSpeeds);

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

  /**
   * Updates the robot's position on the field using forward kinematics and
   * integration of the pose over time. This method automatically calculates
   * the current time to calculate period (difference between two timestamps).
   * The period is used to calculate the change in distance from a velocity.
   * This also takes in an angle parameter which is used instead of the
   * angular rate that is calculated from forward kinematics.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelSpeeds     The current wheel speeds.
   *
   * @return The new pose of the robot.
   */
  const Pose2d& Update(const Rotation2d& gyroAngle,
                       const DifferentialDriveWheelSpeeds& wheelSpeeds) {
    return UpdateWithTime(frc2::Timer::GetFPGATimestamp(), gyroAngle,
                          wheelSpeeds);
  }

 private:
  DifferentialDriveKinematics m_kinematics;
  Pose2d m_pose;

  units::second_t m_previousTime = -1_s;
  Rotation2d m_gyroOffset;
  Rotation2d m_previousAngle;

  units::meter_t m_prevLeftDistance = 0_m;
  units::meter_t m_prevRightDistance = 0_m;
};
}  // namespace frc
