/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/SpeedController.h"

#include <frc/RobotController.h>

using namespace frc;

void SpeedController::SetVoltage(units::volt_t output) {
  Set(output / units::volt_t(RobotController::GetInputVoltage()));
}
