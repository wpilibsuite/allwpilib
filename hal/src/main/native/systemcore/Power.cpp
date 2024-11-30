// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Power.h"

#include <memory>

#include "HALInitializer.h"
#include "hal/Errors.h"

using namespace hal;

namespace hal {

static void initializePower(int32_t* status) {
  hal::init::CheckInit();
}

}  // namespace hal

namespace hal::init {
void InitializePower() {}
}  // namespace hal::init

extern "C" {

double HAL_GetVinVoltage(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

double HAL_GetVinCurrent(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

double HAL_GetUserVoltage6V(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

double HAL_GetUserCurrent6V(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

HAL_Bool HAL_GetUserActive6V(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int32_t HAL_GetUserCurrentFaults6V(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_SetUserRailEnabled6V(HAL_Bool enabled, int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return;
}

double HAL_GetUserVoltage5V(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

double HAL_GetUserCurrent5V(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

HAL_Bool HAL_GetUserActive5V(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int32_t HAL_GetUserCurrentFaults5V(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_SetUserRailEnabled5V(HAL_Bool enabled, int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return;
}

double HAL_GetUserVoltage3V3(int32_t* status) {
  initializePower(status);
  *status = HAL_HANDLE_ERROR;
  return 0;
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
