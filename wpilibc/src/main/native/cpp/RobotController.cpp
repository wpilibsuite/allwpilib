// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/RobotController.h"

#include <hal/CAN.h>
#include <hal/HALBase.h>
#include <hal/Power.h>

#include "frc/Errors.h"

using namespace frc;

int RobotController::GetFPGAVersion() {
  int32_t status = 0;
  int version = HAL_GetFPGAVersion(&status);
  FRC_CheckErrorStatus(status, "GetFPGAVersion");
  return version;
}

int64_t RobotController::GetFPGARevision() {
  int32_t status = 0;
  int64_t revision = HAL_GetFPGARevision(&status);
  FRC_CheckErrorStatus(status, "GetFPGARevision");
  return revision;
}

uint64_t RobotController::GetFPGATime() {
  int32_t status = 0;
  uint64_t time = HAL_GetFPGATime(&status);
  FRC_CheckErrorStatus(status, "GetFPGATime");
  return time;
}

bool RobotController::GetUserButton() {
  int32_t status = 0;
  bool value = HAL_GetFPGAButton(&status);
  FRC_CheckErrorStatus(status, "GetUserButton");
  return value;
}

units::volt_t RobotController::GetBatteryVoltage() {
  int32_t status = 0;
  double retVal = HAL_GetVinVoltage(&status);
  FRC_CheckErrorStatus(status, "GetBatteryVoltage");
  return units::volt_t{retVal};
}

bool RobotController::IsSysActive() {
  int32_t status = 0;
  bool retVal = HAL_GetSystemActive(&status);
  FRC_CheckErrorStatus(status, "IsSysActive");
  return retVal;
}

bool RobotController::IsBrownedOut() {
  int32_t status = 0;
  bool retVal = HAL_GetBrownedOut(&status);
  FRC_CheckErrorStatus(status, "IsBrownedOut");
  return retVal;
}

double RobotController::GetInputVoltage() {
  int32_t status = 0;
  double retVal = HAL_GetVinVoltage(&status);
  FRC_CheckErrorStatus(status, "GetInputVoltage");
  return retVal;
}

double RobotController::GetInputCurrent() {
  int32_t status = 0;
  double retVal = HAL_GetVinCurrent(&status);
  FRC_CheckErrorStatus(status, "GetInputCurrent");
  return retVal;
}

double RobotController::GetVoltage3V3() {
  int32_t status = 0;
  double retVal = HAL_GetUserVoltage3V3(&status);
  FRC_CheckErrorStatus(status, "GetVoltage3V3");
  return retVal;
}

double RobotController::GetCurrent3V3() {
  int32_t status = 0;
  double retVal = HAL_GetUserCurrent3V3(&status);
  FRC_CheckErrorStatus(status, "GetCurrent3V3");
  return retVal;
}

bool RobotController::GetEnabled3V3() {
  int32_t status = 0;
  bool retVal = HAL_GetUserActive3V3(&status);
  FRC_CheckErrorStatus(status, "GetEnabled3V3");
  return retVal;
}

int RobotController::GetFaultCount3V3() {
  int32_t status = 0;
  int retVal = HAL_GetUserCurrentFaults3V3(&status);
  FRC_CheckErrorStatus(status, "GetFaultCount3V3");
  return retVal;
}

double RobotController::GetVoltage5V() {
  int32_t status = 0;
  double retVal = HAL_GetUserVoltage5V(&status);
  FRC_CheckErrorStatus(status, "GetVoltage5V");
  return retVal;
}

double RobotController::GetCurrent5V() {
  int32_t status = 0;
  double retVal = HAL_GetUserCurrent5V(&status);
  FRC_CheckErrorStatus(status, "GetCurrent5V");
  return retVal;
}

bool RobotController::GetEnabled5V() {
  int32_t status = 0;
  bool retVal = HAL_GetUserActive5V(&status);
  FRC_CheckErrorStatus(status, "GetEnabled5V");
  return retVal;
}

int RobotController::GetFaultCount5V() {
  int32_t status = 0;
  int retVal = HAL_GetUserCurrentFaults5V(&status);
  FRC_CheckErrorStatus(status, "GetFaultCount5V");
  return retVal;
}

double RobotController::GetVoltage6V() {
  int32_t status = 0;
  double retVal = HAL_GetUserVoltage6V(&status);
  FRC_CheckErrorStatus(status, "GetVoltage6V");
  return retVal;
}

double RobotController::GetCurrent6V() {
  int32_t status = 0;
  double retVal = HAL_GetUserCurrent6V(&status);
  FRC_CheckErrorStatus(status, "GetCurrent6V");
  return retVal;
}

bool RobotController::GetEnabled6V() {
  int32_t status = 0;
  bool retVal = HAL_GetUserActive6V(&status);
  FRC_CheckErrorStatus(status, "GetEnabled6V");
  return retVal;
}

int RobotController::GetFaultCount6V() {
  int32_t status = 0;
  int retVal = HAL_GetUserCurrentFaults6V(&status);
  FRC_CheckErrorStatus(status, "GetFaultCount6V");
  return retVal;
}

units::volt_t RobotController::GetBrownoutVoltage() {
  int32_t status = 0;
  double retVal = HAL_GetBrownoutVoltage(&status);
  FRC_CheckErrorStatus(status, "GetBrownoutVoltage");
  return units::volt_t{retVal};
}

void RobotController::SetBrownoutVoltage(units::volt_t brownoutVoltage) {
  int32_t status = 0;
  HAL_SetBrownoutVoltage(brownoutVoltage.value(), &status);
  FRC_CheckErrorStatus(status, "SetBrownoutVoltage");
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
  FRC_CheckErrorStatus(status, "GetCANStatus");
  return {percentBusUtilization, static_cast<int>(busOffCount),
          static_cast<int>(txFullCount), static_cast<int>(receiveErrorCount),
          static_cast<int>(transmitErrorCount)};
}
