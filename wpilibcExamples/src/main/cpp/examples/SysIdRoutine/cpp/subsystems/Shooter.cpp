// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Shooter.hpp"

#include "wpi/commands2/Commands.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/voltage.hpp"

Shooter::Shooter() {
  shooterEncoder.SetDistancePerPulse(
      constants::shooter::ENCODER_DISTANCE_PER_PULSE.value());
}

wpi::cmd::CommandPtr Shooter::RunShooterCommand(
    std::function<double()> shooterVelocity) {
  return wpi::cmd::Run(
             [this, shooterVelocity] {
               shooterMotor.SetVoltage(
                   wpi::units::volts<>{shooterFeedback.Calculate(
                       shooterEncoder.GetRate(), shooterVelocity())} +
                   shooterFeedforward.Calculate(
                       wpi::units::turns_per_second<>{shooterVelocity()}));
               feederMotor.SetThrottle(constants::shooter::FEEDER_VELOCITY);
             },
             {this})
      .WithName("Set Shooter Velocity");
}

wpi::cmd::CommandPtr Shooter::SysIdQuasistatic(
    wpi::cmd::sysid::Direction direction) {
  return sysIdRoutine.Quasistatic(direction);
}

wpi::cmd::CommandPtr Shooter::SysIdDynamic(
    wpi::cmd::sysid::Direction direction) {
  return sysIdRoutine.Dynamic(direction);
}
