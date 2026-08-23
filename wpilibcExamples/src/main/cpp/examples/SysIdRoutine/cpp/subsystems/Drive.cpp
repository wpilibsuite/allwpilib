// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Drive.hpp"

#include "wpi/commands2/Commands.hpp"

Drive::Drive() {
  leftMotor.AddFollower(wpi::PWMSparkMax{constants::drive::LEFT_MOTOR2_PORT});
  rightMotor.AddFollower(wpi::PWMSparkMax{constants::drive::RIGHT_MOTOR2_PORT});

  rightMotor.SetInverted(true);

  leftEncoder.SetDistancePerPulse(
      constants::drive::ENCODER_DISTANCE_PER_PULSE.value());
  rightEncoder.SetDistancePerPulse(
      constants::drive::ENCODER_DISTANCE_PER_PULSE.value());

  drive.SetSafetyEnabled(false);
}

wpi::cmd::CommandPtr Drive::ArcadeDriveCommand(std::function<double()> fwd,
                                               std::function<double()> rot) {
  return wpi::cmd::Run([this, fwd, rot] { drive.ArcadeDrive(fwd(), rot()); },
                       {this})
      .WithName("Arcade Drive");
}

wpi::cmd::CommandPtr Drive::SysIdQuasistatic(
    wpi::cmd::sysid::Direction direction) {
  return sysIdRoutine.Quasistatic(direction);
}

wpi::cmd::CommandPtr Drive::SysIdDynamic(wpi::cmd::sysid::Direction direction) {
  return sysIdRoutine.Dynamic(direction);
}
