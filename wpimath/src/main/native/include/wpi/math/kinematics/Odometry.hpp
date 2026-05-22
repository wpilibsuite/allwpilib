// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

/**
 * Class for odometry. Robot code should not use this directly- Instead, use the
 * particular type for your drivetrain (e.g., DifferentialDriveOdometry).
 * Odometry allows you to track the robot's position on the field over a course
 * of a match using readings from your wheel encoders.
 *
 * Teams can use odometry during the autonomous period for complex tasks like
 * path following. Furthermore, odometry can be used for latency compensation
 * when using computer-vision systems.
 *
 * @tparam WheelPositions Wheel positions type.
 * @tparam WheelVelocities Wheel velocities type.
 * @tparam WheelAccelerations Wheel accelerations type.
 */
template <typename WheelPositions>
class WPILIB_DLLEXPORT Odometry {
 public:
  virtual ~Odometry() = default;

  /**
   * Constructs an Odometry object.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param initialPose The starting position of the robot on the field.
   */
  explicit Odometry(const Rotation2d& gyroAngle,
                    const Pose2d& initialPose = Pose2d{})
      : m_pose(initialPose) {
    m_previousAngle = m_pose.Rotation();
    m_gyroOffset = (-gyroAngle).RotateBy(m_pose.Rotation());
  }

  /**
   * Resets the robot's position on the field.
   *
   * The gyroscope angle does not need to be reset here on the user's robot
   * code. The library automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The current distances measured by each wheel.
   * @param pose The position on the field that your robot is at.
   */
  virtual void ResetPosition(const Rotation2d& gyroAngle,
                             const WheelPositions& wheelPositions,
                             const Pose2d& pose) = 0;

  /**
   * Resets the pose.
   *
   * @param pose The pose to reset to.
   */
  void ResetPose(const Pose2d& pose) {
    m_gyroOffset =
        m_gyroOffset.RotateBy(-m_pose.Rotation()).RotateBy(pose.Rotation());
    m_pose = pose;
    m_previousAngle = pose.Rotation();
  }

  /**
   * Resets the translation of the pose.
   *
   * @param translation The translation to reset to.
   */
  void ResetTranslation(const Translation2d& translation) {
    m_pose = Pose2d{translation, m_pose.Rotation()};
  }

  /**
   * Resets the rotation of the pose.
   *
   * @param rotation The rotation to reset to.
   */
  void ResetRotation(const Rotation2d& rotation) {
    m_gyroOffset = m_gyroOffset.RotateBy(m_pose.Rotation()).RotateBy(rotation);
    m_pose = Pose2d{m_pose.Translation(), rotation};
    m_previousAngle = rotation;
  }

  /**
   * Returns the position of the robot on the field.
   * @return The pose of the robot.
   */
  const Pose2d& GetPose() const { return m_pose; }

  /**
   * Updates the robot's position on the field using forward kinematics and
   * integration of the pose over time. This method takes in an angle parameter
   * which is used instead of the angular rate that is calculated from forward
   * kinematics, in addition to the current distance measurement at each wheel.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The current encoder readings.
   * @return The new pose of the robot.
   */
  virtual const Pose2d& Update(const Rotation2d& gyroAngle,
                               const WheelPositions& wheelPositions) = 0;

 protected:
  /**
   * Resets the robot's position on the field.
   *
   * The implementing class should call this method once they have reset their
   * wheel positions.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param pose The position on the field that your robot is at.
   */
  void ResetPosition(const Rotation2d& gyroAngle, const Pose2d& pose) {
    m_pose = pose;
    m_previousAngle = pose.Rotation();
    m_gyroOffset = (-gyroAngle).RotateBy(m_pose.Rotation());
  }

  /**
   * Updates the robot's position on the field by integrating the pose over
   * time. This protected method takes in a twist, which is to be calculated by
   * the implementing class's kinematics.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param twist The twist as calculated by the implementing class's
   * kinematics.
   * @return The new pose of the robot.
   */
  const Pose2d& Update(const Rotation2d& gyroAngle, Twist2d twist) {
    auto angle = gyroAngle.RotateBy(m_gyroOffset);

    twist.dtheta = (angle - m_previousAngle).Radians();

    auto newPose = m_pose + twist.Exp();

    m_previousAngle = angle;
    m_pose = {newPose.Translation(), angle};

    return m_pose;
  }

 private:
  Pose2d m_pose;

  // Always equal to m_pose.Rotation()
  Rotation2d m_previousAngle;

  Rotation2d m_gyroOffset;
};

}  // namespace wpi::math
