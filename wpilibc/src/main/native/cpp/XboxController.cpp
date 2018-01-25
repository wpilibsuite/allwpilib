/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "XboxController.h"

#include <HAL/HAL.h>

using namespace frc;

/**
 * Construct an instance of an Xbox controller.
 *
 * The controller index is the USB port on the Driver Station.
 *
 * @param port The port on the Driver Station that the controller is plugged
 *             into (0-5).
 */
XboxController::XboxController(int port) : GenericHID(port) {
  // HAL_Report(HALUsageReporting::kResourceType_XboxController, port);
  HAL_Report(HALUsageReporting::kResourceType_Joystick, port);
}

/**
 * Get the X axis value of the controller.
 *
 * @param hand Side of controller whose value should be returned.
 */
double XboxController::GetX(JoystickHand hand) const {
  if (hand == kLeftHand) {
    return GetRawAxis(0);
  } else {
    return GetRawAxis(4);
  }
}

/**
 * Get the Y axis value of the controller.
 *
 * @param hand Side of controller whose value should be returned.
 */
double XboxController::GetY(JoystickHand hand) const {
  if (hand == kLeftHand) {
    return GetRawAxis(1);
  } else {
    return GetRawAxis(5);
  }
}

/**
 * Get the trigger axis value of the controller.
 *
 * @param hand Side of controller whose value should be returned.
 */
double XboxController::GetTriggerAxis(JoystickHand hand) const {
  if (hand == kLeftHand) {
    return GetRawAxis(2);
  } else {
    return GetRawAxis(3);
  }
}

/**
 * Read the value of the bumper button on the controller.
 *
 * @param hand Side of controller whose value should be returned.
 */
bool XboxController::GetBumper(JoystickHand hand) const {
  if (hand == kLeftHand) {
    return GetRawButton(static_cast<int>(Button::kBumperLeft));
  } else {
    return GetRawButton(static_cast<int>(Button::kBumperRight));
  }
}

/**
 * Whether the bumper was pressed since the last check.
 *
 * @param hand Side of controller whose value should be returned.
 * @return Whether the button was pressed since the last check.
 */
bool XboxController::GetBumperPressed(JoystickHand hand) {
  if (hand == kLeftHand) {
    return GetRawButtonPressed(static_cast<int>(Button::kBumperLeft));
  } else {
    return GetRawButtonPressed(static_cast<int>(Button::kBumperRight));
  }
}

/**
 * Whether the bumper was released since the last check.
 *
 * @param hand Side of controller whose value should be returned.
 * @return Whether the button was released since the last check.
 */
bool XboxController::GetBumperReleased(JoystickHand hand) {
  if (hand == kLeftHand) {
    return GetRawButtonReleased(static_cast<int>(Button::kBumperLeft));
  } else {
    return GetRawButtonReleased(static_cast<int>(Button::kBumperRight));
  }
}

/**
 * Read the value of the stick button on the controller.
 *
 * @param hand Side of controller whose value should be returned.
 * @return The state of the button.
 */
bool XboxController::GetStickButton(JoystickHand hand) const {
  if (hand == kLeftHand) {
    return GetRawButton(static_cast<int>(Button::kStickLeft));
  } else {
    return GetRawButton(static_cast<int>(Button::kStickRight));
  }
}

/**
 * Whether the stick button was pressed since the last check.
 *
 * @param hand Side of controller whose value should be returned.
 * @return Whether the button was pressed since the last check.
 */
bool XboxController::GetStickButtonPressed(JoystickHand hand) {
  if (hand == kLeftHand) {
    return GetRawButtonPressed(static_cast<int>(Button::kStickLeft));
  } else {
    return GetRawButtonPressed(static_cast<int>(Button::kStickRight));
  }
}

/**
 * Whether the stick button was released since the last check.
 *
 * @param hand Side of controller whose value should be returned.
 * @return Whether the button was released since the last check.
 */
bool XboxController::GetStickButtonReleased(JoystickHand hand) {
  if (hand == kLeftHand) {
    return GetRawButtonReleased(static_cast<int>(Button::kStickLeft));
  } else {
    return GetRawButtonReleased(static_cast<int>(Button::kStickRight));
  }
}

