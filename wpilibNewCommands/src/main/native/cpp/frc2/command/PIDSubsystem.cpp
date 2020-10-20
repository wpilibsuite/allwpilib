/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/PIDSubsystem.h"

using namespace frc2;

PIDSubsystem::PIDSubsystem(PIDController controller, double initialPosition)
    : m_controller{controller} {
  SetSetpoint(initialPosition);
  AddChild("PID Controller", &m_controller);
}

void PIDSubsystem::Periodic() {
  if (m_enabled) {
    UseOutput(m_controller.Calculate(GetMeasurement(), m_setpoint), m_setpoint);
  }
}

void PIDSubsystem::SetSetpoint(double setpoint) { m_setpoint = setpoint; }

double PIDSubsystem::GetSetpoint() const { return m_setpoint; }

void PIDSubsystem::Enable() {
  m_controller.Reset();
  m_enabled = true;
}

void PIDSubsystem::Disable() {
  UseOutput(0, 0);
  m_enabled = false;
}

bool PIDSubsystem::IsEnabled() { return m_enabled; }

PIDController& PIDSubsystem::GetController() { return m_controller; }
