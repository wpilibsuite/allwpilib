// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/ShooterSubsystem.h"

#include <frc/controller/PIDController.h>

#include "Constants.h"

using namespace ShooterConstants;

ShooterSubsystem::ShooterSubsystem()
    : PIDSubsystem{frc2::PIDController{kP, kI, kD}},
      m_shooterMotor(kShooterMotorPort),
      m_feederMotor(kFeederMotorPort),
      m_shooterEncoder(kEncoderPorts[0], kEncoderPorts[1]),
      m_shooterFeedforward(kS, kV) {
  m_controller.SetTolerance(kShooterToleranceRPS.value());
  m_shooterEncoder.SetDistancePerPulse(kEncoderDistancePerPulse);
  SetSetpoint(kShooterTargetRPS.value());
}

void ShooterSubsystem::UseOutput(double output, double setpoint) {
  m_shooterMotor.SetVoltage(units::volt_t{output} +
                            m_shooterFeedforward.Calculate(kShooterTargetRPS));
}

bool ShooterSubsystem::AtSetpoint() {
  return m_controller.AtSetpoint();
}

double ShooterSubsystem::GetMeasurement() {
  return m_shooterEncoder.GetRate();
}

void ShooterSubsystem::RunFeeder() {
  m_feederMotor.Set(kFeederSpeed);
}

void ShooterSubsystem::StopFeeder() {
  m_feederMotor.Set(0);
}
