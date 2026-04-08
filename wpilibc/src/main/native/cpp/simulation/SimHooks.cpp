// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/SimHooks.hpp"

#include "wpi/hal/simulation/MockHooks.hpp"

namespace wpi::sim {

void SetRuntimeType(HAL_RuntimeType type) {
  HALSIM_SetRuntimeType(type);
}

void WaitForProgramStart() {
  HALSIM_WaitForProgramStart();
}

void SetProgramStarted(bool started) {
  HALSIM_SetProgramStarted(started);
}

bool GetProgramStarted() {
  return HALSIM_GetProgramStarted();
}

void SetProgramState(wpi::hal::ControlWord controlWord) {
  wpi::hal::sim::SetProgramState(controlWord);
}

wpi::hal::ControlWord GetProgramState() {
  return wpi::hal::sim::GetProgramState();
}

void RestartTiming() {
  HALSIM_RestartTiming();
}

void PauseTiming() {
  HALSIM_PauseTiming();
}

void ResumeTiming() {
  HALSIM_ResumeTiming();
}

bool IsTimingPaused() {
  return HALSIM_IsTimingPaused();
}

void StepTiming(wpi::units::second_t delta) {
  HALSIM_StepTiming(static_cast<uint64_t>(delta.value() * 1e6));
}

void StepTimingAsync(wpi::units::second_t delta) {
  HALSIM_StepTimingAsync(static_cast<uint64_t>(delta.value() * 1e6));
}

}  // namespace wpi::sim
