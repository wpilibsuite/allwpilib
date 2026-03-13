// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.hpp"

void Drivetrain::Drive(wpi::units::meters_per_second_t xVelocity,
                       wpi::units::meters_per_second_t yVelocity,
                       wpi::units::radians_per_second_t rot, bool fieldRelative,
                       wpi::units::second_t period) {
  wpi::math::ChassisVelocities chassisVelocities{xVelocity, yVelocity, rot};
  if (fieldRelative) {
    chassisVelocities =
        chassisVelocities.ToRobotRelative(m_imu.GetRotation2d());
  }
  chassisVelocities = chassisVelocities.Discretize(period);

  auto states = m_kinematics.ToSwerveModuleVelocities(chassisVelocities);
  m_kinematics.DesaturateWheelVelocities(&states, kMaxVelocity);

  auto [fl, fr, bl, br] = states;
  m_frontLeft.SetDesiredVelocity(fl);
  m_frontRight.SetDesiredVelocity(fr);
  m_backLeft.SetDesiredVelocity(bl);
  m_backRight.SetDesiredVelocity(br);
}

void Drivetrain::UpdateOdometry() {
  m_odometry.Update(m_imu.GetRotation2d(),
                    {m_frontLeft.GetPosition(), m_frontRight.GetPosition(),
                     m_backLeft.GetPosition(), m_backRight.GetPosition()});
}
