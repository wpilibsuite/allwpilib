// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/StadiaController.h"

#include <hal/FRCUsageReporting.h>

#include "frc/event/BooleanEvent.h"

using namespace frc;

StadiaController::StadiaController(int port) : GenericHID(port) {
  // re-enable when StadiaController is added to Usage Reporting
  // HAL_Report(HALUsageReporting::kResourceType_StadiaController, port + 1);
}

double StadiaController::GetLeftX() const {
  return GetRawAxis(Axis::kLeftX);
}

double StadiaController::GetRightX() const {
  return GetRawAxis(Axis::kRightX);
}

double StadiaController::GetLeftY() const {
  return GetRawAxis(Axis::kLeftY);
}

double StadiaController::GetRightY() const {
  return GetRawAxis(Axis::kRightY);
}

bool StadiaController::GetLeftBumper() const {
  return GetRawButton(Button::kLeftBumper);
}

bool StadiaController::GetRightBumper() const {
  return GetRawButton(Button::kRightBumper);
}

bool StadiaController::GetLeftBumperPressed() {
  return GetRawButtonPressed(Button::kLeftBumper);
}

bool StadiaController::GetRightBumperPressed() {
  return GetRawButtonPressed(Button::kRightBumper);
}

bool StadiaController::GetLeftBumperReleased() {
  return GetRawButtonReleased(Button::kLeftBumper);
}

bool StadiaController::GetRightBumperReleased() {
  return GetRawButtonReleased(Button::kRightBumper);
}

BooleanEvent StadiaController::LeftBumper(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetLeftBumper(); });
}

BooleanEvent StadiaController::RightBumper(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetRightBumper(); });
}

bool StadiaController::GetLeftStickButton() const {
  return GetRawButton(Button::kLeftStick);
}

bool StadiaController::GetRightStickButton() const {
  return GetRawButton(Button::kRightStick);
}

bool StadiaController::GetLeftStickButtonPressed() {
  return GetRawButtonPressed(Button::kLeftStick);
}

bool StadiaController::GetRightStickButtonPressed() {
  return GetRawButtonPressed(Button::kRightStick);
}

bool StadiaController::GetLeftStickButtonReleased() {
  return GetRawButtonReleased(Button::kLeftStick);
}

bool StadiaController::GetRightStickButtonReleased() {
  return GetRawButtonReleased(Button::kRightStick);
}

BooleanEvent StadiaController::LeftStick(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetLeftStickButton(); });
}

BooleanEvent StadiaController::RightStick(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetRightStickButton(); });
}

bool StadiaController::GetAButton() const {
  return GetRawButton(Button::kA);
}

bool StadiaController::GetAButtonPressed() {
  return GetRawButtonPressed(Button::kA);
}

bool StadiaController::GetAButtonReleased() {
  return GetRawButtonReleased(Button::kA);
}

BooleanEvent StadiaController::A(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetAButton(); });
}

bool StadiaController::GetBButton() const {
  return GetRawButton(Button::kB);
}

bool StadiaController::GetBButtonPressed() {
  return GetRawButtonPressed(Button::kB);
}

bool StadiaController::GetBButtonReleased() {
  return GetRawButtonReleased(Button::kB);
}

BooleanEvent StadiaController::B(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetBButton(); });
}

bool StadiaController::GetXButton() const {
  return GetRawButton(Button::kX);
}

bool StadiaController::GetXButtonPressed() {
  return GetRawButtonPressed(Button::kX);
}

bool StadiaController::GetXButtonReleased() {
  return GetRawButtonReleased(Button::kX);
}

BooleanEvent StadiaController::X(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetXButton(); });
}

bool StadiaController::GetYButton() const {
  return GetRawButton(Button::kY);
}

bool StadiaController::GetYButtonPressed() {
  return GetRawButtonPressed(Button::kY);
}

bool StadiaController::GetYButtonReleased() {
  return GetRawButtonReleased(Button::kY);
}

BooleanEvent StadiaController::Y(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetYButton(); });
}

bool StadiaController::GetEllipsesButton() const {
  return GetRawButton(Button::kEllipses);
}

bool StadiaController::GetEllipsesButtonPressed() {
  return GetRawButtonPressed(Button::kEllipses);
}

bool StadiaController::GetEllipsesButtonReleased() {
  return GetRawButtonReleased(Button::kEllipses);
}

BooleanEvent StadiaController::Ellipses(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetEllipsesButton(); });
}

bool StadiaController::GetHamburgerButton() const {
  return GetRawButton(Button::kHamburger);
}

bool StadiaController::GetHamburgerButtonPressed() {
  return GetRawButtonPressed(Button::kHamburger);
}

bool StadiaController::GetHamburgerButtonReleased() {
  return GetRawButtonReleased(Button::kHamburger);
}

BooleanEvent StadiaController::Hamburger(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetHamburgerButton(); });
}

bool StadiaController::GetStadiaButton() const {
  return GetRawButton(Button::kStadia);
}

bool StadiaController::GetStadiaButtonPressed() {
  return GetRawButtonPressed(Button::kStadia);
}

bool StadiaController::GetStadiaButtonReleased() {
  return GetRawButtonReleased(Button::kStadia);
}

BooleanEvent StadiaController::Stadia(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetStadiaButton(); });
}

bool StadiaController::GetGoogleButton() const {
  return GetRawButton(Button::kGoogle);
}

bool StadiaController::GetGoogleButtonPressed() {
  return GetRawButtonPressed(Button::kGoogle);
}

bool StadiaController::GetGoogleButtonReleased() {
  return GetRawButtonReleased(Button::kGoogle);
}

BooleanEvent StadiaController::Google(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetGoogleButton(); });
}

bool StadiaController::GetFrameButton() const {
  return GetRawButton(Button::kFrame);
}

bool StadiaController::GetFrameButtonPressed() {
  return GetRawButtonPressed(Button::kFrame);
}

bool StadiaController::GetFrameButtonReleased() {
  return GetRawButtonReleased(Button::kFrame);
}

BooleanEvent StadiaController::Frame(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetFrameButton(); });
}

bool StadiaController::GetLeftTriggerButton() const {
  return GetRawButton(Button::kLeftTrigger);
}

bool StadiaController::GetLeftTriggerButtonPressed() {
  return GetRawButtonPressed(Button::kLeftTrigger);
}

bool StadiaController::GetLeftTriggerButtonReleased() {
  return GetRawButtonReleased(Button::kLeftTrigger);
}

BooleanEvent StadiaController::LeftTrigger(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetLeftTriggerButton(); });
}

bool StadiaController::GetRightTriggerButton() const {
  return GetRawButton(Button::kRightTrigger);
}

bool StadiaController::GetRightTriggerButtonPressed() {
  return GetRawButtonPressed(Button::kRightTrigger);
}

bool StadiaController::GetRightTriggerButtonReleased() {
  return GetRawButtonReleased(Button::kRightTrigger);
}

BooleanEvent StadiaController::RightTrigger(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetRightTriggerButton(); });
}