/**
 * Read the value of the A button on the controller.
 *
 * @return The state of the button.
 */
bool XboxController::GetAButton() const {
  return GetRawButton(static_cast<int>(Button::kA));
}

/**
 * Whether the A button was pressed since the last check.
 *
 * @return Whether the button was pressed since the last check.
 */
bool XboxController::GetAButtonPressed() {
  return GetRawButtonPressed(static_cast<int>(Button::kA));
}

/**
 * Whether the A button was released since the last check.
 *
 * @return Whether the button was released since the last check.
 */
bool XboxController::GetAButtonReleased() {
  return GetRawButtonReleased(static_cast<int>(Button::kA));
}

/**
 * Read the value of the B button on the controller.
 *
 * @return The state of the button.
 */
bool XboxController::GetBButton() const {
  return GetRawButton(static_cast<int>(Button::kB));
}

/**
 * Whether the B button was pressed since the last check.
 *
 * @return Whether the button was pressed since the last check.
 */
bool XboxController::GetBButtonPressed() {
  return GetRawButtonPressed(static_cast<int>(Button::kB));
}

/**
 * Whether the B button was released since the last check.
 *
 * @return Whether the button was released since the last check.
 */
bool XboxController::GetBButtonReleased() {
  return GetRawButtonReleased(static_cast<int>(Button::kB));
}

/**
 * Read the value of the X button on the controller.
 *
 * @return The state of the button.
 */
bool XboxController::GetXButton() const {
  return GetRawButton(static_cast<int>(Button::kX));
}

/**
 * Whether the X button was pressed since the last check.
 *
 * @return Whether the button was pressed since the last check.
 */
bool XboxController::GetXButtonPressed() {
  return GetRawButtonPressed(static_cast<int>(Button::kX));
}

/**
 * Whether the X button was released since the last check.
 *
 * @return Whether the button was released since the last check.
 */
bool XboxController::GetXButtonReleased() {
  return GetRawButtonReleased(static_cast<int>(Button::kX));
}

/**
 * Read the value of the Y button on the controller.
 *
 * @return The state of the button.
 */
bool XboxController::GetYButton() const {
  return GetRawButton(static_cast<int>(Button::kY));
}

/**
 * Whether the Y button was pressed since the last check.
 *
 * @return Whether the button was pressed since the last check.
 */
bool XboxController::GetYButtonPressed() {
  return GetRawButtonPressed(static_cast<int>(Button::kY));
}

/**
 * Whether the Y button was released since the last check.
 *
 * @return Whether the button was released since the last check.
 */
bool XboxController::GetYButtonReleased() {
  return GetRawButtonReleased(static_cast<int>(Button::kY));
}

/**
 * Read the value of the back button on the controller.
 *
 * @param hand Side of controller whose value should be returned.
 * @return The state of the button.
 */
bool XboxController::GetBackButton() const {
  return GetRawButton(static_cast<int>(Button::kBack));
}

/**
 * Whether the back button was pressed since the last check.
 *
 * @return Whether the button was pressed since the last check.
 */
bool XboxController::GetBackButtonPressed() {
  return GetRawButtonPressed(static_cast<int>(Button::kBack));
}

/**
 * Whether the back button was released since the last check.
 *
 * @return Whether the button was released since the last check.
 */
bool XboxController::GetBackButtonReleased() {
  return GetRawButtonReleased(static_cast<int>(Button::kBack));
}

/**
 * Read the value of the start button on the controller.
 *
 * @param hand Side of controller whose value should be returned.
 * @return The state of the button.
 */
bool XboxController::GetStartButton() const {
  return GetRawButton(static_cast<int>(Button::kStart));
}

/**
 * Whether the start button was pressed since the last check.
 *
 * @return Whether the button was pressed since the last check.
 */
bool XboxController::GetStartButtonPressed() {
  return GetRawButtonPressed(static_cast<int>(Button::kStart));
}

/**
 * Whether the start button was released since the last check.
 *
 * @return Whether the button was released since the last check.
 */
bool XboxController::GetStartButtonReleased() {
  return GetRawButtonReleased(static_cast<int>(Button::kStart));
}
