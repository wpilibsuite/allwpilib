// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/MockHooks.h"

extern "C" {

void HALSIM_SetRuntimeType(HAL_RuntimeType type) {}

void HALSIM_WaitForProgramStart(void) {}

void HALSIM_SetProgramStarted(void) {}

HAL_Bool HALSIM_GetProgramStarted(void) {
  return false;
}

void HALSIM_RestartTiming(void) {}

void HALSIM_PauseTiming(void) {}

void HALSIM_ResumeTiming(void) {}

HAL_Bool HALSIM_IsTimingPaused(void) {
  return false;
}

void HALSIM_StepTiming(uint64_t delta) {}

void HALSIM_StepTimingAsync(uint64_t delta) {}

void HALSIM_SetSendError(HALSIM_SendErrorHandler handler) {}

void HALSIM_SetSendConsoleLine(HALSIM_SendConsoleLineHandler handler) {}

int32_t HALSIM_RegisterSimPeriodicBeforeCallback(
    HALSIM_SimPeriodicCallback callback, void* param) {
  return 0;
}

void HALSIM_CancelSimPeriodicBeforeCallback(int32_t uid) {}

int32_t HALSIM_RegisterSimPeriodicAfterCallback(
    HALSIM_SimPeriodicCallback callback, void* param) {
  return 0;
}

void HALSIM_CancelSimPeriodicAfterCallback(int32_t uid) {}

void HALSIM_CancelAllSimPeriodicCallbacks(void) {}

}  // extern "C"
