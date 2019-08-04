/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Power.h"

#include <memory>

#include "HALInitializer.h"
#include "hal/ChipObject.h"

using namespace hal;

namespace hal {

static std::unique_ptr<tPower> power{nullptr};

static void initializePower(int32_t* status) {
  hal::init::CheckInit();
  if (power == nullptr) {
    power.reset(tPower::create(status));
  }
}

}  // namespace hal

namespace hal {
namespace init {
void InitializePower() {}
}  // namespace init
}  // namespace hal

extern "C" {

double HAL_GetVinVoltage(int32_t* status) {
  initializePower(status);
  return power->readVinVoltage(status) / 4.096 * 0.025733 - 0.029;
}

double HAL_GetVinCurrent(int32_t* status) {
  initializePower(status);
  return power->readVinCurrent(status) / 4.096 * 0.017042 - 0.071;
}

double HAL_GetUserVoltage6V(int32_t* status) {
  initializePower(status);
  return power->readUserVoltage6V(status) / 4.096 * 0.007019 - 0.014;
}

double HAL_GetUserCurrent6V(int32_t* status) {
  initializePower(status);
  return power->readUserCurrent6V(status) / 4.096 * 0.005566 - 0.009;
}

HAL_Bool HAL_GetUserActive6V(int32_t* status) {
  initializePower(status);
  return power->readStatus_User6V(status) == 4;
}

int32_t HAL_GetUserCurrentFaults6V(int32_t* status) {
  initializePower(status);
  return static_cast<int32_t>(
      power->readFaultCounts_OverCurrentFaultCount6V(status));
}

double HAL_GetUserVoltage5V(int32_t* status) {
  initializePower(status);
  return power->readUserVoltage5V(status) / 4.096 * 0.005962 - 0.013;
}

double HAL_GetUserCurrent5V(int32_t* status) {
  initializePower(status);
  return power->readUserCurrent5V(status) / 4.096 * 0.001996 - 0.002;
}

HAL_Bool HAL_GetUserActive5V(int32_t* status) {
  initializePower(status);
  return power->readStatus_User5V(status) == 4;
}

int32_t HAL_GetUserCurrentFaults5V(int32_t* status) {
  initializePower(status);
  return static_cast<int32_t>(
      power->readFaultCounts_OverCurrentFaultCount5V(status));
}

double HAL_GetUserVoltage3V3(int32_t* status) {
  initializePower(status);
  return power->readUserVoltage3V3(status) / 4.096 * 0.004902 - 0.01;
}

double HAL_GetUserCurrent3V3(int32_t* status) {
  initializePower(status);
  return power->readUserCurrent3V3(status) / 4.096 * 0.002486 - 0.003;
}

HAL_Bool HAL_GetUserActive3V3(int32_t* status) {
  initializePower(status);
  return power->readStatus_User3V3(status) == 4;
}

int32_t HAL_GetUserCurrentFaults3V3(int32_t* status) {
  initializePower(status);
  return static_cast<int32_t>(
      power->readFaultCounts_OverCurrentFaultCount3V3(status));
}

}  // extern "C"
