// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Shooter.h"

#include <frc2/command/Commands.h>
#include <units/angle.h>
#include <units/voltage.h>

Shooter::Shooter() {
  m_shooterEncoder.SetDistancePerPulse(
      constants::shooter::kEncoderDistancePerPulse.value());
}

frc2::CommandPtr Shooter::RunShooterCommand(
    std::function<double()> shooterSpeed) {
  return frc2::cmd::Run(
             [this, shooterSpeed] {
               m_shooterMotor.SetVoltage(
                   units::volt_t{m_shooterFeedback.Calculate(
                       m_shooterEncoder.GetRate(), shooterSpeed())} +
                   m_shooterFeedforward.Calculate(
                       units::turns_per_second_t{shooterSpeed()}));
               m_feederMotor.Set(constants::shooter::kFeederSpeed);
             },
             {this})
      .WithName("Set Shooter Speed");
}

frc2::CommandPtr Shooter::SysIdQuasistatic(frc2::sysid::Direction direction) {
  return m_sysIdRoutine.Quasistatic(direction);
}

frc2::CommandPtr Shooter::SysIdDynamic(frc2::sysid::Direction direction) {
  return m_sysIdRoutine.Dynamic(direction);
}
