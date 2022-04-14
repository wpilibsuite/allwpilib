// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/XboxController.h"

#include <hal/FRCUsageReporting.h>

using namespace frc;

XboxController::XboxController(int port) : GenericHID(port) {
  HAL_Report(HALUsageReporting::kResourceType_XboxController, port + 1);
}

double XboxController::GetLeftX() const {
  return GetRawAxis(Axis::kLeftX);
}

double XboxController::GetRightX() const {
  return GetRawAxis(Axis::kRightX);
}

double XboxController::GetLeftY() const {
  return GetRawAxis(Axis::kLeftY);
}

double XboxController::GetRightY() const {
  return GetRawAxis(Axis::kRightY);
}

double XboxController::GetLeftTriggerAxis() const {
  return GetRawAxis(Axis::kLeftTrigger);
}

double XboxController::GetRightTriggerAxis() const {
  return GetRawAxis(Axis::kRightTrigger);
}

bool XboxController::GetLeftBumper() const {
  return GetRawButton(Button::kLeftBumper);
}

bool XboxController::GetRightBumper() const {
  return GetRawButton(Button::kRightBumper);
}

bool XboxController::GetLeftBumperPressed() {
  return GetRawButtonPressed(Button::kLeftBumper);
}

bool XboxController::GetRightBumperPressed() {
  return GetRawButtonPressed(Button::kRightBumper);
}

bool XboxController::GetLeftBumperReleased() {
  return GetRawButtonReleased(Button::kLeftBumper);
}

bool XboxController::GetRightBumperReleased() {
  return GetRawButtonReleased(Button::kRightBumper);
}

bool XboxController::GetLeftStickButton() const {
  return GetRawButton(Button::kLeftStick);
}

bool XboxController::GetRightStickButton() const {
  return GetRawButton(Button::kRightStick);
}

bool XboxController::GetLeftStickButtonPressed() {
  return GetRawButtonPressed(Button::kLeftStick);
}

bool XboxController::GetRightStickButtonPressed() {
  return GetRawButtonPressed(Button::kRightStick);
}

bool XboxController::GetLeftStickButtonReleased() {
  return GetRawButtonReleased(Button::kLeftStick);
}

bool XboxController::GetRightStickButtonReleased() {
  return GetRawButtonReleased(Button::kRightStick);
}

bool XboxController::GetAButton() const {
  return GetRawButton(Button::kA);
}

bool XboxController::GetAButtonPressed() {
  return GetRawButtonPressed(Button::kA);
}

bool XboxController::GetAButtonReleased() {
  return GetRawButtonReleased(Button::kA);
}

bool XboxController::GetBButton() const {
  return GetRawButton(Button::kB);
}

bool XboxController::GetBButtonPressed() {
  return GetRawButtonPressed(Button::kB);
}

bool XboxController::GetBButtonReleased() {
  return GetRawButtonReleased(Button::kB);
}

bool XboxController::GetXButton() const {
  return GetRawButton(Button::kX);
}

bool XboxController::GetXButtonPressed() {
  return GetRawButtonPressed(Button::kX);
}

bool XboxController::GetXButtonReleased() {
  return GetRawButtonReleased(Button::kX);
}

bool XboxController::GetYButton() const {
  return GetRawButton(Button::kY);
}

bool XboxController::GetYButtonPressed() {
  return GetRawButtonPressed(Button::kY);
}

bool XboxController::GetYButtonReleased() {
  return GetRawButtonReleased(Button::kY);
}

bool XboxController::GetBackButton() const {
  return GetRawButton(Button::kBack);
}

bool XboxController::GetBackButtonPressed() {
  return GetRawButtonPressed(Button::kBack);
}

bool XboxController::GetBackButtonReleased() {
  return GetRawButtonReleased(Button::kBack);
}

bool XboxController::GetStartButton() const {
  return GetRawButton(Button::kStart);
}

bool XboxController::GetStartButtonPressed() {
  return GetRawButtonPressed(Button::kStart);
}

bool XboxController::GetStartButtonReleased() {
  return GetRawButtonReleased(Button::kStart);
}
