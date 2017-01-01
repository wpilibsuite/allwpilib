/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "GenericHID.h"

#include "DriverStation.h"
#include "HAL/HAL.h"

using namespace frc;

GenericHID::GenericHID(int port) : m_ds(DriverStation::GetInstance()) {
  m_port = port;
}

/**
 * Get the value of the axis.
 *
 * @param axis The axis to read, starting at 0.
 * @return The value of the axis.
 */
double GenericHID::GetRawAxis(int axis) const {
  return m_ds.GetStickAxis(m_port, axis);
}

/**
 * Get the button value (starting at button 1)
 *
 * The buttons are returned in a single 16 bit value with one bit representing
 * the state of each button. The appropriate button is returned as a boolean
 * value.
 *
 * @param button The button number to be read (starting at 1)
 * @return The state of the button.
 **/
bool GenericHID::GetRawButton(int button) const {
  return m_ds.GetStickButton(m_port, button);
}

/**
 * Get the angle in degrees of a POV on the HID.
 *
 * The POV angles start at 0 in the up direction, and increase clockwise
 * (e.g. right is 90, upper-left is 315).
 *
 * @param pov The index of the POV to read (starting at 0)
 * @return the angle of the POV in degrees, or -1 if the POV is not pressed.
 */
int GenericHID::GetPOV(int pov) const {
  return m_ds.GetStickPOV(GetPort(), pov);
}

/**
 * Get the number of POVs for the HID.
 *
 * @return the number of POVs for the current HID
 */
int GenericHID::GetPOVCount() const { return m_ds.GetStickPOVCount(GetPort()); }

/**
 * Get the port number of the HID.
 *
 * @return The port number of the HID.
 */
int GenericHID::GetPort() const { return m_port; }

/**
 * Get the type of the HID.
 *
 * @return the type of the HID.
 */
GenericHID::HIDType GenericHID::GetType() const {
  return static_cast<HIDType>(m_ds.GetJoystickType(m_port));
}

/**
 * Get the name of the HID.
 *
 * @return the name of the HID.
 */
std::string GenericHID::GetName() const { return m_ds.GetJoystickName(m_port); }

/**
 * Set a single HID output value for the HID.
 *
 * @param outputNumber The index of the output to set (1-32)
 * @param value        The value to set the output to
 */

void GenericHID::SetOutput(int outputNumber, bool value) {
  m_outputs =
      (m_outputs & ~(1 << (outputNumber - 1))) | (value << (outputNumber - 1));

  HAL_SetJoystickOutputs(m_port, m_outputs, m_leftRumble, m_rightRumble);
}

/**
 * Set all output values for the HID.
 *
 * @param value The 32 bit output value (1 bit for each output)
 */
void GenericHID::SetOutputs(int value) {
  m_outputs = value;
  HAL_SetJoystickOutputs(m_port, m_outputs, m_leftRumble, m_rightRumble);
}

/**
 * Set the rumble output for the HID.
 *
 * The DS currently supports 2 rumble values, left rumble and right rumble.
 *
 * @param type  Which rumble value to set
 * @param value The normalized value (0 to 1) to set the rumble to
 */
void GenericHID::SetRumble(RumbleType type, double value) {
  if (value < 0)
    value = 0;
  else if (value > 1)
    value = 1;
  if (type == kLeftRumble) {
    m_leftRumble = value * 65535;
  } else {
    m_rightRumble = value * 65535;
  }
  HAL_SetJoystickOutputs(m_port, m_outputs, m_leftRumble, m_rightRumble);
}
