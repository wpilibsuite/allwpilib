/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/PWMSpeedController.h"

using namespace frc;

void PWMSpeedController::Set(double speed) {
  SetSpeed(m_isInverted ? -speed : speed);
}

double PWMSpeedController::Get() const { return GetSpeed(); }

void PWMSpeedController::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool PWMSpeedController::GetInverted() const { return m_isInverted; }

void PWMSpeedController::Disable() { SetDisabled(); }

void PWMSpeedController::StopMotor() { PWMBase::StopMotor(); }

void PWMSpeedController::PIDWrite(double output) { Set(output); }

PWMSpeedController::PWMSpeedController(int channel) : PWMBase(channel) {}
