/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/PWMSpeedController.h"

#include "frc/smartdashboard/SendableBuilder.h"

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

void PWMSpeedController::StopMotor() { PWM::StopMotor(); }

void PWMSpeedController::PIDWrite(double output) { Set(output); }

PWMSpeedController::PWMSpeedController(int channel) : PWM(channel) {}

void PWMSpeedController::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Speed Controller");
  builder.SetActuator(true);
  builder.SetSafeState([=]() { SetDisabled(); });
  builder.AddDoubleProperty("Value", [=]() { return GetSpeed(); },
                            [=](double value) { SetSpeed(value); });
}
