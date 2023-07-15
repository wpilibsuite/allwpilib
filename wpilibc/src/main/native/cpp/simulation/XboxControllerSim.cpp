// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/XboxControllerSim.h"

#include "frc/XboxController.h"

using namespace frc;
using namespace frc::sim;

XboxControllerSim::XboxControllerSim(const XboxController& joystick)
    : GenericHIDSim{joystick} {
  SetAxisCount(6);
  SetButtonCount(10);
  SetPOVCount(1);
}

XboxControllerSim::XboxControllerSim(int port) : GenericHIDSim{port} {
  SetAxisCount(6);
  SetButtonCount(10);
  SetPOVCount(1);
}

void XboxControllerSim::SetLeftX(double value) {
  SetRawAxis(XboxController::Axis::kLeftX, value);
}

void XboxControllerSim::SetRightX(double value) {
  SetRawAxis(XboxController::Axis::kRightX, value);
}

void XboxControllerSim::SetLeftY(double value) {
  SetRawAxis(XboxController::Axis::kLeftY, value);
}

void XboxControllerSim::SetRightY(double value) {
  SetRawAxis(XboxController::Axis::kRightY, value);
}

void XboxControllerSim::SetLeftTriggerAxis(double value) {
  SetRawAxis(XboxController::Axis::kLeftTrigger, value);
}

void XboxControllerSim::SetRightTriggerAxis(double value) {
  SetRawAxis(XboxController::Axis::kRightTrigger, value);
}

void XboxControllerSim::SetLeftBumper(bool state) {
  SetRawButton(XboxController::Button::kLeftBumper, state);
}

void XboxControllerSim::SetRightBumper(bool state) {
  SetRawButton(XboxController::Button::kRightBumper, state);
}

void XboxControllerSim::SetLeftStickButton(bool state) {
  SetRawButton(XboxController::Button::kLeftStick, state);
}

void XboxControllerSim::SetRightStickButton(bool state) {
  SetRawButton(XboxController::Button::kRightStick, state);
}

void XboxControllerSim::SetAButton(bool state) {
  SetRawButton(XboxController::Button::kA, state);
}

void XboxControllerSim::SetBButton(bool state) {
  SetRawButton(XboxController::Button::kB, state);
}

void XboxControllerSim::SetXButton(bool state) {
  SetRawButton(XboxController::Button::kX, state);
}

void XboxControllerSim::SetYButton(bool state) {
  SetRawButton(XboxController::Button::kY, state);
}

void XboxControllerSim::SetBackButton(bool state) {
  SetRawButton(XboxController::Button::kBack, state);
}

void XboxControllerSim::SetStartButton(bool state) {
  SetRawButton(XboxController::Button::kStart, state);
}
