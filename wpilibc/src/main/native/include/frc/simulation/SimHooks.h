/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <hal/HALBase.h>
#include <units/time.h>

namespace frc {
namespace sim {

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

}  // namespace sim
}  // namespace frc
