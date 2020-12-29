// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/HALBase.h"
#include "hal/Types.h"

extern "C" {
void HALSIM_SetRuntimeType(HAL_RuntimeType type);
void HALSIM_WaitForProgramStart(void);
void HALSIM_SetProgramStarted(void);
HAL_Bool HALSIM_GetProgramStarted(void);
void HALSIM_RestartTiming(void);
void HALSIM_PauseTiming(void);
void HALSIM_ResumeTiming(void);
HAL_Bool HALSIM_IsTimingPaused(void);
void HALSIM_StepTiming(uint64_t delta);
void HALSIM_StepTimingAsync(uint64_t delta);

typedef int32_t (*HALSIM_SendErrorHandler)(
    HAL_Bool isError, int32_t errorCode, HAL_Bool isLVCode, const char* details,
    const char* location, const char* callStack, HAL_Bool printMsg);
void HALSIM_SetSendError(HALSIM_SendErrorHandler handler);

typedef int32_t (*HALSIM_SendConsoleLineHandler)(const char* line);
void HALSIM_SetSendConsoleLine(HALSIM_SendConsoleLineHandler handler);

typedef void (*HALSIM_SimPeriodicCallback)(void* param);
int32_t HALSIM_RegisterSimPeriodicBeforeCallback(
    HALSIM_SimPeriodicCallback callback, void* param);
void HALSIM_CancelSimPeriodicBeforeCallback(int32_t uid);

int32_t HALSIM_RegisterSimPeriodicAfterCallback(
    HALSIM_SimPeriodicCallback callback, void* param);
void HALSIM_CancelSimPeriodicAfterCallback(int32_t uid);

}  // extern "C"
