// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/kinematics/Odometry.hpp"
#include "wpi/math/kinematics/ThreeDeadWheelPositions.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Class for triple follower wheel odometry. Odometry allows you to track the
 * robot's position on the field using readings from three dead wheels and a
 * gyroscope.
 */
class WPILIB_DLLEXPORT ThreeDeadWheelOdometry
    : public Odometry<ThreeDeadWheelPositions> {
 public:
  /**
   * Constructs a ThreeDeadWheelOdometry object.
   *
   * @param x1WheelYPos The y-position of the first forward-facing wheel
   *     relative to the center of the robot.
   * @param x2WheelYPos The y-position of the second forward-facing wheel
   *     relative to the center of the robot.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the
   *     center of the robot.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The current wheel position readings.
   * @param initialPose The starting position of the robot on the field.
   */
  ThreeDeadWheelOdometry(
      wpi::units::meter_t x1WheelYPos, wpi::units::meter_t x2WheelYPos,
      wpi::units::meter_t yWheelXPos, const Rotation2d& gyroAngle,
      const ThreeDeadWheelPositions& wheelPositions,
      const Pose2d& initialPose = Pose2d{})
      : Odometry(gyroAngle, initialPose),
        m_x1WheelYPos(x1WheelYPos),
        m_x2WheelYPos(x2WheelYPos),
        m_yWheelXPos(yWheelXPos),
        m_previousWheelPositions(wheelPositions) {
    if (units::math::abs(m_x2WheelYPos - m_x1WheelYPos) < 0.1_m) {
      throw std::domain_error(
          "x1WheelYPos and x2WheelYPos must not be too close to each other");
    }
  }

  /**
   * Resets the robot's position on the field.
   *
   * The gyroscope angle does not need to be reset here on the user's robot
   * code. The library automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param x1 The distance traveled by the first forward-facing wheel.
   * @param x2 The distance traveled by the second forward-facing wheel.
   * @param y The distance traveled by the left-facing wheel.
   * @param pose The position on the field that your robot is at.
   */
  void ResetPosition(const Rotation2d& gyroAngle, wpi::units::meter_t x1,
                     wpi::units::meter_t x2, wpi::units::meter_t y,
                     const Pose2d& pose) {
    ResetPosition(gyroAngle, {x1, x2, y}, pose);
  }

  void ResetPosition(const Rotation2d& gyroAngle,
                     const ThreeDeadWheelPositions& wheelPositions,
                     const Pose2d& pose) override {
    m_previousWheelPositions = wheelPositions;
    Odometry::ResetPosition(gyroAngle, pose);
  }

  /**
   * Updates the robot's position on the field using forward kinematics and
   * integration of the pose over time. This method takes in an angle parameter
   * which is used instead of the angular rate that is calculated from forward
   * kinematics, in addition to the current distance measurement at each wheel.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param x1 The distance traveled by the first forward-facing wheel.
   * @param x2 The distance traveled by the second forward-facing wheel.
   * @param y The distance traveled by the left-facing wheel.
   * @return The new pose of the robot.
   */
  const Pose2d& Update(const Rotation2d& gyroAngle, wpi::units::meter_t x1,
                       wpi::units::meter_t x2, wpi::units::meter_t y) {
    return Update(gyroAngle, {x1, x2, y});
  }

  const Pose2d& Update(
      const Rotation2d& gyroAngle,
      const ThreeDeadWheelPositions& wheelPositions) override {
    auto deltaXWheel1 = wheelPositions.x1 - m_previousWheelPositions.x1;
    auto deltaXWheel2 = wheelPositions.x2 - m_previousWheelPositions.x2;
    auto deltaYWheel = wheelPositions.y - m_previousWheelPositions.y;

    auto deltaTheta =
        (deltaXWheel1 - deltaXWheel2) / (m_x2WheelYPos - m_x1WheelYPos);
    auto deltaX = (deltaXWheel1 + deltaXWheel2) / 2.0;
    auto deltaY = deltaYWheel - m_yWheelXPos * deltaTheta;
    m_previousWheelPositions = wheelPositions;
    return Odometry::Update(gyroAngle,
                            {deltaX, deltaY, wpi::units::radian_t{deltaTheta}});
  }

  /**
   * Converts measured wheel velocities to chassis velocities using inverse
   * kinematics.
   *
   * @param vx1 The velocity of the first forward-facing wheels.
   * @param vx2 The velocity of the second forward-facing wheel.
   * @param vy The velocity of the sideways-facing wheel.
   * @return The velocity of the chassis.
   */
  constexpr ChassisVelocities ToChassisVelocities(
      wpi::units::meters_per_second_t vx1, wpi::units::meters_per_second_t vx2,
      wpi::units::meters_per_second_t vy) const {
    auto omega = (vx1 - vx2) / (m_x2WheelYPos - m_x1WheelYPos);
    return {(vx1 + vx2) / 2.0, vy - m_yWheelXPos * omega, omega * 1_rad};
  }

 private:
  wpi::units::meter_t m_x1WheelYPos;
  wpi::units::meter_t m_x2WheelYPos;
  wpi::units::meter_t m_yWheelXPos;

  ThreeDeadWheelPositions m_previousWheelPositions;
};
}  // namespace wpi::math
