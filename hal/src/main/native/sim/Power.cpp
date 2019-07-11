/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Power.h"

#include "mockdata/RoboRioDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializePower() {}
}  // namespace init
}  // namespace hal

// TODO: Fix the naming in here
extern "C" {
double HAL_GetVinVoltage(int32_t* status) {
  return SimRoboRioData[0].vInVoltage;
}
double HAL_GetVinCurrent(int32_t* status) {
  return SimRoboRioData[0].vInCurrent;
}
double HAL_GetUserVoltage6V(int32_t* status) {
  return SimRoboRioData[0].userVoltage6V;
}
double HAL_GetUserCurrent6V(int32_t* status) {
  return SimRoboRioData[0].userCurrent6V;
}
HAL_Bool HAL_GetUserActive6V(int32_t* status) {
  return SimRoboRioData[0].userActive6V;
}
int32_t HAL_GetUserCurrentFaults6V(int32_t* status) {
  return SimRoboRioData[0].userFaults6V;
}
double HAL_GetUserVoltage5V(int32_t* status) {
  return SimRoboRioData[0].userVoltage5V;
}
double HAL_GetUserCurrent5V(int32_t* status) {
  return SimRoboRioData[0].userCurrent5V;
}
HAL_Bool HAL_GetUserActive5V(int32_t* status) {
  return SimRoboRioData[0].userActive5V;
}
int32_t HAL_GetUserCurrentFaults5V(int32_t* status) {
  return SimRoboRioData[0].userFaults5V;
}
double HAL_GetUserVoltage3V3(int32_t* status) {
  return SimRoboRioData[0].userVoltage3V3;
}
double HAL_GetUserCurrent3V3(int32_t* status) {
  return SimRoboRioData[0].userCurrent3V3;
}
HAL_Bool HAL_GetUserActive3V3(int32_t* status) {
  return SimRoboRioData[0].userActive3V3;
}
int32_t HAL_GetUserCurrentFaults3V3(int32_t* status) {
  return SimRoboRioData[0].userFaults3V3;
}
}  // extern "C"
