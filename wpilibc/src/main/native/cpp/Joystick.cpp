/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Joystick.h"

#include <cmath>

#include "DriverStation.h"
#include "HAL/HAL.h"
#include "WPIErrors.h"

using namespace frc;

const int Joystick::kDefaultXAxis;
const int Joystick::kDefaultYAxis;
const int Joystick::kDefaultZAxis;
const int Joystick::kDefaultTwistAxis;
const int Joystick::kDefaultThrottleAxis;
const int Joystick::kDefaultTriggerButton;
const int Joystick::kDefaultTopButton;
static Joystick* joysticks[DriverStation::kJoystickPorts];
static bool joySticksInitialized = false;

/**
 * Construct an instance of a joystick.
 *
 * The joystick index is the USB port on the Driver Station.
 *
 * @param port The port on the Driver Station that the joystick is plugged into
 *             (0-5).
 */
Joystick::Joystick(int port) : Joystick(port, kNumAxisTypes, kNumButtonTypes) {
  m_axes[kXAxis] = kDefaultXAxis;
  m_axes[kYAxis] = kDefaultYAxis;
  m_axes[kZAxis] = kDefaultZAxis;
  m_axes[kTwistAxis] = kDefaultTwistAxis;
  m_axes[kThrottleAxis] = kDefaultThrottleAxis;

  m_buttons[kTriggerButton] = kDefaultTriggerButton;
  m_buttons[kTopButton] = kDefaultTopButton;

  HAL_Report(HALUsageReporting::kResourceType_Joystick, port);
}

/**
 * Version of the constructor to be called by sub-classes.
 *
 * This constructor allows the subclass to configure the number of constants
 * for axes and buttons.
 *
 * @param port           The port on the Driver Station that the joystick is
 *                       plugged into.
 * @param numAxisTypes   The number of axis types in the enum.
 * @param numButtonTypes The number of button types in the enum.
 */
Joystick::Joystick(int port, int numAxisTypes, int numButtonTypes)
    : JoystickBase(port),
      m_ds(DriverStation::GetInstance()),
      m_axes(numAxisTypes),
      m_buttons(numButtonTypes) {
  if (!joySticksInitialized) {
    for (auto& joystick : joysticks) joystick = nullptr;
    joySticksInitialized = true;
  }
  if (GetPort() >= DriverStation::kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
  } else {
    joysticks[GetPort()] = this;
  }
}

Joystick* Joystick::GetStickForPort(int port) {
  Joystick* stick = joysticks[port];
  if (stick == nullptr) {
    stick = new Joystick(port);
    joysticks[port] = stick;
  }
  return stick;
}

/**
 * Get the X value of the joystick.
 *
 * This depends on the mapping of the joystick connected to the current port.
 *
 * @param hand This parameter is ignored for the Joystick class and is only
 *             here to complete the GenericHID interface.
 */
double Joystick::GetX(JoystickHand hand) const {
  return GetRawAxis(m_axes[kXAxis]);
}

/**
 * Get the Y value of the joystick.
 *
 * This depends on the mapping of the joystick connected to the current port.
 *
 * @param hand This parameter is ignored for the Joystick class and is only
 *             here to complete the GenericHID interface.
 */
double Joystick::GetY(JoystickHand hand) const {
  return GetRawAxis(m_axes[kYAxis]);
}

/**
 * Get the Z value of the current joystick.
 *
 * This depends on the mapping of the joystick connected to the current port.
 */
double Joystick::GetZ(JoystickHand hand) const {
  return GetRawAxis(m_axes[kZAxis]);
}

/**
 * Get the twist value of the current joystick.
 *
 * This depends on the mapping of the joystick connected to the current port.
 */
double Joystick::GetTwist() const { return GetRawAxis(m_axes[kTwistAxis]); }

/**
 * Get the throttle value of the current joystick.
 *
 * This depends on the mapping of the joystick connected to the current port.
 */
double Joystick::GetThrottle() const {
  return GetRawAxis(m_axes[kThrottleAxis]);
}

