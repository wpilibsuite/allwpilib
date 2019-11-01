/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/units.h>

#include "frc/geometry/Pose2d.h"
#include "frc/kinematics/MecanumDriveKinematics.h"
#include "frc/kinematics/MecanumDriveWheelSpeeds.h"
#include "frc2/Timer.h"

namespace frc {

/**
 * Class for mecanum drive odometry. Odometry allows you to track the robot's
 * position on the field over a course of a match using readings from your
 * mecanum wheel encoders.
 *
 * Teams can use odometry during the autonomous period for complex tasks like
 * path following. Furthermore, odometry can be used for latency compensation
 * when using computer-vision systems.
 */
class MecanumDriveOdometry {
 public:
  /**
   * Constructs a MecanumDriveOdometry object.
   *
   * @param kinematics The mecanum drive kinematics for your drivetrain.
   * @param initialPose The starting position of the robot on the field.
   */
  explicit MecanumDriveOdometry(MecanumDriveKinematics kinematics,
                                const Pose2d& initialPose = Pose2d());

  /**
   * Resets the robot's position on the field.
   *
   * @param pose The position on the field that your robot is at.
   */
  void ResetPosition(const Pose2d& pose) {
    m_pose = pose;
    m_previousAngle = pose.Rotation();
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
   * @param angle The angle of the robot.
   * @param wheelSpeeds The current wheel speeds.
   *
   * @return The new pose of the robot.
   */
  const Pose2d& UpdateWithTime(units::second_t currentTime,
                               const Rotation2d& angle,
                               MecanumDriveWheelSpeeds wheelSpeeds);

  /**
   * Updates the robot's position on the field using forward kinematics and
   * integration of the pose over time. This method automatically calculates
   * the current time to calculate period (difference between two timestamps).
   * The period is used to calculate the change in distance from a velocity.
   * This also takes in an angle parameter which is used instead of the
   * angular rate that is calculated from forward kinematics.
   *
   * @param angle The angle of the robot.
   * @param wheelSpeeds     The current wheel speeds.
   *
   * @return The new pose of the robot.
   */
  const Pose2d& Update(const Rotation2d& angle,
                       MecanumDriveWheelSpeeds wheelSpeeds) {
    return UpdateWithTime(frc2::Timer::GetFPGATimestamp(), angle, wheelSpeeds);
  }

 private:
  MecanumDriveKinematics m_kinematics;
  Pose2d m_pose;

  units::second_t m_previousTime = -1_s;
  Rotation2d m_previousAngle;
};

}  // namespace frc
