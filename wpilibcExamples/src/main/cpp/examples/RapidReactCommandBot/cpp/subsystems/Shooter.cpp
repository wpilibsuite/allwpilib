// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Shooter.hpp"

#include "wpi/commands2/Commands.hpp"

Shooter::Shooter() {
  shooterFeedback.SetTolerance(ShooterConstants::kShooterTolerance.value());
  shooterEncoder.SetDistancePerPulse(
      ShooterConstants::kEncoderDistancePerPulse);

  SetDefaultCommand(RunOnce([this] {
                      shooterMotor.Disable();
                      feederMotor.Disable();
                    })
                        .AndThen(Run([] {}))
                        .WithName("Idle"));
}

wpi::cmd::CommandPtr Shooter::ShootCommand(
    wpi::units::turns_per_second_t setpoint) {
  return wpi::cmd::Parallel(
             // Run the shooter flywheel at the desired setpoint using
             // feedforward and feedback
             Run([this, setpoint] {
               shooterMotor.SetVoltage(
                   shooterFeedforward.Calculate(setpoint) +
                   wpi::units::volt_t(shooterFeedback.Calculate(
                       shooterEncoder.GetRate(), setpoint.value())));
             }),
             // Wait until the shooter has reached the setpoint, and then
             // run the feeder
             wpi::cmd::WaitUntil([this] {
               return shooterFeedback.AtSetpoint();
             }).AndThen([this] { feederMotor.SetThrottle(1.0); }))
      .WithName("Shoot");
}
