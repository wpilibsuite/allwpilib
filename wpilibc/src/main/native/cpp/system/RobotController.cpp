// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/RobotController.hpp"

#include <functional>
#include <string>

#include "wpi/hal/CAN.h"
#include "wpi/hal/HALBase.h"
#include "wpi/hal/Power.h"
#include "wpi/system/Errors.hpp"
#include "wpi/util/string.hpp"

using namespace wpi;

std::function<uint64_t()> RobotController::m_timeSource = [] {
  return RobotController::GetFPGATime();
};

std::string RobotController::GetSerialNumber() {
  WPI_String serialNum;
  HAL_GetSerialNumber(&serialNum);
  std::string ret{wpi::util::to_string_view(&serialNum)};
  WPI_FreeString(&serialNum);
  return ret;
}

std::string RobotController::GetComments() {
  WPI_String comments;
  HAL_GetComments(&comments);
  std::string ret{wpi::util::to_string_view(&comments)};
  WPI_FreeString(&comments);
  return ret;
}

int32_t RobotController::GetTeamNumber() {
  return HAL_GetTeamNumber();
}

void RobotController::SetTimeSource(std::function<uint64_t()> supplier) {
  m_timeSource = supplier;
}

uint64_t RobotController::GetTime() {
  return m_timeSource();
}

uint64_t RobotController::GetFPGATime() {
  int32_t status = 0;
  uint64_t time = HAL_GetFPGATime(&status);
  WPILIB_CheckErrorStatus(status, "GetFPGATime");
  return time;
}

wpi::units::volt_t RobotController::GetBatteryVoltage() {
  int32_t status = 0;
  double retVal = HAL_GetVinVoltage(&status);
  WPILIB_CheckErrorStatus(status, "GetBatteryVoltage");
  return wpi::units::volt_t{retVal};
}

bool RobotController::IsSysActive() {
  int32_t status = 0;
  bool retVal = HAL_GetSystemActive(&status);
  WPILIB_CheckErrorStatus(status, "IsSysActive");
  return retVal;
}

bool RobotController::IsBrownedOut() {
  int32_t status = 0;
  bool retVal = HAL_GetBrownedOut(&status);
  WPILIB_CheckErrorStatus(status, "IsBrownedOut");
  return retVal;
}

int RobotController::GetCommsDisableCount() {
  int32_t status = 0;
  int retVal = HAL_GetCommsDisableCount(&status);
  WPILIB_CheckErrorStatus(status, "GetCommsDisableCount");
  return retVal;
}

bool RobotController::GetRSLState() {
  int32_t status = 0;
  bool retVal = HAL_GetRSLState(&status);
  WPILIB_CheckErrorStatus(status, "GetRSLState");
  return retVal;
}

bool RobotController::IsSystemTimeValid() {
  int32_t status = 0;
  bool retVal = HAL_GetSystemTimeValid(&status);
  WPILIB_CheckErrorStatus(status, "IsSystemTimeValid");
  return retVal;
}

double RobotController::GetInputVoltage() {
  int32_t status = 0;
  double retVal = HAL_GetVinVoltage(&status);
  WPILIB_CheckErrorStatus(status, "GetInputVoltage");
  return retVal;
}

double RobotController::GetVoltage3V3() {
  int32_t status = 0;
  double retVal = HAL_GetUserVoltage3V3(&status);
  WPILIB_CheckErrorStatus(status, "GetVoltage3V3");
  return retVal;
}

double RobotController::GetCurrent3V3() {
  int32_t status = 0;
  double retVal = HAL_GetUserCurrent3V3(&status);
  WPILIB_CheckErrorStatus(status, "GetCurrent3V3");
  return retVal;
}

void RobotController::SetEnabled3V3(bool enabled) {
  int32_t status = 0;
  HAL_SetUserRailEnabled3V3(enabled, &status);
  WPILIB_CheckErrorStatus(status, "SetEnabled3V3");
}

bool RobotController::GetEnabled3V3() {
  int32_t status = 0;
  bool retVal = HAL_GetUserActive3V3(&status);
  WPILIB_CheckErrorStatus(status, "GetEnabled3V3");
  return retVal;
}

int RobotController::GetFaultCount3V3() {
  int32_t status = 0;
  int retVal = HAL_GetUserCurrentFaults3V3(&status);
  WPILIB_CheckErrorStatus(status, "GetFaultCount3V3");
  return retVal;
}

void RobotController::ResetRailFaultCounts() {
  int32_t status = 0;
  HAL_ResetUserCurrentFaults(&status);
  WPILIB_CheckErrorStatus(status, "ResetRailFaultCounts");
}

wpi::units::volt_t RobotController::GetBrownoutVoltage() {
  int32_t status = 0;
  double retVal = HAL_GetBrownoutVoltage(&status);
  WPILIB_CheckErrorStatus(status, "GetBrownoutVoltage");
  return wpi::units::volt_t{retVal};
}

void RobotController::SetBrownoutVoltage(wpi::units::volt_t brownoutVoltage) {
  int32_t status = 0;
  HAL_SetBrownoutVoltage(brownoutVoltage.value(), &status);
  WPILIB_CheckErrorStatus(status, "SetBrownoutVoltage");
}

wpi::units::celsius_t RobotController::GetCPUTemp() {
  int32_t status = 0;
  double retVal = HAL_GetCPUTemp(&status);
  WPILIB_CheckErrorStatus(status, "GetCPUTemp");
  return wpi::units::celsius_t{retVal};
}

CANStatus RobotController::GetCANStatus(int busId) {
  int32_t status = 0;
  float percentBusUtilization = 0;
  uint32_t busOffCount = 0;
  uint32_t txFullCount = 0;
  uint32_t receiveErrorCount = 0;
  uint32_t transmitErrorCount = 0;
  HAL_CAN_GetCANStatus(busId, &percentBusUtilization, &busOffCount,
                       &txFullCount, &receiveErrorCount, &transmitErrorCount,
                       &status);
  WPILIB_CheckErrorStatus(status, "GetCANStatus");
  return {percentBusUtilization, static_cast<int>(busOffCount),
          static_cast<int>(txFullCount), static_cast<int>(receiveErrorCount),
          static_cast<int>(transmitErrorCount)};
}
