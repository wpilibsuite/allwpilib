/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MockSpeedController.h"

using namespace frc;

void MockSpeedController::Set(double speed) {
  m_speed = m_isInverted ? -speed : speed;
}

double MockSpeedController::Get() const { return m_speed; }

void MockSpeedController::SetInverted(bool isInverted) {
  m_isInverted = isInverted;
}

bool MockSpeedController::GetInverted() const { return m_isInverted; }

void MockSpeedController::Disable() { m_speed = 0; }

void MockSpeedController::StopMotor() { Disable(); }

void MockSpeedController::PIDWrite(double output) { Set(output); }
