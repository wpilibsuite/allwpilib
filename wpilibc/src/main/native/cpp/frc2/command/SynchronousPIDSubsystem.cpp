/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/SynchronousPIDSubsystem.h"

using namespace frc2;

SynchronousPIDSubsystem::SynchronousPIDSubsystem(PIDController controller)
    : m_controller{controller} {}

void SynchronousPIDSubsystem::Periodic() {
  if (m_enabled) {
    UseOutput(m_controller.Calculate(GetMeasurement(), GetSetpoint()));
  }
}

void SynchronousPIDSubsystem::Enable() {
  m_controller.Reset();
  m_enabled = true;
}

void SynchronousPIDSubsystem::Disable() {
  UseOutput(0);
  m_enabled = false;
}

PIDController& SynchronousPIDSubsystem::GetController() { return m_controller; }
