/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/RobotController.h"

#include <hal/HAL.h>

#include "frc/ErrorBase.h"

using namespace frc;

int RobotController::GetFPGAVersion() {
  int32_t status = 0;
  int version = HAL_GetFPGAVersion(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return version;
}

int64_t RobotController::GetFPGARevision() {
  int32_t status = 0;
  int64_t revision = HAL_GetFPGARevision(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return revision;
}

uint64_t RobotController::GetFPGATime() {
  int32_t status = 0;
  uint64_t time = HAL_GetFPGATime(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return time;
}

bool RobotController::GetUserButton() {
  int32_t status = 0;

  bool value = HAL_GetFPGAButton(&status);
  wpi_setGlobalError(status);

  return value;
}

bool RobotController::IsSysActive() {
  int32_t status = 0;
  bool retVal = HAL_GetSystemActive(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

bool RobotController::IsBrownedOut() {
  int32_t status = 0;
  bool retVal = HAL_GetBrownedOut(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

double RobotController::GetInputVoltage() {
  int32_t status = 0;
  double retVal = HAL_GetVinVoltage(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

double RobotController::GetInputCurrent() {
  int32_t status = 0;
  double retVal = HAL_GetVinCurrent(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

double RobotController::GetVoltage3V3() {
  int32_t status = 0;
  double retVal = HAL_GetUserVoltage3V3(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

double RobotController::GetCurrent3V3() {
  int32_t status = 0;
  double retVal = HAL_GetUserCurrent3V3(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

bool RobotController::GetEnabled3V3() {
  int32_t status = 0;
  bool retVal = HAL_GetUserActive3V3(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

int RobotController::GetFaultCount3V3() {
  int32_t status = 0;
  int retVal = HAL_GetUserCurrentFaults3V3(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

double RobotController::GetVoltage5V() {
  int32_t status = 0;
  double retVal = HAL_GetUserVoltage5V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

double RobotController::GetCurrent5V() {
  int32_t status = 0;
  double retVal = HAL_GetUserCurrent5V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

bool RobotController::GetEnabled5V() {
  int32_t status = 0;
  bool retVal = HAL_GetUserActive5V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

int RobotController::GetFaultCount5V() {
  int32_t status = 0;
  int retVal = HAL_GetUserCurrentFaults5V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

double RobotController::GetVoltage6V() {
  int32_t status = 0;
  double retVal = HAL_GetUserVoltage6V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

double RobotController::GetCurrent6V() {
  int32_t status = 0;
  double retVal = HAL_GetUserCurrent6V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

bool RobotController::GetEnabled6V() {
  int32_t status = 0;
  bool retVal = HAL_GetUserActive6V(&status);
  wpi_setGlobalErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

int RobotController::GetFaultCount6V() {
  int32_t status = 0;
  int retVal = HAL_GetUserCurrentFaults6V(&status);
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
