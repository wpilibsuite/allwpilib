// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Drive.hpp"

#include <wpi/commands2/Commands.hpp>

Drive::Drive() {
  m_leftMotor.AddFollower(wpi::PWMSparkMax{constants::drive::kLeftMotor2Port});
  m_rightMotor.AddFollower(
      wpi::PWMSparkMax{constants::drive::kRightMotor2Port});

  m_rightMotor.SetInverted(true);

  m_leftEncoder.SetDistancePerPulse(
      constants::drive::kEncoderDistancePerPulse.value());
  m_rightEncoder.SetDistancePerPulse(
      constants::drive::kEncoderDistancePerPulse.value());

  m_drive.SetSafetyEnabled(false);
}

wpi::cmd::CommandPtr Drive::ArcadeDriveCommand(std::function<double()> fwd,
                                           std::function<double()> rot) {
  return wpi::cmd::cmd::Run([this, fwd, rot] { m_drive.ArcadeDrive(fwd(), rot()); },
                        {this})
      .WithName("Arcade Drive");
}

wpi::cmd::CommandPtr Drive::SysIdQuasistatic(wpi::cmd::sysid::Direction direction) {
  return m_sysIdRoutine.Quasistatic(direction);
}

wpi::cmd::CommandPtr Drive::SysIdDynamic(wpi::cmd::sysid::Direction direction) {
  return m_sysIdRoutine.Dynamic(direction);
}
