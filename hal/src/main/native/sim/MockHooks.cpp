// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <thread>

#include <wpi/print.h>
#include <wpi/timestamp.h>

#include "MockHooksInternal.h"
#include "NotifierInternal.h"
#include "hal/simulation/NotifierData.h"

static std::atomic<bool> programStarted{false};

static std::atomic<uint64_t> programStartTime{0};
static std::atomic<uint64_t> programPauseTime{0};
static std::atomic<uint64_t> programStepTime{0};

namespace hal::init {
void InitializeMockHooks() {
  wpi::SetNowImpl(GetFPGATime);
}
}  // namespace hal::init

namespace hal {
void RestartTiming() {
  programStartTime = wpi::NowDefault();
  programStepTime = 0;
  if (programPauseTime != 0) {
    programPauseTime = programStartTime.load();
  }
}

void PauseTiming() {
  if (programPauseTime == 0) {
    programPauseTime = wpi::NowDefault();
  }
}

void ResumeTiming() {
  if (programPauseTime != 0) {
    programStartTime += wpi::NowDefault() - programPauseTime;
    programPauseTime = 0;
  }
}

bool IsTimingPaused() {
  return programPauseTime != 0;
}

void StepTiming(uint64_t delta) {
  programStepTime += delta;
}

uint64_t GetFPGATime() {
  uint64_t curTime = programPauseTime;
  if (curTime == 0) {
    curTime = wpi::NowDefault();
  }
  return curTime + programStepTime - programStartTime;
}

double GetFPGATimestamp() {
  return GetFPGATime() * 1.0e-6;
}

void SetProgramStarted() {
  programStarted = true;
}
bool GetProgramStarted() {
  return programStarted;
}
}  // namespace hal

using namespace hal;

extern "C" {
void HALSIM_WaitForProgramStart(void) {
  int count = 0;
  while (!programStarted) {
    count++;
    wpi::print("Waiting for program start signal: {}\n", count);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

void HALSIM_SetProgramStarted(void) {
  SetProgramStarted();
}

HAL_Bool HALSIM_GetProgramStarted(void) {
  return GetProgramStarted();
}

void HALSIM_RestartTiming(void) {
  RestartTiming();
}

void HALSIM_PauseTiming(void) {
  PauseTiming();
  PauseNotifiers();
}

void HALSIM_ResumeTiming(void) {
  ResumeTiming();
  ResumeNotifiers();
}

HAL_Bool HALSIM_IsTimingPaused(void) {
  return IsTimingPaused();
}

void HALSIM_StepTiming(uint64_t delta) {
  WaitNotifiers();

  while (delta > 0) {
    int32_t status = 0;
    uint64_t curTime = HAL_GetFPGATime(&status);
    uint64_t nextTimeout = HALSIM_GetNextNotifierTimeout();
    uint64_t step = (std::min)(delta, nextTimeout - curTime);

    StepTiming(step);
    delta -= step;

    WakeupWaitNotifiers();
  }
}

void HALSIM_StepTimingAsync(uint64_t delta) {
  StepTiming(delta);
  WakeupNotifiers();
}
}  // extern "C"
