/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <atomic>
#include <chrono>
#include <cstdio>
#include <thread>

#include <wpi/timestamp.h>

#include "MockHooksInternal.h"
#include "NotifierInternal.h"

static std::atomic<bool> programStarted{false};

static std::atomic<uint64_t> programStartTime{0};
static std::atomic<uint64_t> programPauseTime{0};

namespace hal {
namespace init {
void InitializeMockHooks() {}
}  // namespace init
}  // namespace hal

namespace hal {
void RestartTiming() {
  programStartTime = wpi::Now();
  if (programPauseTime != 0) programPauseTime = programStartTime.load();
}

void PauseTiming() {
  if (programPauseTime == 0) programPauseTime = wpi::Now();
}

void ResumeTiming() {
  if (programPauseTime != 0) {
    programStartTime += wpi::Now() - programPauseTime;
    programPauseTime = 0;
  }
}

bool IsTimingPaused() { return programPauseTime != 0; }

void StepTiming(uint64_t delta) {
  if (programPauseTime != 0) programPauseTime += delta;
}

int64_t GetFPGATime() {
  uint64_t curTime = programPauseTime;
  if (curTime == 0) curTime = wpi::Now();
  return curTime - programStartTime;
}

double GetFPGATimestamp() { return GetFPGATime() * 1.0e-6; }

void SetProgramStarted() { programStarted = true; }
bool GetProgramStarted() { return programStarted; }
}  // namespace hal

using namespace hal;

extern "C" {
void HALSIM_WaitForProgramStart(void) {
  int count = 0;
  while (!programStarted) {
    count++;
    std::printf("Waiting for program start signal: %d\n", count);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

void HALSIM_SetProgramStarted(void) { SetProgramStarted(); }

HAL_Bool HALSIM_GetProgramStarted(void) { return GetProgramStarted(); }

void HALSIM_RestartTiming(void) { RestartTiming(); }

void HALSIM_PauseTiming(void) {
  PauseTiming();
  PauseNotifiers();
}

void HALSIM_ResumeTiming(void) {
  ResumeTiming();
  ResumeNotifiers();
}

HAL_Bool HALSIM_IsTimingPaused(void) { return IsTimingPaused(); }

void HALSIM_StepTiming(uint64_t delta) {
  StepTiming(delta);
  WakeupNotifiers();
}
}  // extern "C"
