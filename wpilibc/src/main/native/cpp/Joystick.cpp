/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Joystick.h"

#include <cmath>

#include <HAL/HAL.h>

#include "DriverStation.h"
#include "WPIErrors.h"

using namespace frc;

constexpr double kPi = 3.14159265358979323846;

/**
 * Construct an instance of a joystick.
 *
 * The joystick index is the USB port on the Driver Station.
 *
 * @param port The port on the Driver Station that the joystick is plugged into
 *             (0-5).
 */
Joystick::Joystick(int port) : GenericHID(port) {
  m_axes[Axis::kX] = kDefaultXAxis;
  m_axes[Axis::kY] = kDefaultYAxis;
  m_axes[Axis::kZ] = kDefaultZAxis;
  m_axes[Axis::kTwist] = kDefaultTwistAxis;
  m_axes[Axis::kThrottle] = kDefaultThrottleAxis;

  HAL_Report(HALUsageReporting::kResourceType_Joystick, port);
}

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
 * Set the channel associated with the X axis.
 *
 * @param channel The channel to set the axis to.
 */
void Joystick::SetXChannel(int channel) { m_axes[Axis::kX] = channel; }

/**
 * Set the channel associated with the Y axis.
 *
 * @param axis    The axis to set the channel for.
 * @param channel The channel to set the axis to.
 */
void Joystick::SetYChannel(int channel) { m_axes[Axis::kY] = channel; }

/**
 * Set the channel associated with the Z axis.
 *
 * @param axis    The axis to set the channel for.
 * @param channel The channel to set the axis to.
 */
void Joystick::SetZChannel(int channel) { m_axes[Axis::kZ] = channel; }

/**
 * Set the channel associated with the twist axis.
 *
 * @param axis    The axis to set the channel for.
 * @param channel The channel to set the axis to.
 */
void Joystick::SetTwistChannel(int channel) { m_axes[Axis::kTwist] = channel; }

/**
 * Set the channel associated with the throttle axis.
 *
 * @param axis    The axis to set the channel for.
 * @param channel The channel to set the axis to.
 */
void Joystick::SetThrottleChannel(int channel) {
  m_axes[Axis::kThrottle] = channel;
}

/**
 * Get the channel currently associated with the X axis.
 *
 * @return The channel for the axis.
 */
int Joystick::GetXChannel() const { return m_axes[Axis::kX]; }

/**
 * Get the channel currently associated with the Y axis.
 *
 * @return The channel for the axis.
 */
int Joystick::GetYChannel() const { return m_axes[Axis::kY]; }

/**
 * Get the channel currently associated with the Z axis.
 *
 * @return The channel for the axis.
 */
int Joystick::GetZChannel() const { return m_axes[Axis::kZ]; }

/**
 * Get the channel currently associated with the twist axis.
 *
 * @return The channel for the axis.
 */
int Joystick::GetTwistChannel() const { return m_axes[Axis::kTwist]; }

/**
 * Get the channel currently associated with the throttle axis.
 *
 * @return The channel for the axis.
 */
int Joystick::GetThrottleChannel() const { return m_axes[Axis::kThrottle]; }

/**
 * Get the X value of the joystick.
 *
 * This depends on the mapping of the joystick connected to the current port.
 *
 * @param hand This parameter is ignored for the Joystick class and is only
 *             here to complete the GenericHID interface.
 */
double Joystick::GetX(JoystickHand hand) const {
  return GetRawAxis(m_axes[kDefaultXAxis]);
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
  return GetRawAxis(m_axes[kDefaultYAxis]);
}

/**
 * Get the Z value of the current joystick.
 *
 * This depends on the mapping of the joystick connected to the current port.
 */
double Joystick::GetZ() const { return GetRawAxis(m_axes[kDefaultZAxis]); }

/**
 * Get the twist value of the current joystick.
 *
 * This depends on the mapping of the joystick connected to the current port.
 */
double Joystick::GetTwist() const {
  return GetRawAxis(m_axes[kDefaultTwistAxis]);
}

/**
 * Get the throttle value of the current joystick.
 *
 * This depends on the mapping of the joystick connected to the current port.
 */
double Joystick::GetThrottle() const {
  return GetRawAxis(m_axes[kDefaultThrottleAxis]);
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
      return GetX();
    case kYAxis:
      return GetY();
    case kZAxis:
      return GetZ();
    case kTwistAxis:
      return GetTwist();
    case kThrottleAxis:
      return GetThrottle();
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
 * @return The state of the trigger.
 */
bool Joystick::GetTrigger() const { return GetRawButton(Button::kTrigger); }

/**
 * Whether the trigger was pressed since the last check.
 *
 * @return Whether the button was pressed since the last check.
 */
bool Joystick::GetTriggerPressed() {
  return GetRawButtonPressed(Button::kTrigger);
}

/**
 * Whether the trigger was released since the last check.
 *
 * @return Whether the button was released since the last check.
 */
bool Joystick::GetTriggerReleased() {
  return GetRawButtonReleased(Button::kTrigger);
}

/**
 * Read the state of the top button on the joystick.
 *
 * Look up which button has been assigned to the top and read its state.
 *
 * @return The state of the top button.
 */
bool Joystick::GetTop() const { return GetRawButton(Button::kTop); }

/**
 * Whether the top button was pressed since the last check.
 *
 * @return Whether the button was pressed since the last check.
 */
bool Joystick::GetTopPressed() { return GetRawButtonPressed(Button::kTop); }

/**
 * Whether the top button was released since the last check.
 *
 * @return Whether the button was released since the last check.
 */
bool Joystick::GetTopReleased() { return GetRawButtonReleased(Button::kTop); }

Joystick* Joystick::GetStickForPort(int port) {
  static std::array<std::unique_ptr<Joystick>, DriverStation::kJoystickPorts>
      joysticks{};
  auto stick = joysticks[port].get();
  if (stick == nullptr) {
    joysticks[port] = std::make_unique<Joystick>(port);
    stick = joysticks[port].get();
  }
  return stick;
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
  int temp = button;
  return GetRawButton(static_cast<Button>(temp));
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
 * @return The direction of the vector in degrees
 */
double Joystick::GetDirectionDegrees() const {
  return (180 / kPi) * GetDirectionRadians();
}
