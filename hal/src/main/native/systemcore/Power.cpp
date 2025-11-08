// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/Power.h"

#include <memory>

#include "HALInitializer.h"
#include "SystemServerInternal.h"
#include "mrc/NtNetComm.h"
#include "wpi/hal/Errors.h"
#include "wpi/nt/DoubleTopic.hpp"

using namespace hal;

namespace hal {

static void initializePower(int32_t* status) {
  hal::init::CheckInit();
}

}  // namespace hal

namespace {
struct SystemServerPower {
  nt::NetworkTableInstance ntInst;

  nt::DoubleSubscriber batterySubscriber;

  explicit SystemServerPower(nt::NetworkTableInstance inst) {
    ntInst = inst;

    batterySubscriber =
        ntInst.GetDoubleTopic(ROBOT_BATTERY_VOLTAGE_PATH).Subscribe(0.0);
  }
};
}  // namespace

static ::SystemServerPower* systemServerPower;

namespace hal::init {
void InitializePower() {
  systemServerPower = new ::SystemServerPower{hal::GetSystemServer()};
}
}  // namespace hal::init

extern "C" {

double HAL_GetVinVoltage(int32_t* status) {
  initializePower(status);
  return systemServerPower->batterySubscriber.Get();
}

double HAL_GetUserVoltage3V3(int32_t* status) {
  initializePower(status);
  // Until we have a value, make this work, as lots of other
  // code depends on it.
  return 3.3;
}

double HAL_GetUserCurrent3V3(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

HAL_Bool HAL_GetUserActive3V3(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int32_t HAL_GetUserCurrentFaults3V3(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_SetUserRailEnabled3V3(HAL_Bool enabled, int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_ResetUserCurrentFaults(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetBrownoutVoltage(double voltage, int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return;
}

double HAL_GetBrownoutVoltage(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

double HAL_GetCPUTemp(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

}  // extern "C"
