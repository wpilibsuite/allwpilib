/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Power.h"

#include "ChipObject.h"

static tPower* power = nullptr;

static void initializePower(int32_t* status) {
  if (power == nullptr) {
    power = tPower::create(status);
  }
}

extern "C" {

/**
 * Get the roboRIO input voltage
 */
float HAL_GetVinVoltage(int32_t* status) {
  initializePower(status);
  return power->readVinVoltage(status) / 4.096f * 0.025733f - 0.029f;
}

/**
 * Get the roboRIO input current
 */
float HAL_GetVinCurrent(int32_t* status) {
  initializePower(status);
  return power->readVinCurrent(status) / 4.096f * 0.017042f - 0.071f;
}

/**
 * Get the 6V rail voltage
 */
float HAL_GetUserVoltage6V(int32_t* status) {
  initializePower(status);
  return power->readUserVoltage6V(status) / 4.096f * 0.007019f - 0.014f;
}

/**
 * Get the 6V rail current
 */
float HAL_GetUserCurrent6V(int32_t* status) {
  initializePower(status);
  return power->readUserCurrent6V(status) / 4.096f * 0.005566f - 0.009f;
}

/**
 * Get the active state of the 6V rail
 */
HAL_Bool HAL_GetUserActive6V(int32_t* status) {
  initializePower(status);
  return power->readStatus_User6V(status) == 4;
}

/**
 * Get the fault count for the 6V rail
 */
int32_t HAL_GetUserCurrentFaults6V(int32_t* status) {
  initializePower(status);
  return static_cast<int32_t>(
      power->readFaultCounts_OverCurrentFaultCount6V(status));
}

/**
 * Get the 5V rail voltage
 */
float HAL_GetUserVoltage5V(int32_t* status) {
  initializePower(status);
  return power->readUserVoltage5V(status) / 4.096f * 0.005962f - 0.013f;
}

/**
 * Get the 5V rail current
 */
float HAL_GetUserCurrent5V(int32_t* status) {
  initializePower(status);
  return power->readUserCurrent5V(status) / 4.096f * 0.001996f - 0.002f;
}

/**
 * Get the active state of the 5V rail
 */
HAL_Bool HAL_GetUserActive5V(int32_t* status) {
  initializePower(status);
  return power->readStatus_User5V(status) == 4;
}

/**
 * Get the fault count for the 5V rail
 */
int32_t HAL_GetUserCurrentFaults5V(int32_t* status) {
  initializePower(status);
  return static_cast<int32_t>(
      power->readFaultCounts_OverCurrentFaultCount5V(status));
}

uint8_t HAL_GetUserStatus5V(int32_t* status) {
  initializePower(status);
  return power->readStatus_User5V(status);
}

/**
 * Get the 3.3V rail voltage
 */
float HAL_GetUserVoltage3V3(int32_t* status) {
  initializePower(status);
  return power->readUserVoltage3V3(status) / 4.096f * 0.004902f - 0.01f;
}

/**
 * Get the 3.3V rail current
 */
float HAL_GetUserCurrent3V3(int32_t* status) {
  initializePower(status);
  return power->readUserCurrent3V3(status) / 4.096f * 0.002486f - 0.003f;
}

/**
 * Get the active state of the 3.3V rail
 */
HAL_Bool HAL_GetUserActive3V3(int32_t* status) {
  initializePower(status);
  return power->readStatus_User3V3(status) == 4;
}

/**
 * Get the fault count for the 3.3V rail
 */
int32_t HAL_GetUserCurrentFaults3V3(int32_t* status) {
  initializePower(status);
  return static_cast<int32_t>(
      power->readFaultCounts_OverCurrentFaultCount3V3(status));
}

}  // extern "C"
