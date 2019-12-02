/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/DriveSubsystem.h"

using namespace DriveConstants;

DriveSubsystem::DriveSubsystem()
    : m_leftMaster{kLeftMotor1Port},
      m_leftSlave{kLeftMotor2Port},
      m_rightMaster{kRightMotor1Port},
      m_rightSlave{kRightMotor2Port},
      m_feedforward{ks, kv, ka} {
  m_leftSlave.Follow(m_leftMaster);
  m_rightSlave.Follow(m_rightMaster);

  m_leftMaster.SetPID(kp, 0, 0);
  m_rightMaster.SetPID(kp, 0, 0);
}

void DriveSubsystem::Periodic() {
  // Implementation of subsystem periodic method goes here.
}

void DriveSubsystem::SetDriveStates(
    frc::TrapezoidProfile<units::meters>::State left,
    frc::TrapezoidProfile<units::meters>::State right) {
  m_leftMaster.SetSetpoint(ExampleSmartMotorController::PIDMode::kPosition,
                           left.position.to<double>(),
                           m_feedforward.Calculate(left.velocity).to<double>());
  m_rightMaster.SetSetpoint(
      ExampleSmartMotorController::PIDMode::kPosition,
      right.position.to<double>(),
      m_feedforward.Calculate(right.velocity).to<double>());
}

void DriveSubsystem::ArcadeDrive(double fwd, double rot) {
  m_drive.ArcadeDrive(fwd, rot);
}

void DriveSubsystem::ResetEncoders() {
  m_leftMaster.ResetEncoder();
  m_rightMaster.ResetEncoder();
}

units::meter_t DriveSubsystem::GetLeftEncoderDistance() {
  return units::meter_t(m_leftMaster.GetEncoderDistance());
}

units::meter_t DriveSubsystem::GetRightEncoderDistance() {
  return units::meter_t(m_rightMaster.GetEncoderDistance());
}

void DriveSubsystem::SetMaxOutput(double maxOutput) {
  m_drive.SetMaxOutput(maxOutput);
}
