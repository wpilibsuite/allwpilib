/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "XboxController.h"

#include "DriverStation.h"
#include "HAL/HAL.h"

using namespace frc;

/**
 * Construct an instance of an Xbox controller.
 *
 * The joystick index is the USB port on the Driver Station.
 *
 * @param port The port on the Driver Station that the joystick is plugged into
 *             (0-5).
 */
XboxController::XboxController(int port)
    : GamepadBase(port), m_ds(DriverStation::GetInstance()) {
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
 * Read the value of the bumper button on the controller.
 *
 * @param hand Side of controller whose value should be returned.
 */
bool XboxController::GetBumper(JoystickHand hand) const {
  if (hand == kLeftHand) {
    return GetRawButton(5);
  } else {
    return GetRawButton(6);
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
    return GetRawButton(9);
  } else {
    return GetRawButton(10);
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
 * Read the value of the A button on the controller.
 *
 * @param hand Side of controller whose value should be returned.
 * @return The state of the button.
 */
bool XboxController::GetAButton() const { return GetRawButton(1); }

/**
 * Read the value of the B button on the controller.
 *
 * @param hand Side of controller whose value should be returned.
 * @return The state of the button.
 */
bool XboxController::GetBButton() const { return GetRawButton(2); }

/**
 * Read the value of the X button on the controller.
 *
 * @param hand Side of controller whose value should be returned.
 * @return The state of the button.
 */
bool XboxController::GetXButton() const { return GetRawButton(3); }

/**
 * Read the value of the Y button on the controller.
 *
 * @param hand Side of controller whose value should be returned.
 * @return The state of the button.
 */
bool XboxController::GetYButton() const { return GetRawButton(4); }

/**
 * Read the value of the back button on the controller.
 *
 * @param hand Side of controller whose value should be returned.
 * @return The state of the button.
 */
bool XboxController::GetBackButton() const { return GetRawButton(7); }

/**
 * Read the value of the start button on the controller.
 *
 * @param hand Side of controller whose value should be returned.
 * @return The state of the button.
 */
bool XboxController::GetStartButton() const { return GetRawButton(8); }
