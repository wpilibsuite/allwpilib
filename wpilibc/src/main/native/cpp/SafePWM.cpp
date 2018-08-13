/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/SafePWM.h"

using namespace frc;

SafePWM::SafePWM(int channel) : PWM(channel) {
  m_safetyHelper = std::make_unique<MotorSafetyHelper>(this);
  m_safetyHelper->SetSafetyEnabled(false);
}

void SafePWM::SetExpiration(double timeout) {
  m_safetyHelper->SetExpiration(timeout);
}

double SafePWM::GetExpiration() const {
  return m_safetyHelper->GetExpiration();
}

bool SafePWM::IsAlive() const { return m_safetyHelper->IsAlive(); }

void SafePWM::StopMotor() { SetDisabled(); }

void SafePWM::SetSafetyEnabled(bool enabled) {
  m_safetyHelper->SetSafetyEnabled(enabled);
}

bool SafePWM::IsSafetyEnabled() const {
  return m_safetyHelper->IsSafetyEnabled();
}

void SafePWM::GetDescription(wpi::raw_ostream& desc) const {
  desc << "PWM " << GetChannel();
}

void SafePWM::SetSpeed(double speed) {
  PWM::SetSpeed(speed);
  m_safetyHelper->Feed();
}
