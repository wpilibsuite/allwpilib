// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Rotation3d.hpp"
#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/kinematics/Odometry3d.hpp"
#include "wpi/math/kinematics/TwoDeadWheelPositions.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
class WPILIB_DLLEXPORT TwoDeadWheelOdometry3d {
 public:
  TwoDeadWheelOdometry3d(wpi::units::meter_t xWheelYPos,
                         wpi::units::meter_t yWheelXPos,
                         const Rotation3d& gyroAngle,
                         const TwoDeadWheelPositions& wheelPositions,
                         const Pose3d& initialPose = Pose3d{})
  : m_pose(initialPose),
    m_xWheelYPos(xWheelYPos),
    m_yWheelXPos(yWheelXPos),
    m_previousWheelPositions(wheelPositions) {
    m_previousAngle = m_pose.Rotation();
    m_gyroOffset = gyroAngle.Inverse().RotateBy(m_pose.Rotation());
  }

  void ResetPosition(const Rotation3d& gyroAngle,
                     const TwoDeadWheelPositions& wheelPositions,
                     const Pose3d& pose) {
    m_pose = pose;
    m_previousAngle = pose.Rotation();
    // When applied extrinsically, m_gyroOffset cancels the
    // current gyroAngle and then rotates to m_pose.Rotation()
    m_gyroOffset = gyroAngle.Inverse().RotateBy(m_pose.Rotation());
    m_previousWheelPositions = wheelPositions;
  }

  const Pose3d& Update(const Rotation3d& gyroAngle,
                       const TwoDeadWheelPositions& wheelPositions) {
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
   * @param vx The velocity of the forward-facing wheel.
   * @param vy The velocity of the sideways-facing wheel.
   * @param omega The angular velocity of the robot as reported by the gyro.
   * @return The velocity of the chassis.
   */
  constexpr ChassisVelocities ToChassisVelocities(
      wpi::units::meters_per_second_t vx, wpi::units::meters_per_second_t vy,
      wpi::units::radians_per_second_t omega) const {
    return {vx + m_xWheelYPos * omega / 1_rad,
            vy - m_yWheelXPos * omega / 1_rad, omega};
  }

 private:
  wpi::units::meter_t m_xWheelYPos;
  wpi::units::meter_t m_yWheelXPos;

  Pose3d m_pose;

  Rotation3d m_previousAngle;
  Rotation3d m_gyroOffset;
  TwoDeadWheelPositions m_previousWheelPositions;
};
}  // namespace wpi::math
