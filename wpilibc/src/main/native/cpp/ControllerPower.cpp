/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/ControllerPower.h"

#include <stdint.h>

#include <hal/HAL.h>
#include <hal/Power.h>

#include "frc/ErrorBase.h"

using namespace frc;

double ControllerPower::GetInputVoltage() {
  int32_t status = 0;
  double retVal = HAL_GetVinVoltage(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

double ControllerPower::GetInputCurrent() {
  int32_t status = 0;
  double retVal = HAL_GetVinCurrent(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

double ControllerPower::GetVoltage3V3() {
  int32_t status = 0;
  double retVal = HAL_GetUserVoltage3V3(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

double ControllerPower::GetCurrent3V3() {
  int32_t status = 0;
  double retVal = HAL_GetUserCurrent3V3(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

bool ControllerPower::GetEnabled3V3() {
  int32_t status = 0;
  bool retVal = HAL_GetUserActive3V3(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

int ControllerPower::GetFaultCount3V3() {
  int32_t status = 0;
  int retVal = HAL_GetUserCurrentFaults3V3(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

double ControllerPower::GetVoltage5V() {
  int32_t status = 0;
  double retVal = HAL_GetUserVoltage5V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

double ControllerPower::GetCurrent5V() {
  int32_t status = 0;
  double retVal = HAL_GetUserCurrent5V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

bool ControllerPower::GetEnabled5V() {
  int32_t status = 0;
  bool retVal = HAL_GetUserActive5V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

int ControllerPower::GetFaultCount5V() {
  int32_t status = 0;
  int retVal = HAL_GetUserCurrentFaults5V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

double ControllerPower::GetVoltage6V() {
  int32_t status = 0;
  double retVal = HAL_GetUserVoltage6V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

double ControllerPower::GetCurrent6V() {
  int32_t status = 0;
  double retVal = HAL_GetUserCurrent6V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

bool ControllerPower::GetEnabled6V() {
  int32_t status = 0;
  bool retVal = HAL_GetUserActive6V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

int ControllerPower::GetFaultCount6V() {
  int32_t status = 0;
  int retVal = HAL_GetUserCurrentFaults6V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}
