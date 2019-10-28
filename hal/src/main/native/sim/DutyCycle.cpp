/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/DutyCycle.h"

namespace hal {
namespace init {
void InitializeDutyCycle() {}
}  // namespace init
}  // namespace hal

extern "C" {
HAL_DutyCycleHandle HAL_InitializeDutyCycle(HAL_Handle digitalSourceHandle,
                                            HAL_AnalogTriggerType triggerType,
                                            int32_t* status) {
  return HAL_kInvalidHandle;
}
void HAL_FreeDutyCycle(HAL_DutyCycleHandle dutyCycleHandle) {}

int32_t HAL_GetDutyCycleFrequency(HAL_DutyCycleHandle dutyCycleHandle,
                                  int32_t* status) {
  return 0;
}
double HAL_GetDutyCycleOutput(HAL_DutyCycleHandle dutyCycleHandle,
                              int32_t* status) {
  return 0;
}
int32_t HAL_GetDutyCycleOutputRaw(HAL_DutyCycleHandle dutyCycleHandle,
                                  int32_t* status) {
  return 0;
}
int32_t HAL_GetDutyCycleOutputScaleFactor(HAL_DutyCycleHandle dutyCycleHandle,
                                          int32_t* status) {
  return 0;
}
int32_t HAL_GetDutyCycleFPGAIndex(HAL_DutyCycleHandle dutyCycleHandle,
                                  int32_t* status) {
  return -1;
}
}  // extern "C"
