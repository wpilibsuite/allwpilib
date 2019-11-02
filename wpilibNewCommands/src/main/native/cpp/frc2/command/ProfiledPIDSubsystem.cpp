/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/ProfiledPIDSubsystem.h"

using namespace frc2;
using State = frc::TrapezoidProfile::State;

ProfiledPIDSubsystem::ProfiledPIDSubsystem(
    frc::ProfiledPIDController controller)
    : m_controller{controller} {}

void ProfiledPIDSubsystem::Periodic() {
  if (m_enabled) {
    UseOutput(m_controller.Calculate(GetMeasurement(), GetGoal()),
              m_controller.GetSetpoint());
  }
}

void ProfiledPIDSubsystem::Enable() {
  m_controller.Reset();
  m_enabled = true;
}

void ProfiledPIDSubsystem::Disable() {
  UseOutput(0, State{0_m, 0_mps});
  m_enabled = false;
}

frc::ProfiledPIDController& ProfiledPIDSubsystem::GetController() {
  return m_controller;
}
