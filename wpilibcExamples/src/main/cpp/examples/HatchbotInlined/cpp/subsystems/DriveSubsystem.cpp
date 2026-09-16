// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/DriveSubsystem.hpp"

#include "wpi/telemetry/TelemetryTable.hpp"

using namespace DriveConstants;

DriveSubsystem::DriveSubsystem()
    : left1{LEFT_MOTOR1_PORT},
      left2{LEFT_MOTOR2_PORT},
      right1{RIGHT_MOTOR1_PORT},
      right2{RIGHT_MOTOR2_PORT},
      leftEncoder{LEFT_ENCODER_PORTS[0], LEFT_ENCODER_PORTS[1]},
      rightEncoder{RIGHT_ENCODER_PORTS[0], RIGHT_ENCODER_PORTS[1]} {
  left1.AddFollower(left2);
  right1.AddFollower(right2);

  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  right1.SetInverted(true);

  // Set the distance per pulse for the encoders
  leftEncoder.SetDistancePerPulse(ENCODER_DISTANCE_PER_PULSE);
  rightEncoder.SetDistancePerPulse(ENCODER_DISTANCE_PER_PULSE);
}

void DriveSubsystem::Periodic() {
  // Implementation of subsystem periodic method goes here.
}

void DriveSubsystem::ArcadeDrive(double fwd, double rot) {
  drive.ArcadeDrive(fwd, rot);
}

void DriveSubsystem::ResetEncoders() {
  leftEncoder.Reset();
  rightEncoder.Reset();
}

double DriveSubsystem::GetAverageEncoderDistance() {
  return (leftEncoder.GetDistance() + rightEncoder.GetDistance()) / 2.0;
}

void DriveSubsystem::SetMaxOutput(double maxOutput) {
  drive.SetMaxOutput(maxOutput);
}

void DriveSubsystem::LogTo(wpi::telemetry::TelemetryTable& table) const {
  SubsystemBase::LogTo(table);

  // Publish encoder distances to telemetry.
  table.Log("leftDistance", leftEncoder.GetDistance());
  table.Log("rightDistance", rightEncoder.GetDistance());
}
