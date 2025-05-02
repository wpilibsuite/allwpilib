// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Drive.h"

#include <frc2/command/Commands.h>

Drive::Drive() {
  m_leftMotor.AddFollower(frc::PWMSparkMax{constants::drive::LEFT_MOTOR_2_PORT});
  m_rightMotor.AddFollower(
      frc::PWMSparkMax{constants::drive::RIGHT_MOTOR_2_PORT});

  m_rightMotor.SetInverted(true);

  m_leftEncoder.SetDistancePerPulse(
      constants::drive::ENCODER_DISTANCE_PER_PULSE.value());
  m_rightEncoder.SetDistancePerPulse(
      constants::drive::ENCODER_DISTANCE_PER_PULSE.value());

  m_drive.SetSafetyEnabled(false);
}

frc2::CommandPtr Drive::ArcadeDriveCommand(std::function<double()> fwd,
                                           std::function<double()> rot) {
  return frc2::cmd::Run([this, fwd, rot] { m_drive.ArcadeDrive(fwd(), rot()); },
                        {this})
      .WithName("Arcade Drive");
}

frc2::CommandPtr Drive::SysIdQuasistatic(frc2::sysid::Direction direction) {
  return m_sysIdRoutine.Quasistatic(direction);
}

frc2::CommandPtr Drive::SysIdDynamic(frc2::sysid::Direction direction) {
  return m_sysIdRoutine.Dynamic(direction);
}
