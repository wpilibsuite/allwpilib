// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

/**
 * Class for double follower wheel odometry. Odometry allows you to track the
 * robot's position on the field over the course of a match using readings from
 * 2 encoders and a gyroscope.
 *
 * <p>Teams can use odometry during the autonomous period for complex tasks like
 * path following. Furthermore, odometry can be used for latency compensation
 * when using computer-vision systems.
 */
class WPILIB_DLLEXPORT TwoDeadWheelOdometry {
  /*
   * This class uses logic cloned from Odometry.java. It does not extend
   * Odometry because the structure of the kinematics and odometry classes
   * assume that the gyro angle is independent from the kinematics, while in a
   * two-dead-wheel configuration, the gyro is required to do the kinematics, as
   * otherwise the forward kinematics is rank-deficient.
   */

 public:
  /**
   * Constructs a TwoDeadWheelOdometry object.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to
   * the center of the robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the
   * center of the robot in meters.
   * @param xWheelPos The distance traveled by the forward-facing wheel, in
   * meters.
   * @param yWheelPos The distance traveled by the left-facing wheel, in meters.
   * @param gyroAngle The angle reported by the gyroscope. This does not need to
   * be offset to match the robot's orientation on the field.
   * @param initialPose The starting position of the robot on the field.
   */
  explicit TwoDeadWheelOdometry(wpi::units::meter_t xWheelYPos,
                                wpi::units::meter_t yWheelXPos,
                                wpi::units::meter_t xWheelPos,
                                wpi::units::meter_t yWheelPos,
                                const Rotation2d& gyroAngle,
                                const Pose2d& initialPose = Pose2d{});

  /**
   * Resets the robot's position on the field.
   *
   * @param xWheelPos The distance traveled by the forward-facing wheel, in
   * meters.
   * @param yWheelPos The distance traveled by the left-facing wheel, in meters.
   * @param gyroAngle The angle reported by the gyroscope. This does not need to
   * be offset to match the robot's orientation on the field.
   * @param pose The new position of the robot on the field.
   */
  void ResetPosition(wpi::units::meter_t xWheelPos,
                     wpi::units::meter_t yWheelPos, const Rotation2d& gyroAngle,
                     const Pose2d& pose) {
    m_previousGyroAngle = gyroAngle;
    m_previousXWheelPos = xWheelPos;
    m_previousYWheelPos = yWheelPos;
    m_pose = pose;
  }

  /**
   * Resets the pose.
   *
   * @param pose The pose to reset to.
   */
  void ResetPose(const Pose2d& pose) { m_pose = pose; }

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
    m_pose = Pose2d{m_pose.Translation(), rotation};
  }

  /**
   * Returns the position of the robot on the field.
   * @return The pose of the robot.
   */
  const Pose2d& GetPose() const { return m_pose; }

  /**
   * Updates the robot's position.
   *
   * @param xWheelPos The distance traveled by the forward-facing wheel, in
   * meters.
   * @param yWheelPos The distance traveled by the left-facing wheel, in meters.
   * @param gyroAngle The angle reported by the gyroscope. This does not need to
   * be offset to match the robot's orientation on the field.
   * @return The updated pose.
   */
  const Pose2d& Update(wpi::units::meter_t xWheelPos,
                       wpi::units::meter_t yWheelPos,
                       const Rotation2d& gyroAngle) {
    const auto deltaTheta = (gyroAngle - m_previousGyroAngle).Radians();
    const auto deltaX = xWheelPos - m_previousXWheelPos +
                        wpi::units::meter_t{m_xWheelYPos * deltaTheta.value()};
    const auto deltaY = yWheelPos - m_previousYWheelPos -
                        wpi::units::meter_t{m_yWheelXPos * deltaTheta.value()};
    const Twist2d twist{deltaX, deltaY, deltaTheta};

    m_pose = m_pose + twist.Exp();

    m_previousXWheelPos = xWheelPos;
    m_previousYWheelPos = yWheelPos;
    m_previousGyroAngle = gyroAngle;

    return m_pose;
  }

  /**
   * Converts measured wheel velocities to chassis velocities using inverse
   * kinematics.
   *
   * @param vx The velocity of the forward-facing wheel.
   * @param vy The velocity of the sideways-facing wheel.
   * @param omega The angular velocity of the robot as reported by the gyro.
   * @return The velocity of the chassis.
   */
  ChassisVelocities ToChassisVelocities(
      wpi::units::meters_per_second_t vx, wpi::units::meters_per_second_t vy,
      wpi::units::radians_per_second_t omega) const {
    return {vx + m_xWheelYPos * omega / 1_rad,
            vy - m_yWheelXPos * omega / 1_rad, omega};
  }

 private:
  const wpi::units::meter_t m_xWheelYPos;
  const wpi::units::meter_t m_yWheelXPos;
  Pose2d m_pose;

  wpi::units::meter_t m_previousXWheelPos;
  wpi::units::meter_t m_previousYWheelPos;

  Rotation2d m_previousGyroAngle;
};

}  // namespace wpi::math
