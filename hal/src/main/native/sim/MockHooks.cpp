// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/simulation/MockHooks.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <thread>

#include "MockHooksInternal.hpp"
#include "NotifierInternal.hpp"
#include "wpi/hal/simulation/NotifierData.h"
#include "wpi/util/print.hpp"
#include "wpi/util/timestamp.hpp"

static std::atomic<bool> programStarted{false};
static std::atomic<int64_t> programState{0};

static std::atomic<uint64_t> programStartTime{0};
static std::atomic<uint64_t> programPauseTime{0};
static std::atomic<uint64_t> programStepTime{0};

namespace wpi::hal::init {
void InitializeMockHooks() {
  wpi::util::SetNowImpl(GetMonotonicTime);
}
}  // namespace wpi::hal::init

namespace wpi::hal {
void RestartTiming() {
  programStartTime = wpi::util::NowDefault();
  programStepTime = 0;
  if (programPauseTime != 0) {
    programPauseTime = programStartTime.load();
  }
}

void PauseTiming() {
  if (programPauseTime == 0) {
    programPauseTime = wpi::util::NowDefault();
  }
}

void ResumeTiming() {
  if (programPauseTime != 0) {
    programStartTime += wpi::util::NowDefault() - programPauseTime;
    programPauseTime = 0;
  }
}

bool IsTimingPaused() {
  return programPauseTime != 0;
}

void StepTiming(uint64_t delta) {
  programStepTime += delta;
}

uint64_t GetMonotonicTime() {
  uint64_t curTime = programPauseTime;
  if (curTime == 0) {
    curTime = wpi::util::NowDefault();
  }
  return curTime + programStepTime - programStartTime;
}

void SetProgramStarted(bool started) {
  programStarted = started;
}
bool GetProgramStarted() {
  return programStarted;
}
}  // namespace wpi::hal

using namespace wpi::hal;

extern "C" {
void HALSIM_WaitForProgramStart(void) {
  int count = 0;
  while (!programStarted) {
    count++;
    if (count % 10 == 0) {
      wpi::util::print("Waiting for program start signal: {}\n", count);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void HALSIM_SetProgramStarted(HAL_Bool started) {
  SetProgramStarted(started);
}

HAL_Bool HALSIM_GetProgramStarted(void) {
  return GetProgramStarted();
}

void HALSIM_SetProgramState(HAL_ControlWord controlWord) {
  programState = controlWord.value;
}

void HALSIM_GetProgramState(HAL_ControlWord* controlWord) {
  controlWord->value = programState;
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
    uint64_t curTime = HAL_GetMonotonicTime();
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
