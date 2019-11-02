/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
void CommandState::StartRunning() { m_startTime = -1; }
double CommandState::TimeSinceInitialized() const {
  return m_startTime != -1 ? frc::Timer::GetFPGATimestamp() - m_startTime : -1;
}
