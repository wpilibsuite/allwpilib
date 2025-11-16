// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Shooter.hpp"

#include "wpi/commands2/Commands.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/voltage.hpp"

Shooter::Shooter() {
  m_shooterEncoder.SetDistancePerPulse(
      constants::shooter::kEncoderDistancePerPulse.value());
}

wpi::cmd::CommandPtr Shooter::RunShooterCommand(
    std::function<double()> shooterSpeed) {
  return wpi::cmd::cmd::Run(
             [this, shooterSpeed] {
               m_shooterMotor.SetVoltage(
                   wpi::units::volt_t{m_shooterFeedback.Calculate(
                       m_shooterEncoder.GetRate(), shooterSpeed())} +
                   m_shooterFeedforward.Calculate(
                       wpi::units::turns_per_second_t{shooterSpeed()}));
               m_feederMotor.Set(constants::shooter::kFeederSpeed);
             },
             {this})
      .WithName("Set Shooter Speed");
}

wpi::cmd::CommandPtr Shooter::SysIdQuasistatic(
    wpi::cmd::sysid::Direction direction) {
  return m_sysIdRoutine.Quasistatic(direction);
}

wpi::cmd::CommandPtr Shooter::SysIdDynamic(
    wpi::cmd::sysid::Direction direction) {
  return m_sysIdRoutine.Dynamic(direction);
}
