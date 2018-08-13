/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/RobotState.h"

#include "frc/DriverStation.h"

using namespace frc;

bool RobotState::IsDisabled() {
  return DriverStation::GetInstance().IsDisabled();
}

bool RobotState::IsEnabled() {
  return DriverStation::GetInstance().IsEnabled();
}

bool RobotState::IsOperatorControl() {
  return DriverStation::GetInstance().IsOperatorControl();
}

bool RobotState::IsAutonomous() {
  return DriverStation::GetInstance().IsAutonomous();
}

bool RobotState::IsTest() { return DriverStation::GetInstance().IsTest(); }
