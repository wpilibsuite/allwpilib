/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/DriveSubsystem.h"

using namespace DriveConstants;

DriveSubsystem::DriveSubsystem()
    : m_leftLeader{kLeftMotor1Port},
      m_leftFollower{kLeftMotor2Port},
      m_rightLeader{kRightMotor1Port},
      m_rightFollower{kRightMotor2Port},
      m_feedforward{ks, kv, ka} {
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
                           left.position.to<double>(),
                           m_feedforward.Calculate(left.velocity) / 12_V);
  m_rightLeader.SetSetpoint(ExampleSmartMotorController::PIDMode::kPosition,
                            right.position.to<double>(),
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
  return units::meter_t(m_leftLeader.GetEncoderDistance());
}

units::meter_t DriveSubsystem::GetRightEncoderDistance() {
  return units::meter_t(m_rightLeader.GetEncoderDistance());
}

void DriveSubsystem::SetMaxOutput(double maxOutput) {
  m_drive.SetMaxOutput(maxOutput);
}
