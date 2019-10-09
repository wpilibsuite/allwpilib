/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/ShooterSubsystem.h"

#include <frc/controller/PIDController.h>

#include "Constants.h"

using namespace ShooterConstants;

ShooterSubsystem::ShooterSubsystem()
    : PIDSubsystem(frc2::PIDController(kP, kI, kD)),
      m_shooterMotor(kShooterMotorPort),
      m_feederMotor(kFeederMotorPort),
      m_shooterEncoder(kEncoderPorts[0], kEncoderPorts[1]) {
  m_controller.SetTolerance(kShooterToleranceRPS);
  m_shooterEncoder.SetDistancePerPulse(kEncoderDistancePerPulse);
}

void ShooterSubsystem::UseOutput(double output) {
  // Use a feedforward of the form kS + kV * velocity
  m_shooterMotor.Set(output + kSFractional + kVFractional * kShooterTargetRPS);
}

void ShooterSubsystem::Disable() {
  // Turn off motor when we disable, since useOutput(0) doesn't stop the motor
  // due to our feedforward
  frc2::PIDSubsystem::Disable();
  m_shooterMotor.Set(0);
}

bool ShooterSubsystem::AtSetpoint() { return m_controller.AtSetpoint(); }

double ShooterSubsystem::GetMeasurement() { return m_shooterEncoder.GetRate(); }

double ShooterSubsystem::GetSetpoint() { return kShooterTargetRPS; }

void ShooterSubsystem::RunFeeder() { m_feederMotor.Set(kFeederSpeed); }

void ShooterSubsystem::StopFeeder() { m_feederMotor.Set(0); }
