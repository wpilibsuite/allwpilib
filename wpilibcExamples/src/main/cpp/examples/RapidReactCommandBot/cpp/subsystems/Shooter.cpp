// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Shooter.hpp"

#include "wpi/commands2/Commands.hpp"

Shooter::Shooter() {
  shooterFeedback.SetTolerance(ShooterConstants::SHOOTER_TOLERANCE.value());
  shooterEncoder.SetDistancePerPulse(
      ShooterConstants::ENCODER_DISTANCE_PER_PULSE);

  SetDefaultCommand(RunOnce([this] {
                      shooterMotor.SetThrottle(0.0);
                      feederMotor.SetThrottle(0.0);
                    })
                        .AndThen(Run([] {}))
                        .WithName("Idle"));
}

wpi::cmd::CommandPtr Shooter::ShootCommand(
    wpi::units::turns_per_second<> setpoint) {
  return wpi::cmd::Parallel(
             // Run the shooter flywheel at the desired setpoint using
             // feedforward and feedback
             Run([this, setpoint] {
               shooterMotor.SetVoltage(
                   shooterFeedforward.Calculate(setpoint) +
                   wpi::units::volts<>(shooterFeedback.Calculate(
                       shooterEncoder.GetRate(), setpoint.value())));
             }),
             // Wait until the shooter has reached the setpoint, and then
             // run the feeder
             wpi::cmd::WaitUntil([this] {
               return shooterFeedback.AtSetpoint();
             }).AndThen([this] { feederMotor.SetThrottle(1.0); }))
      .WithName("Shoot");
}
