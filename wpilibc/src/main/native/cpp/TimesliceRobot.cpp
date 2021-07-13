// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/TimesliceRobot.hpp"

#include <frc/Threads.h>

#include "frc/Errors.h"
#include "frc/fmt/Units.h"

using namespace frc;

TimesliceRobot::TimesliceRobot(units::second_t robotPeriodicAllocation,
                               units::second_t controllerPeriod)
    : m_controllerPeriod{controllerPeriod},
      m_nextOffset{robotPeriodicAllocation} {
  SetPriority(kDefaultPriority);
}

void TimesliceRobot::SetPriority(int priority) {
  if (!frc::SetCurrentThreadPriority(true, priority)) {
    FRC_ReportError(err::Error,
                    "Setting TimesliceRobot RT priority to {} failed\n",
                    priority);
  }
}

void TimesliceRobot::Schedule(std::function<void()> func,
                              units::second_t allocation) {
  if (m_nextOffset > m_controllerPeriod) {
    FRC_ReportError(err::Error,
                    "Function scheduled at offset {} with allocation {} "
                    "exceeded controller period of {}\n",
                    m_nextOffset, allocation, m_controllerPeriod);
  }

  AddPeriodic(func, m_controllerPeriod, m_nextOffset);
  m_nextOffset += allocation;
}
