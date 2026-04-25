// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/DriveSubsystem.hpp"

#include "wpi/util/sendable/SendableBuilder.hpp"

using namespace DriveConstants;

DriveSubsystem::DriveSubsystem()
    : left1{kLeftMotor1Port},
      left2{kLeftMotor2Port},
      right1{kRightMotor1Port},
      right2{kRightMotor2Port},
      leftEncoder{kLeftEncoderPorts[0], kLeftEncoderPorts[1]},
      rightEncoder{kRightEncoderPorts[0], kRightEncoderPorts[1]} {
  wpi::util::SendableRegistry::AddChild(&drive, &left1);
  wpi::util::SendableRegistry::AddChild(&drive, &right1);

  left1.AddFollower(left2);
  right1.AddFollower(right2);

  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  right1.SetInverted(true);

  // Set the distance per pulse for the encoders
  leftEncoder.SetDistancePerPulse(kEncoderDistancePerPulse);
  rightEncoder.SetDistancePerPulse(kEncoderDistancePerPulse);
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

void DriveSubsystem::InitSendable(wpi::util::SendableBuilder& builder) {
  SubsystemBase::InitSendable(builder);

  // Publish encoder distances to telemetry.
  builder.AddDoubleProperty(
      "leftDistance", [this] { return leftEncoder.GetDistance(); }, nullptr);
  builder.AddDoubleProperty(
      "rightDistance", [this] { return rightEncoder.GetDistance(); }, nullptr);
}
