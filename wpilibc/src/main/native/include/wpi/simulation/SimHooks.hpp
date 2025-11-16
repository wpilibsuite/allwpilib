// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/HALBase.h"
#include "wpi/units/time.hpp"

namespace wpi::sim {

/**
 * Override the HAL runtime type (simulated/real).
 *
 * @param type runtime type
 */
void SetRuntimeType(HAL_RuntimeType type);

void WaitForProgramStart();

void SetProgramStarted();

bool GetProgramStarted();

/**
 * Restart the simulator time.
 */
void RestartTiming();

/**
 * Pause the simulator time.
 */
void PauseTiming();

/**
 * Resume the simulator time.
 */
void ResumeTiming();

/**
 * Check if the simulator time is paused.
 *
 * @return true if paused
 */
bool IsTimingPaused();

/**
 * Advance the simulator time and wait for all notifiers to run.
 *
 * @param delta the amount to advance (in seconds)
 */
void StepTiming(wpi::units::second_t delta);

/**
 * Advance the simulator time and return immediately.
 *
 * @param delta the amount to advance (in seconds)
 */
void StepTimingAsync(wpi::units::second_t delta);

}  // namespace wpi::sim
