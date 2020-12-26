// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/commands/CommandGroupEntry.h"

#include "frc/commands/Command.h"

using namespace frc;

CommandGroupEntry::CommandGroupEntry(Command* command, Sequence state,
                                     double timeout)
    : m_timeout(timeout), m_command(command), m_state(state) {}

bool CommandGroupEntry::IsTimedOut() const {
  if (m_timeout < 0.0)
    return false;
  double time = m_command->TimeSinceInitialized();
  if (time == 0.0)
    return false;
  return time >= m_timeout;
}
