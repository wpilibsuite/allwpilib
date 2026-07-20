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
class WPILIB_DLLEXPORT TwoDeadWheelOdometry {
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
      : m_pose(initialPose),
        m_xWheelYPos(xWheelYPos),
        m_yWheelXPos(yWheelXPos),
        m_previousWheelPositions(wheelPositions) {
    m_previousAngle = m_pose.Rotation();
    m_gyroOffset = (-gyroAngle).RotateBy(m_pose.Rotation());
  }

  void ResetPosition(const Rotation2d& gyroAngle,
                     const TwoDeadWheelPositions& wheelPositions,
                     const Pose2d& pose) {
    m_pose = pose;
    m_previousAngle = pose.Rotation();
    m_gyroOffset = (-gyroAngle).RotateBy(m_pose.Rotation());
    m_previousWheelPositions = wheelPositions;
  }

  const Pose2d& Update(const Rotation2d& gyroAngle,
                       const TwoDeadWheelPositions& wheelPositions) {
    auto deltaTheta = (gyroAngle - m_previousAngle).Radians();
    auto deltaX =
        wheelPositions.x - m_previousWheelPositions.x +
        wpi::units::meter_t{m_xWheelYPos.value() * deltaTheta.value()};
    auto deltaY =
        wheelPositions.y - m_previousWheelPositions.y -
        wpi::units::meter_t{m_yWheelXPos.value() * deltaTheta.value()};
    Twist2d twist{deltaX, deltaY, deltaTheta};

    auto angle = gyroAngle.RotateBy(m_gyroOffset);
    auto newPose = m_pose + twist.Exp();

    m_previousAngle = angle;
    m_previousWheelPositions = wheelPositions;
    m_pose = {newPose.Translation(), angle};

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
  constexpr ChassisVelocities ToChassisVelocities(
      wpi::units::meters_per_second_t vx, wpi::units::meters_per_second_t vy,
      wpi::units::radians_per_second_t omega) const {
    auto vxCorrection =
        wpi::units::meters_per_second_t{m_xWheelYPos.value() * omega.value()};
    auto vyCorrection =
        wpi::units::meters_per_second_t{m_yWheelXPos.value() * omega.value()};
    return {vx + vxCorrection, vy - vyCorrection, omega};
  }

 private:
  wpi::units::meter_t m_xWheelYPos;
  wpi::units::meter_t m_yWheelXPos;

  Pose2d m_pose;

  Rotation2d m_previousAngle;
  Rotation2d m_gyroOffset;
  TwoDeadWheelPositions m_previousWheelPositions;
};
}  // namespace wpi::math
