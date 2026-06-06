// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Rotation3d.hpp"
#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/kinematics/TwoDeadWheelPositions.hpp"
#include "wpi/math/kinematics/Odometry3d.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
class WPILIB_DLLEXPORT TwoDeadWheelOdometry3d
    : public Odometry3d<TwoDeadWheelPositions> {
 public:
  TwoDeadWheelOdometry3d(
      wpi::units::meter_t xWheelYPos, wpi::units::meter_t yWheelXPos,
      const Rotation3d& gyroAngle,
      const TwoDeadWheelPositions& wheelPositions,
      const Pose3d& initialPose = Pose3d{})
      : Odometry3d(gyroAngle, initialPose),
        m_xWheelYPos(xWheelYPos),
        m_yWheelXPos(yWheelXPos),
        m_previousAngle(gyroAngle),
        m_previousWheelPositions(wheelPositions) {}

  void ResetPosition(const Rotation3d& gyroAngle,
                     const TwoDeadWheelPositions& wheelPositions,
                     const Pose3d& pose) override {
    m_previousAngle = gyroAngle;
    m_previousWheelPositions = wheelPositions;
    Odometry3d::ResetPosition(gyroAngle, pose);
  }

  const Pose3d& Update(
      const Rotation3d& gyroAngle,
      const TwoDeadWheelPositions& wheelPositions) override {
    auto deltaAngle = gyroAngle.RelativeTo(m_previousAngle);
    auto deltaTheta = deltaAngle.ToRotation2d().Radians();
    auto deltaX = wheelPositions.x - m_previousWheelPositions.x +
                  m_xWheelYPos * deltaTheta.value();
    auto deltaY = wheelPositions.y - m_previousWheelPositions.y -
                  m_yWheelXPos * deltaTheta.value();
    Twist2d twist{deltaX, deltaY, deltaTheta};
    m_previousAngle = gyroAngle;
    m_previousWheelPositions = wheelPositions;
    return Odometry3d::Update(gyroAngle, twist);
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
    return {vx + m_xWheelYPos * omega / 1_rad,
            vy - m_yWheelXPos * omega / 1_rad, omega};
  }

 private:
  wpi::units::meter_t m_xWheelYPos;
  wpi::units::meter_t m_yWheelXPos;

  Rotation3d m_previousAngle;
  TwoDeadWheelPositions m_previousWheelPositions;
};
}  // namespace wpi::math
