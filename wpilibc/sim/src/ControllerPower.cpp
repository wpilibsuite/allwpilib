/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ControllerPower.h"

#include <stdint.h>

#include "ErrorBase.h"
#include "HAL/HAL.h"
#include "HAL/Power.h"

/**
 * Get the input voltage to the robot controller.
 *
 * @return The controller input voltage value in Volts
 */
float ControllerPower::GetInputVoltage() {
  return 12.0;
}

/**
 * Get the input current to the robot controller.
 *
 * @return The controller input current value in Amps
 */
float ControllerPower::GetInputCurrent() {
  return 0.0;
}

/**
 * Get the voltage of the 6V rail.
 *
 * @return The controller 6V rail voltage value in Volts
 */
float ControllerPower::GetVoltage6V() {
  return 6.0;
}

/**
 * Get the current output of the 6V rail.
 *
 * @return The controller 6V rail output current value in Amps
 */
float ControllerPower::GetCurrent6V() {
  return 0.0;
}

/**
 * Get the enabled state of the 6V rail. The rail may be disabled due to a
 * controller brownout, a short circuit on the rail, or controller over-voltage.
 *
 * @return The controller 6V rail enabled value. True for enabled.
 */
bool ControllerPower::GetEnabled6V() {
  return true;
}

/**
 * Get the count of the total current faults on the 6V rail since the controller
 * has booted.
 *
 * @return The number of faults.
 */
int ControllerPower::GetFaultCount6V() {
  return 0;
}

/**
 * Get the voltage of the 5V rail.
 *
 * @return The controller 5V rail voltage value in Volts
 */
float ControllerPower::GetVoltage5V() {
  return 5.0;
}

/**
 * Get the current output of the 5V rail.
 *
 * @return The controller 5V rail output current value in Amps
 */
float ControllerPower::GetCurrent5V() {
  return 0.0;
}

/**
 * Get the enabled state of the 5V rail. The rail may be disabled due to a
 * controller brownout, a short circuit on the rail, or controller over-voltage.
 *
 * @return The controller 5V rail enabled value. True for enabled.
 */
bool ControllerPower::GetEnabled5V() {
  return true;
}

/**
 * Get the count of the total current faults on the 5V rail since the controller
 * has booted.
 *
 * @return The number of faults
 */
int ControllerPower::GetFaultCount5V() {
  return 0;
}

/**
 * Get the voltage of the 3.3V rail.
 *
 * @return The controller 3.3V rail voltage value in Volts
 */
float ControllerPower::GetVoltage3V3() {
  return 3.3;
}

/**
 * Get the current output of the 3.3V rail.
 *
 * @return The controller 3.3V rail output current value in Amps
 */
float ControllerPower::GetCurrent3V3() {
  return 0.0;
}

/**
 * Get the enabled state of the 3.3V rail. The rail may be disabled due to a
 * controller brownout, a short circuit on the rail, or controller over-voltage.
 *
 * @return The controller 3.3V rail enabled value. True for enabled.
 */
bool ControllerPower::GetEnabled3V3() {
  return true;
}

/**
 * Get the count of the total current faults on the 3.3V rail since the
 * controller has booted.
 *
 * @return The number of faults
 */
int ControllerPower::GetFaultCount3V3() {
  return 0;
}
