// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <hal/HALBase.h>
#include <units/time.h>

namespace frc::sim {

void SetRuntimeType(HAL_RuntimeType type);

void WaitForProgramStart();

void SetProgramStarted();

bool GetProgramStarted();

void RestartTiming();

void PauseTiming();

void ResumeTiming();

bool IsTimingPaused();

void StepTiming(units::second_t delta);

void StepTimingAsync(units::second_t delta);

}  // namespace frc::sim
