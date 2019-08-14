/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/PWM.h"

#include <utility>

#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

PWM::PWM(int channel) : PWMBase(channel) {
  SendableRegistry::GetInstance().AddLW(this, "PWM", channel);
}

void PWM::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("PWM");
  builder.SetActuator(true);
  builder.SetSafeState([=]() { SetDisabled(); });
  builder.AddDoubleProperty("Value", [=]() { return GetRaw(); },
                            [=](double value) { SetRaw(value); });
}
