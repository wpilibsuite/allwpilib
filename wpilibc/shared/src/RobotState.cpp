/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "RobotState.h"

#include "Base.h"

std::shared_ptr<frc::RobotStateInterface> frc::RobotState::impl;

void frc::RobotState::SetImplementation(RobotStateInterface& i) {
  impl = std::shared_ptr<RobotStateInterface>(
      &i, NullDeleter<RobotStateInterface>());
}

void frc::RobotState::SetImplementation(
    std::shared_ptr<RobotStateInterface> i) {
  impl = i;
}

bool frc::RobotState::IsDisabled() {
  if (impl != nullptr) {
    return impl->IsDisabled();
  }
  return true;
}

bool frc::RobotState::IsEnabled() {
  if (impl != nullptr) {
    return impl->IsEnabled();
  }
  return false;
}

bool frc::RobotState::IsOperatorControl() {
  if (impl != nullptr) {
    return impl->IsOperatorControl();
  }
  return true;
}

bool frc::RobotState::IsAutonomous() {
  if (impl != nullptr) {
    return impl->IsAutonomous();
  }
  return false;
}

bool frc::RobotState::IsTest() {
  if (impl != nullptr) {
    return impl->IsTest();
  }
  return false;
}
