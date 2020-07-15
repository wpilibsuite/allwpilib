/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/XboxControllerSim.h"

#include "frc/XboxController.h"

using namespace frc;
using namespace frc::sim;

XboxControllerSim::XboxControllerSim(const XboxController& joystick)
    : GenericHIDSim{joystick} {
  SetAxisCount(6);
  SetButtonCount(10);
}

XboxControllerSim::XboxControllerSim(int port) : GenericHIDSim{port} {
  SetAxisCount(6);
  SetButtonCount(10);
}

void XboxControllerSim::SetX(GenericHID::JoystickHand hand, double value) {
  if (hand == GenericHID::kLeftHand) {
    SetRawAxis(static_cast<int>(XboxController::Axis::kLeftX), value);
  } else {
    SetRawAxis(static_cast<int>(XboxController::Axis::kRightX), value);
  }
}

void XboxControllerSim::SetY(GenericHID::JoystickHand hand, double value) {
  if (hand == GenericHID::kLeftHand) {
    SetRawAxis(static_cast<int>(XboxController::Axis::kLeftY), value);
  } else {
    SetRawAxis(static_cast<int>(XboxController::Axis::kRightY), value);
  }
}

void XboxControllerSim::SetTriggerAxis(GenericHID::JoystickHand hand,
                                       double value) {
  if (hand == GenericHID::kLeftHand) {
    SetRawAxis(static_cast<int>(XboxController::Axis::kLeftTrigger), value);
  } else {
    SetRawAxis(static_cast<int>(XboxController::Axis::kRightTrigger), value);
  }
}

void XboxControllerSim::SetBumper(GenericHID::JoystickHand hand, bool state) {
  if (hand == GenericHID::kLeftHand) {
    SetRawButton(static_cast<int>(XboxController::Button::kBumperLeft), state);
  } else {
    SetRawButton(static_cast<int>(XboxController::Button::kBumperRight), state);
  }
}

void XboxControllerSim::SetStickButton(GenericHID::JoystickHand hand,
                                       bool state) {
  if (hand == GenericHID::kLeftHand) {
    SetRawButton(static_cast<int>(XboxController::Button::kStickLeft), state);
  } else {
    SetRawButton(static_cast<int>(XboxController::Button::kStickRight), state);
  }
}

void XboxControllerSim::SetAButton(bool state) {
  SetRawButton(static_cast<int>(XboxController::Button::kA), state);
}

void XboxControllerSim::SetBButton(bool state) {
  SetRawButton(static_cast<int>(XboxController::Button::kB), state);
}

void XboxControllerSim::SetXButton(bool state) {
  SetRawButton(static_cast<int>(XboxController::Button::kX), state);
}

void XboxControllerSim::SetYButton(bool state) {
  SetRawButton(static_cast<int>(XboxController::Button::kY), state);
}

void XboxControllerSim::SetBackButton(bool state) {
  SetRawButton(static_cast<int>(XboxController::Button::kBack), state);
}

void XboxControllerSim::SetStartButton(bool state) {
  SetRawButton(static_cast<int>(XboxController::Button::kStart), state);
}
