// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Shooter.h"

#include <frc2/command/Commands.h>

Shooter::Shooter() {
  m_shooterFeedback.SetTolerance(ShooterConstants::kShooterTolerance.value());
  m_shooterEncoder.SetDistancePerPulse(
      ShooterConstants::kEncoderDistancePerPulse);

  SetDefaultCommand(RunOnce([this] {
                      m_shooterMotor.Disable();
                      m_feederMotor.Disable();
                    })
                        .AndThen(Run([] {}))
                        .WithName("Idle"));
}

frc2::CommandPtr Shooter::ShootCommand(units::turns_per_second_t setpoint) {
  return frc2::cmd::Parallel(
             // Run the shooter flywheel at the desired setpoint using
             // feedforward and feedback
             Run([this, setpoint] {
               m_shooterMotor.SetVoltage(
                   m_shooterFeedforward.Calculate(setpoint) +
                   units::volt_t(m_shooterFeedback.Calculate(
                       m_shooterEncoder.GetRate(), setpoint.value())));
             }),
             // Wait until the shooter has reached the setpoint, and then
             // run the feeder
             frc2::cmd::WaitUntil([this] {
               return m_shooterFeedback.AtSetpoint();
             }).AndThen([this] { m_feederMotor.Set(1.0); }))
      .WithName("Shoot");
}
