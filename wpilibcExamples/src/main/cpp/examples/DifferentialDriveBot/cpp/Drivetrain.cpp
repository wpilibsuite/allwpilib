/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Drivetrain.h"

frc::DifferentialDriveWheelSpeeds Drivetrain::GetSpeeds() const {
  return {units::meters_per_second_t(m_leftEncoder.GetRate()),
          units::meters_per_second_t(m_rightEncoder.GetRate())};
}

void Drivetrain::SetSpeeds(const frc::DifferentialDriveWheelSpeeds& speeds) {
  const auto leftOutput = m_leftPIDController.Calculate(
      m_leftEncoder.GetRate(), speeds.left.to<double>());
  const auto rightOutput = m_rightPIDController.Calculate(
      m_rightEncoder.GetRate(), speeds.right.to<double>());

  m_leftGroup.Set(leftOutput);
  m_rightGroup.Set(rightOutput);
}

void Drivetrain::Drive(units::meters_per_second_t xSpeed,
                       units::radians_per_second_t rot) {
  SetSpeeds(m_kinematics.ToWheelSpeeds({xSpeed, 0_mps, rot}));
}

void Drivetrain::UpdateOdometry() {
  m_odometry.Update(GetAngle(), GetSpeeds());
}
