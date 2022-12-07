// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/DriveSubsystem.h"

using namespace DriveConstants;

DriveSubsystem::DriveSubsystem()
    : m_leftLeader{kLeftMotor1Port},
      m_leftFollower{kLeftMotor2Port},
      m_rightLeader{kRightMotor1Port},
      m_rightFollower{kRightMotor2Port},
      m_feedforward{ks, kv, ka} {
  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  m_rightLeader.SetInverted(true);

  // You might need to not do this depending on the specific motor controller
  // that you are using -- contact the respective vendor's documentation for
  // more details.
  m_rightFollower.SetInverted(true);

  m_leftFollower.Follow(m_leftLeader);
  m_rightFollower.Follow(m_rightLeader);

  m_leftLeader.SetPID(kp, 0, 0);
  m_rightLeader.SetPID(kp, 0, 0);
}

void DriveSubsystem::Periodic() {
  // Implementation of subsystem periodic method goes here.
}

void DriveSubsystem::SetDriveStates(
    frc::TrapezoidProfile<units::meters>::State left,
    frc::TrapezoidProfile<units::meters>::State right) {
  m_leftLeader.SetSetpoint(ExampleSmartMotorController::PIDMode::kPosition,
                           left.position.value(),
                           m_feedforward.Calculate(left.velocity) / 12_V);
  m_rightLeader.SetSetpoint(ExampleSmartMotorController::PIDMode::kPosition,
                            right.position.value(),
                            m_feedforward.Calculate(right.velocity) / 12_V);
}

void DriveSubsystem::ArcadeDrive(double fwd, double rot) {
  m_drive.ArcadeDrive(fwd, rot);
}

void DriveSubsystem::ResetEncoders() {
  m_leftLeader.ResetEncoder();
  m_rightLeader.ResetEncoder();
}

units::meter_t DriveSubsystem::GetLeftEncoderDistance() {
  return units::meter_t{m_leftLeader.GetEncoderDistance()};
}

units::meter_t DriveSubsystem::GetRightEncoderDistance() {
  return units::meter_t{m_rightLeader.GetEncoderDistance()};
}

void DriveSubsystem::SetMaxOutput(double maxOutput) {
  m_drive.SetMaxOutput(maxOutput);
}
