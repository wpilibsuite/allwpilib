// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/GamepadSim.hpp"

#include "wpi/driverstation/Gamepad.hpp"

using namespace wpi;
using namespace wpi::sim;

GamepadSim::GamepadSim(const wpi::Gamepad& joystick) : GenericHIDSim{joystick} {
  SetAxesMaximumIndex(6);
  SetButtonsMaximumIndex(26);
  SetPOVsMaximumIndex(1);
}

GamepadSim::GamepadSim(int port) : GenericHIDSim{port} {
  SetAxesMaximumIndex(6);
  SetButtonsMaximumIndex(26);
  SetPOVsMaximumIndex(1);
}

void GamepadSim::SetButton(wpi::Gamepad::Button button, bool value) {
  SetRawButton(static_cast<int>(button), value);
}

void GamepadSim::SetAxis(wpi::Gamepad::Axis axis, double value) {
  SetRawAxis(static_cast<int>(axis), value);
}

void GamepadSim::SetLeftX(double value) {
  SetAxis(Gamepad::Axis::LEFT_X, value);
}

void GamepadSim::SetLeftY(double value) {
  SetAxis(Gamepad::Axis::LEFT_Y, value);
}

void GamepadSim::SetRightX(double value) {
  SetAxis(Gamepad::Axis::RIGHT_X, value);
}

void GamepadSim::SetRightY(double value) {
  SetAxis(Gamepad::Axis::RIGHT_Y, value);
}

void GamepadSim::SetLeftTriggerAxis(double value) {
  SetAxis(Gamepad::Axis::LEFT_TRIGGER, value);
}

void GamepadSim::SetRightTriggerAxis(double value) {
  SetAxis(Gamepad::Axis::RIGHT_TRIGGER, value);
}

void GamepadSim::SetSouthFaceButton(bool value) {
  SetButton(Gamepad::Button::SOUTH_FACE, value);
}

void GamepadSim::SetEastFaceButton(bool value) {
  SetButton(Gamepad::Button::EAST_FACE, value);
}

void GamepadSim::SetWestFaceButton(bool value) {
  SetButton(Gamepad::Button::WEST_FACE, value);
}

void GamepadSim::SetNorthFaceButton(bool value) {
  SetButton(Gamepad::Button::NORTH_FACE, value);
}

void GamepadSim::SetBackButton(bool value) {
  SetButton(Gamepad::Button::BACK, value);
}

void GamepadSim::SetGuideButton(bool value) {
  SetButton(Gamepad::Button::GUIDE, value);
}

void GamepadSim::SetStartButton(bool value) {
  SetButton(Gamepad::Button::START, value);
}

void GamepadSim::SetLeftStickButton(bool value) {
  SetButton(Gamepad::Button::LEFT_STICK, value);
}

void GamepadSim::SetRightStickButton(bool value) {
  SetButton(Gamepad::Button::RIGHT_STICK, value);
}

void GamepadSim::SetLeftBumperButton(bool value) {
  SetButton(Gamepad::Button::LEFT_BUMPER, value);
}

void GamepadSim::SetRightBumperButton(bool value) {
  SetButton(Gamepad::Button::RIGHT_BUMPER, value);
}

void GamepadSim::SetDpadUpButton(bool value) {
  SetButton(Gamepad::Button::DPAD_UP, value);
}

void GamepadSim::SetDpadDownButton(bool value) {
  SetButton(Gamepad::Button::DPAD_DOWN, value);
}

void GamepadSim::SetDpadLeftButton(bool value) {
  SetButton(Gamepad::Button::DPAD_LEFT, value);
}

void GamepadSim::SetDpadRightButton(bool value) {
  SetButton(Gamepad::Button::DPAD_RIGHT, value);
}

void GamepadSim::SetMisc1Button(bool value) {
  SetButton(Gamepad::Button::MISC_1, value);
}

void GamepadSim::SetRightPaddle1Button(bool value) {
  SetButton(Gamepad::Button::RIGHT_PADDLE_1, value);
}

void GamepadSim::SetLeftPaddle1Button(bool value) {
  SetButton(Gamepad::Button::LEFT_PADDLE_1, value);
}

void GamepadSim::SetRightPaddle2Button(bool value) {
  SetButton(Gamepad::Button::RIGHT_PADDLE_2, value);
}

void GamepadSim::SetLeftPaddle2Button(bool value) {
  SetButton(Gamepad::Button::LEFT_PADDLE_2, value);
}

void GamepadSim::SetTouchpadButton(bool value) {
  SetButton(Gamepad::Button::TOUCHPAD, value);
}

void GamepadSim::SetMisc2Button(bool value) {
  SetButton(Gamepad::Button::MISC_2, value);
}

void GamepadSim::SetMisc3Button(bool value) {
  SetButton(Gamepad::Button::MISC_3, value);
}

void GamepadSim::SetMisc4Button(bool value) {
  SetButton(Gamepad::Button::MISC_4, value);
}

void GamepadSim::SetMisc5Button(bool value) {
  SetButton(Gamepad::Button::MISC_5, value);
}

void GamepadSim::SetMisc6Button(bool value) {
  SetButton(Gamepad::Button::MISC_6, value);
}
