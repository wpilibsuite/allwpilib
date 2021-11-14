// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Power.h"

#include "mockdata/RoboRioDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializePower() {}
}  // namespace hal::init

// TODO: Fix the naming in here
extern "C" {
double HAL_GetVinVoltage(int32_t* status) {
  return SimRoboRioData->vInVoltage;
}
double HAL_GetVinCurrent(int32_t* status) {
  return SimRoboRioData->vInCurrent;
}
double HAL_GetUserVoltage6V(int32_t* status) {
  return SimRoboRioData->userVoltage6V;
}
double HAL_GetUserCurrent6V(int32_t* status) {
  return SimRoboRioData->userCurrent6V;
}
HAL_Bool HAL_GetUserActive6V(int32_t* status) {
  return SimRoboRioData->userActive6V;
}
int32_t HAL_GetUserCurrentFaults6V(int32_t* status) {
  return SimRoboRioData->userFaults6V;
}
double HAL_GetUserVoltage5V(int32_t* status) {
  return SimRoboRioData->userVoltage5V;
}
double HAL_GetUserCurrent5V(int32_t* status) {
  return SimRoboRioData->userCurrent5V;
}
HAL_Bool HAL_GetUserActive5V(int32_t* status) {
  return SimRoboRioData->userActive5V;
}
int32_t HAL_GetUserCurrentFaults5V(int32_t* status) {
  return SimRoboRioData->userFaults5V;
}
double HAL_GetUserVoltage3V3(int32_t* status) {
  return SimRoboRioData->userVoltage3V3;
}
double HAL_GetUserCurrent3V3(int32_t* status) {
  return SimRoboRioData->userCurrent3V3;
}
HAL_Bool HAL_GetUserActive3V3(int32_t* status) {
  return SimRoboRioData->userActive3V3;
}
int32_t HAL_GetUserCurrentFaults3V3(int32_t* status) {
  return SimRoboRioData->userFaults3V3;
}
void HAL_SetBrownoutVoltage(double voltage, int32_t* status) {
  SimRoboRioData->brownoutVoltage = voltage;
}
double HAL_GetBrownoutVoltage(int32_t* status) {
  return SimRoboRioData->brownoutVoltage;
}
}  // extern "C"
