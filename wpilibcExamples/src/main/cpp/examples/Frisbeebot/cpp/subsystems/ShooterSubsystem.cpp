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
      m_shooterEncoder(kEncoderPorts[0], kEncoderPorts[1]),
      m_shooterFeedforward(kS, kV) {
  m_controller.SetTolerance(kShooterToleranceRPS.to<double>());
  m_shooterEncoder.SetDistancePerPulse(kEncoderDistancePerPulse);
  SetSetpoint(kShooterTargetRPS.to<double>());
}

void ShooterSubsystem::UseOutput(double output, double setpoint) {
  m_shooterMotor.SetVoltage(units::volt_t(output) +
                            m_shooterFeedforward.Calculate(kShooterTargetRPS));
}

bool ShooterSubsystem::AtSetpoint() { return m_controller.AtSetpoint(); }

double ShooterSubsystem::GetMeasurement() { return m_shooterEncoder.GetRate(); }

void ShooterSubsystem::RunFeeder() { m_feederMotor.Set(kFeederSpeed); }

void ShooterSubsystem::StopFeeder() { m_feederMotor.Set(0); }
