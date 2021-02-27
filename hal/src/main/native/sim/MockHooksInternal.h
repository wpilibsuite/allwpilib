// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/simulation/MockHooks.h"

namespace hal {
void RestartTiming();

void PauseTiming();

void ResumeTiming();

bool IsTimingPaused();

void StepTiming(uint64_t delta);

uint64_t GetFPGATime();

double GetFPGATimestamp();

void SetProgramStarted();
}  // namespace hal
