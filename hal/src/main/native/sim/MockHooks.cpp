/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
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

static std::atomic<bool> programStarted{false};

static std::atomic<uint64_t> programStartTime{0};

namespace hal {
namespace init {
void InitializeMockHooks() {}
}  // namespace init
}  // namespace hal

namespace hal {
void RestartTiming() { programStartTime = wpi::Now(); }

int64_t GetFPGATime() {
  auto now = wpi::Now();
  auto currentTime = now - programStartTime;
  return currentTime;
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
}  // extern "C"
