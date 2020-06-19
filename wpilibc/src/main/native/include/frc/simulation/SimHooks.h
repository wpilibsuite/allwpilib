/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/simulation/MockHooks.h>

namespace frc {
namespace sim {

inline void SetRuntimeType(HAL_RuntimeType type) {
  HALSIM_SetRuntimeType(type);
}

inline void WaitForProgramStart() { HALSIM_WaitForProgramStart(); }

inline void SetProgramStarted() { HALSIM_SetProgramStarted(); }

inline bool GetProgramStarted() { return HALSIM_GetProgramStarted(); }

inline void RestartTiming() { HALSIM_RestartTiming(); }

inline void PauseTiming() { HALSIM_PauseTiming(); }

inline void ResumeTiming() { HALSIM_ResumeTiming(); }

inline bool IsTimingPaused() { return HALSIM_IsTimingPaused(); }

inline void StepTiming(uint64_t delta) { HALSIM_StepTiming(delta); }

}  // namespace sim
}  // namespace frc
