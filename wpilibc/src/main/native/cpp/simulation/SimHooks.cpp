/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/SimHooks.h"

#include <hal/simulation/MockHooks.h>

namespace frc {
namespace sim {

void SetRuntimeType(HAL_RuntimeType type) { HALSIM_SetRuntimeType(type); }

void WaitForProgramStart() { HALSIM_WaitForProgramStart(); }

void SetProgramStarted() { HALSIM_SetProgramStarted(); }

bool GetProgramStarted() { return HALSIM_GetProgramStarted(); }

void RestartTiming() { HALSIM_RestartTiming(); }

void PauseTiming() { HALSIM_PauseTiming(); }

void ResumeTiming() { HALSIM_ResumeTiming(); }

bool IsTimingPaused() { return HALSIM_IsTimingPaused(); }

void StepTiming(units::second_t delta) {
  HALSIM_StepTiming(static_cast<uint64_t>(delta.to<double>() * 1e6));
}

void StepTimingAsync(units::second_t delta) {
  HALSIM_StepTimingAsync(static_cast<uint64_t>(delta.to<double>() * 1e6));
}

}  // namespace sim
}  // namespace frc
