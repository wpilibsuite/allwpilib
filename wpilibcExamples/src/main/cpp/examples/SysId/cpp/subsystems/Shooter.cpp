// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Shooter.h"

#include <frc2/command/Commands.h>

Shooter::Shooter() {
  m_leftEncoder.SetDistancePerPulse(
      constants::shooter::kEncoderDistancePerPulse.value());
}

frc2::CommandPtr Drive::ArcadeDriveCommand(std::function<double()> shooterSpeed,
                                           std::function<double()> rot) {
  return frc2::cmd::Run([this, shooterSpeed] { m_shooterMotor.SetVoltage(shooterSpeed()); },
                        {this})
      .WithName("Set Shooter Speed");
}

frc2::CommandPtr Drive::SysIdQuasistatic(frc2::sysid::Direction direction) {
  return m_sysIdRoutine.Quasistatic(direction);
}

frc2::CommandPtr Drive::SysIdDynamic(frc2::sysid::Direction direction) {
  return m_sysIdRoutine.Dynamic(direction);
}
