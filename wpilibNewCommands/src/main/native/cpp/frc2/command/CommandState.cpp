// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/CommandState.h"

#include <frc/Timer.h>

using namespace frc2;
CommandState::CommandState(bool interruptible)
    : m_interruptible{interruptible} {
  StartTiming();
  StartRunning();
}

void CommandState::StartTiming() {
  m_startTime = frc::Timer::GetFPGATimestamp();
}

void CommandState::StartRunning() {
  m_startTime = -1_s;
}

units::second_t CommandState::TimeSinceInitialized() const {
  return m_startTime != -1_s ? frc::Timer::GetFPGATimestamp() - m_startTime
                             : -1_s;
}
