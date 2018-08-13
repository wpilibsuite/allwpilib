/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/XboxController.h"

#include <hal/HAL.h>

using namespace frc;

XboxController::XboxController(int port) : GenericHID(port) {
  HAL_Report(HALUsageReporting::kResourceType_XboxController, port);
}

double XboxController::GetX(JoystickHand hand) const {
  if (hand == kLeftHand) {
    return GetRawAxis(0);
  } else {
    return GetRawAxis(4);
  }
}

double XboxController::GetY(JoystickHand hand) const {
  if (hand == kLeftHand) {
    return GetRawAxis(1);
  } else {
    return GetRawAxis(5);
  }
}

double XboxController::GetTriggerAxis(JoystickHand hand) const {
  if (hand == kLeftHand) {
    return GetRawAxis(2);
  } else {
    return GetRawAxis(3);
  }
}

bool XboxController::GetBumper(JoystickHand hand) const {
  if (hand == kLeftHand) {
    return GetRawButton(static_cast<int>(Button::kBumperLeft));
  } else {
    return GetRawButton(static_cast<int>(Button::kBumperRight));
  }
}

bool XboxController::GetBumperPressed(JoystickHand hand) {
  if (hand == kLeftHand) {
    return GetRawButtonPressed(static_cast<int>(Button::kBumperLeft));
  } else {
    return GetRawButtonPressed(static_cast<int>(Button::kBumperRight));
  }
}

bool XboxController::GetBumperReleased(JoystickHand hand) {
  if (hand == kLeftHand) {
    return GetRawButtonReleased(static_cast<int>(Button::kBumperLeft));
  } else {
    return GetRawButtonReleased(static_cast<int>(Button::kBumperRight));
  }
}

bool XboxController::GetStickButton(JoystickHand hand) const {
  if (hand == kLeftHand) {
    return GetRawButton(static_cast<int>(Button::kStickLeft));
  } else {
    return GetRawButton(static_cast<int>(Button::kStickRight));
  }
}

bool XboxController::GetStickButtonPressed(JoystickHand hand) {
  if (hand == kLeftHand) {
    return GetRawButtonPressed(static_cast<int>(Button::kStickLeft));
  } else {
    return GetRawButtonPressed(static_cast<int>(Button::kStickRight));
  }
}

bool XboxController::GetStickButtonReleased(JoystickHand hand) {
  if (hand == kLeftHand) {
    return GetRawButtonReleased(static_cast<int>(Button::kStickLeft));
  } else {
    return GetRawButtonReleased(static_cast<int>(Button::kStickRight));
  }
}

bool XboxController::GetAButton() const {
  return GetRawButton(static_cast<int>(Button::kA));
}

bool XboxController::GetAButtonPressed() {
  return GetRawButtonPressed(static_cast<int>(Button::kA));
}

bool XboxController::GetAButtonReleased() {
  return GetRawButtonReleased(static_cast<int>(Button::kA));
}

bool XboxController::GetBButton() const {
  return GetRawButton(static_cast<int>(Button::kB));
}

bool XboxController::GetBButtonPressed() {
  return GetRawButtonPressed(static_cast<int>(Button::kB));
}

bool XboxController::GetBButtonReleased() {
  return GetRawButtonReleased(static_cast<int>(Button::kB));
}

bool XboxController::GetXButton() const {
  return GetRawButton(static_cast<int>(Button::kX));
}

bool XboxController::GetXButtonPressed() {
  return GetRawButtonPressed(static_cast<int>(Button::kX));
}

bool XboxController::GetXButtonReleased() {
  return GetRawButtonReleased(static_cast<int>(Button::kX));
}

bool XboxController::GetYButton() const {
  return GetRawButton(static_cast<int>(Button::kY));
}

bool XboxController::GetYButtonPressed() {
  return GetRawButtonPressed(static_cast<int>(Button::kY));
}

bool XboxController::GetYButtonReleased() {
  return GetRawButtonReleased(static_cast<int>(Button::kY));
}

bool XboxController::GetBackButton() const {
  return GetRawButton(static_cast<int>(Button::kBack));
}

bool XboxController::GetBackButtonPressed() {
  return GetRawButtonPressed(static_cast<int>(Button::kBack));
}

bool XboxController::GetBackButtonReleased() {
  return GetRawButtonReleased(static_cast<int>(Button::kBack));
}

bool XboxController::GetStartButton() const {
  return GetRawButton(static_cast<int>(Button::kStart));
}

bool XboxController::GetStartButtonPressed() {
  return GetRawButtonPressed(static_cast<int>(Button::kStart));
}

bool XboxController::GetStartButtonReleased() {
  return GetRawButtonReleased(static_cast<int>(Button::kStart));
}
