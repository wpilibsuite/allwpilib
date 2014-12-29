/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "ControllerPower.h"

#include <stdint.h>
#include <HAL/Power.hpp>
#include <HAL/HAL.hpp>
#include "ErrorBase.h"

/**
 * Get the input voltage to the robot controller
 * @return The controller input voltage value in Volts
 */
double ControllerPower::GetInputVoltage() {
	int32_t status = 0;
	double retVal = getVinVoltage(&status);
	wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
	return retVal;
}

/**
 * Get the input current to the robot controller
 * @return The controller input current value in Amps
 */
double ControllerPower::GetInputCurrent() {
	int32_t status = 0;
	double retVal = getVinCurrent(&status);
	wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
	return retVal;
}

/**
 * Get the voltage of the 6V rail
 * @return The controller 6V rail voltage value in Volts
 */
double ControllerPower::GetVoltage6V() {
	int32_t status = 0;
	double retVal = getUserVoltage6V(&status);
	wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
	return retVal;
}

/**
 * Get the current output of the 6V rail
 * @return The controller 6V rail output current value in Amps
 */
double ControllerPower::GetCurrent6V() {
	int32_t status = 0;
	double retVal = getUserCurrent6V(&status);
	wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
	return retVal;
}

/**
 * Get the enabled state of the 6V rail. The rail may be disabled due to a controller
 * brownout, a short circuit on the rail, or controller over-voltage
 * @return The controller 6V rail enabled value. True for enabled.
 */
bool ControllerPower::GetEnabled6V() {
	int32_t status = 0;
	bool retVal = getUserActive6V(&status);
	wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
	return retVal;
}

/**
 * Get the count of the total current faults on the 6V rail since the controller has booted
 * @return The number of faults.
 */
int ControllerPower::GetFaultCount6V() {
	int32_t status = 0;
	int retVal = getUserCurrentFaults6V(&status);
	wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
	return retVal;
}

/**
 * Get the voltage of the 5V rail
 * @return The controller 5V rail voltage value in Volts
 */
double ControllerPower::GetVoltage5V() {
	int32_t status = 0;
	double retVal = getUserVoltage5V(&status);
	wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
	return retVal;
}

/**
 * Get the current output of the 5V rail
 * @return The controller 5V rail output current value in Amps
 */
double ControllerPower::GetCurrent5V() {
	int32_t status = 0;
	double retVal = getUserCurrent5V(&status);
	wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
	return retVal;
}

/**
 * Get the enabled state of the 5V rail. The rail may be disabled due to a controller
 * brownout, a short circuit on the rail, or controller over-voltage
 * @return The controller 5V rail enabled value. True for enabled.
 */
bool ControllerPower::GetEnabled5V() {
	int32_t status = 0;
	bool retVal = getUserActive5V(&status);
	wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
	return retVal;
}

/**
 * Get the count of the total current faults on the 5V rail since the controller has booted
 * @return The number of faults
 */
int ControllerPower::GetFaultCount5V() {
	int32_t status = 0;
	int retVal = getUserCurrentFaults5V(&status);
	wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
	return retVal;
}

/**
 * Get the voltage of the 3.3V rail
 * @return The controller 3.3V rail voltage value in Volts
 */
double ControllerPower::GetVoltage3V3() {
	int32_t status = 0;
	double retVal = getUserVoltage3V3(&status);
	wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
	return retVal;
}

/**
 * Get the current output of the 3.3V rail
 * @return The controller 3.3V rail output current value in Amps
 */
double ControllerPower::GetCurrent3V3() {
	int32_t status = 0;
	double retVal = getUserCurrent3V3(&status);
	wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
	return retVal;
}


/**
 * Get the enabled state of the 3.3V rail. The rail may be disabled due to a controller
 * brownout, a short circuit on the rail, or controller over-voltage
 * @return The controller 3.3V rail enabled value. True for enabled.
 */
bool ControllerPower::GetEnabled3V3() {
	int32_t status = 0;
	bool retVal = getUserActive3V3(&status);
	wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
	return retVal;
}

/**
 * Get the count of the total current faults on the 3.3V rail since the controller has booted
 * @return The number of faults
 */
int ControllerPower::GetFaultCount3V3() {
	int32_t status = 0;
	int retVal = getUserCurrentFaults3V3(&status);
	wpi_setGlobalErrorWithContext(status, getHALErrorMessage(status));
	return retVal;
}