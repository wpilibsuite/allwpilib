// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/geometry/Rotation3d.hpp"
#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/kinematics/Odometry3d.hpp"
#include "wpi/math/kinematics/ThreeDeadWheelPositions.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
class WPILIB_DLLEXPORT ThreeDeadWheelOdometry3d
    : public Odometry3d<ThreeDeadWheelPositions> {
 public:
  ThreeDeadWheelOdometry3d(
      wpi::units::meter_t x1WheelYPos, wpi::units::meter_t x2WheelYPos,
      wpi::units::meter_t yWheelXPos, const Rotation3d& gyroAngle,
      const ThreeDeadWheelPositions& wheelPositions,
      const Pose3d& initialPose = Pose3d{})
      : Odometry3d(gyroAngle, initialPose),
        m_x1WheelYPos(x1WheelYPos),
        m_x2WheelYPos(x2WheelYPos),
        m_yWheelXPos(yWheelXPos),
        m_previousWheelPositions(wheelPositions) {
    if (units::math::abs(m_x2WheelYPos - m_x1WheelYPos) < 0.1_m) {
      throw std::domain_error(
          "x1WheelYPos and x2WheelYPos must not be too close to each other");
    }
  }

  void ResetPosition(const Rotation3d& gyroAngle,
                     const ThreeDeadWheelPositions& wheelPositions,
                     const Pose3d& pose) override {
    m_previousWheelPositions = wheelPositions;
    Odometry3d::ResetPosition(gyroAngle, pose);
  }

  const Pose3d& Update(
      const Rotation3d& gyroAngle,
      const ThreeDeadWheelPositions& wheelPositions) override {
    auto deltaXWheel1 = wheelPositions.x1 - m_previousWheelPositions.x1;
    auto deltaXWheel2 = wheelPositions.x2 - m_previousWheelPositions.x2;
    auto deltaYWheel = wheelPositions.y - m_previousWheelPositions.y;

    auto deltaTheta =
        (deltaXWheel1 - deltaXWheel2) / (m_x2WheelYPos - m_x1WheelYPos);
    auto deltaX = (deltaXWheel1 + deltaXWheel2) / 2.0;
    auto deltaY = deltaYWheel - m_yWheelXPos * deltaTheta;
    m_previousWheelPositions = wheelPositions;
    return Odometry3d::Update(gyroAngle, {deltaX, deltaY, deltaTheta * 1_rad});
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