/**
 * For the current joystick, return the axis determined by the argument.
 *
 * This is for cases where the joystick axis is returned programatically,
 * otherwise one of the previous functions would be preferable (for example
 * GetX()).
 *
 * @param axis The axis to read.
 * @return The value of the axis.
 */
double Joystick::GetAxis(AxisType axis) const {
  switch (axis) {
    case kXAxis:
      return this->GetX();
    case kYAxis:
      return this->GetY();
    case kZAxis:
      return this->GetZ();
    case kTwistAxis:
      return this->GetTwist();
    case kThrottleAxis:
      return this->GetThrottle();
    default:
      wpi_setWPIError(BadJoystickAxis);
      return 0.0;
  }
}

/**
 * Read the state of the trigger on the joystick.
 *
 * Look up which button has been assigned to the trigger and read its state.
 *
 * @param hand This parameter is ignored for the Joystick class and is only
 *             here to complete the GenericHID interface.
 * @return The state of the trigger.
 */
bool Joystick::GetTrigger(JoystickHand hand) const {
  return GetRawButton(m_buttons[kTriggerButton]);
}

/**
 * Read the state of the top button on the joystick.
 *
 * Look up which button has been assigned to the top and read its state.
 *
 * @param hand This parameter is ignored for the Joystick class and is only
 *             here to complete the GenericHID interface.
 * @return The state of the top button.
 */
bool Joystick::GetTop(JoystickHand hand) const {
  return GetRawButton(m_buttons[kTopButton]);
}

/**
 * Get buttons based on an enumerated type.
 *
 * The button type will be looked up in the list of buttons and then read.
 *
 * @param button The type of button to read.
 * @return The state of the button.
 */
bool Joystick::GetButton(ButtonType button) const {
  switch (button) {
    case kTriggerButton:
      return GetTrigger();
    case kTopButton:
      return GetTop();
    default:
      return false;
  }
}

/**
 * Get the number of axis for a joystick
 *
 * @return the number of axis for the current joystick
 */
int Joystick::GetAxisCount() const { return m_ds.GetStickAxisCount(GetPort()); }

/**
 * Get the axis type of a joystick axis.
 *
 * @return the axis type of a joystick axis.
 */
int Joystick::GetAxisType(int axis) const {
  return m_ds.GetJoystickAxisType(GetPort(), axis);
}

/**
 * Get the number of buttons for a joystick.
 *
 * @return the number of buttons on the current joystick
 */
int Joystick::GetButtonCount() const {
  return m_ds.GetStickButtonCount(GetPort());
}

/**
 * Get the channel currently associated with the specified axis.
 *
 * @param axis The axis to look up the channel for.
 * @return The channel fr the axis.
 */
int Joystick::GetAxisChannel(AxisType axis) const { return m_axes[axis]; }

/**
 * Set the channel associated with a specified axis.
 *
 * @param axis    The axis to set the channel for.
 * @param channel The channel to set the axis to.
 */
void Joystick::SetAxisChannel(AxisType axis, int channel) {
  m_axes[axis] = channel;
}

/**
 * Get the magnitude of the direction vector formed by the joystick's
 * current position relative to its origin.
 *
 * @return The magnitude of the direction vector
 */
double Joystick::GetMagnitude() const {
  return std::sqrt(std::pow(GetX(), 2) + std::pow(GetY(), 2));
}

/**
 * Get the direction of the vector formed by the joystick and its origin
 * in radians.
 *
 * @return The direction of the vector in radians
 */
double Joystick::GetDirectionRadians() const {
  return std::atan2(GetX(), -GetY());
}

/**
 * Get the direction of the vector formed by the joystick and its origin
 * in degrees.
 *
 * uses std::acos(-1) to represent Pi due to absence of readily accessible Pi
 * constant in C++
 *
 * @return The direction of the vector in degrees
 */
double Joystick::GetDirectionDegrees() const {
  return (180 / std::acos(-1)) * GetDirectionRadians();
}
