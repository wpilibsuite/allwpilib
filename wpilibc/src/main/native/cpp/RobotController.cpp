/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "RobotController.h"

#include <HAL/HAL.h>

#include "ErrorBase.h"

namespace frc {

/**
 * Return the FPGA Version number.
 *
 * For now, expect this to be competition year.
 *
 * @return FPGA Version number.
 */
int RobotController::GetFPGAVersion() {
  int32_t status = 0;
  int version = HAL_GetFPGAVersion(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return version;
}

/**
 * Return the FPGA Revision number.
 *
 * The format of the revision is 3 numbers. The 12 most significant bits are the
 * Major Revision. The next 8 bits are the Minor Revision. The 12 least
 * significant bits are the Build Number.
 *
 * @return FPGA Revision number.
 */
int64_t RobotController::GetFPGARevision() {
  int32_t status = 0;
  int64_t revision = HAL_GetFPGARevision(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return revision;
}

/**
 * Read the microsecond-resolution timer on the FPGA.
 *
 * @return The current time in microseconds according to the FPGA (since FPGA
 *         reset).
 */
uint64_t RobotController::GetFPGATime() {
  int32_t status = 0;
  uint64_t time = HAL_GetFPGATime(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return time;
}

/**
 * Get the state of the "USER" button on the roboRIO.
 *
 * @return True if the button is currently pressed down
 */
bool RobotController::GetUserButton() {
  int32_t status = 0;

  bool value = HAL_GetFPGAButton(&status);
  wpi_setGlobalError(status);

  return value;
}

/**
 * Check if the FPGA outputs are enabled.
 *
 * The outputs may be disabled if the robot is disabled or e-stopped, the
 * watchdog has expired, or if the roboRIO browns out.
 *
 * @return True if the FPGA outputs are enabled.
 */
bool RobotController::IsSysActive() {
  int32_t status = 0;
  bool retVal = HAL_GetSystemActive(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Check if the system is browned out.
 *
 * @return True if the system is browned out
 */
bool RobotController::IsBrownedOut() {
  int32_t status = 0;
  bool retVal = HAL_GetBrownedOut(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Get the input voltage to the robot controller.
 *
 * @return The controller input voltage value in Volts
 */
double RobotController::GetInputVoltage() {
  int32_t status = 0;
  double retVal = HAL_GetVinVoltage(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Get the input current to the robot controller.
 *
 * @return The controller input current value in Amps
 */
double RobotController::GetInputCurrent() {
  int32_t status = 0;
  double retVal = HAL_GetVinCurrent(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Get the voltage of the 6V rail.
 *
 * @return The controller 6V rail voltage value in Volts
 */
double RobotController::GetVoltage6V() {
  int32_t status = 0;
  double retVal = HAL_GetUserVoltage6V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Get the current output of the 6V rail.
 *
 * @return The controller 6V rail output current value in Amps
 */
double RobotController::GetCurrent6V() {
  int32_t status = 0;
  double retVal = HAL_GetUserCurrent6V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Get the enabled state of the 6V rail. The rail may be disabled due to a
 * controller brownout, a short circuit on the rail, or controller over-voltage.
 *
 * @return The controller 6V rail enabled value. True for enabled.
 */
bool RobotController::GetEnabled6V() {
  int32_t status = 0;
  bool retVal = HAL_GetUserActive6V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Get the count of the total current faults on the 6V rail since the controller
 * has booted.
 *
 * @return The number of faults.
 */
int RobotController::GetFaultCount6V() {
  int32_t status = 0;
  int retVal = HAL_GetUserCurrentFaults6V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Get the voltage of the 5V rail.
 *
 * @return The controller 5V rail voltage value in Volts
 */
double RobotController::GetVoltage5V() {
  int32_t status = 0;
  double retVal = HAL_GetUserVoltage5V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Get the current output of the 5V rail.
 *
 * @return The controller 5V rail output current value in Amps
 */
double RobotController::GetCurrent5V() {
  int32_t status = 0;
  double retVal = HAL_GetUserCurrent5V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Get the enabled state of the 5V rail. The rail may be disabled due to a
 * controller brownout, a short circuit on the rail, or controller over-voltage.
 *
 * @return The controller 5V rail enabled value. True for enabled.
 */
bool RobotController::GetEnabled5V() {
  int32_t status = 0;
  bool retVal = HAL_GetUserActive5V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Get the count of the total current faults on the 5V rail since the controller
 * has booted.
 *
 * @return The number of faults
 */
int RobotController::GetFaultCount5V() {
  int32_t status = 0;
  int retVal = HAL_GetUserCurrentFaults5V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Get the voltage of the 3.3V rail.
 *
 * @return The controller 3.3V rail voltage value in Volts
 */
double RobotController::GetVoltage3V3() {
  int32_t status = 0;
  double retVal = HAL_GetUserVoltage3V3(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Get the current output of the 3.3V rail.
 *
 * @return The controller 3.3V rail output current value in Amps
 */
double RobotController::GetCurrent3V3() {
  int32_t status = 0;
  double retVal = HAL_GetUserCurrent3V3(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Get the enabled state of the 3.3V rail. The rail may be disabled due to a
 * controller brownout, a short circuit on the rail, or controller over-voltage.
 *
 * @return The controller 3.3V rail enabled value. True for enabled.
 */
bool RobotController::GetEnabled3V3() {
  int32_t status = 0;
  bool retVal = HAL_GetUserActive3V3(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Get the count of the total current faults on the 3.3V rail since the
 * controller has booted.
 *
 * @return The number of faults
 */
int RobotController::GetFaultCount3V3() {
  int32_t status = 0;
  int retVal = HAL_GetUserCurrentFaults3V3(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

CANStatus RobotController::GetCANStatus() {
  int32_t status = 0;
  float percentBusUtilization = 0;
  uint32_t busOffCount = 0;
  uint32_t txFullCount = 0;
  uint32_t receiveErrorCount = 0;
  uint32_t transmitErrorCount = 0;
  HAL_CAN_GetCANStatus(&percentBusUtilization, &busOffCount, &txFullCount,
                       &receiveErrorCount, &transmitErrorCount, &status);
  if (status != 0) {
    wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
    return {};
  }
  return {percentBusUtilization, static_cast<int>(busOffCount),
          static_cast<int>(txFullCount), static_cast<int>(receiveErrorCount),
          static_cast<int>(transmitErrorCount)};
}
}  // namespace frc
