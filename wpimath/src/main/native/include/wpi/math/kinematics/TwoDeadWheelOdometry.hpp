// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/kinematics/Odometry.hpp"
#include "wpi/math/kinematics/TwoDeadWheelPositions.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Class for double follower wheel odometry. Odometry allows you to track the
 * robot's position on the field using readings from two dead wheels and a
 * gyroscope.
 */
class WPILIB_DLLEXPORT TwoDeadWheelOdometry
    : public Odometry<TwoDeadWheelPositions> {
 public:
  /**
   * Constructs a TwoDeadWheelOdometry object.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to
   *     the center of the robot.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the
   *     center of the robot.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The current wheel position readings.
   * @param initialPose The starting position of the robot on the field.
   */
  TwoDeadWheelOdometry(wpi::units::meter_t xWheelYPos,
                       wpi::units::meter_t yWheelXPos,
                       const Rotation2d& gyroAngle,
                       const TwoDeadWheelPositions& wheelPositions,
                       const Pose2d& initialPose = Pose2d{})
      : Odometry(gyroAngle, initialPose),
        m_xWheelYPos(xWheelYPos),
        m_yWheelXPos(yWheelXPos),
        m_previousAngle(gyroAngle),
        m_previousWheelPositions(wheelPositions) {}

  void ResetPosition(const Rotation2d& gyroAngle,
                     const TwoDeadWheelPositions& wheelPositions,
                     const Pose2d& pose) override {
    m_previousAngle = gyroAngle;
    m_previousWheelPositions = wheelPositions;
    Odometry::ResetPosition(gyroAngle, pose);
  }

  const Pose2d& Update(const Rotation2d& gyroAngle,
                       const TwoDeadWheelPositions& wheelPositions) override {
    auto deltaTheta = (gyroAngle - m_previousAngle).Radians();
    auto deltaX =
        wheelPositions.x - m_previousWheelPositions.x +
        wpi::units::meter_t{m_xWheelYPos.value() * deltaTheta.value()};
    auto deltaY =
        wheelPositions.y - m_previousWheelPositions.y -
        wpi::units::meter_t{m_yWheelXPos.value() * deltaTheta.value()};
    Twist2d twist{deltaX, deltaY, deltaTheta};
    m_previousAngle = gyroAngle;
    m_previousWheelPositions = wheelPositions;
    return Odometry::Update(gyroAngle, twist);
  }

  /**
   * Converts measured wheel velocities to chassis velocities using inverse
   * kinematics.
   *
   * @param omega The angular velocity of the robot as reported by the gyro.
   * @param vx The velocity of the forward-facing wheel.
   * @param vy The velocity of the sideways-facing wheel.
   * @return The velocity of the chassis.
   */
  constexpr ChassisVelocities ToChassisVelocities(
      wpi::units::radians_per_second_t omega,
      wpi::units::meters_per_second_t vx,
      wpi::units::meters_per_second_t vy) const {
    auto vxCorrection =
        wpi::units::meters_per_second_t{m_xWheelYPos.value() * omega.value()};
    auto vyCorrection =
        wpi::units::meters_per_second_t{m_yWheelXPos.value() * omega.value()};
    return {vx + vxCorrection, vy - vyCorrection, omega};
  }

 private:
  wpi::units::meter_t m_xWheelYPos;
  wpi::units::meter_t m_yWheelXPos;

  Rotation2d m_previousAngle;
  TwoDeadWheelPositions m_previousWheelPositions;
};
}  // namespace wpi::math
