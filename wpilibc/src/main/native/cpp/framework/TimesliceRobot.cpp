// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/TimesliceRobot.hpp"

#include "wpi/system/Errors.hpp"

using namespace wpi;

TimesliceRobot::TimesliceRobot(wpi::units::seconds<> robotPeriodicAllocation,
                               wpi::units::seconds<> controllerPeriod)
    : m_nextOffset{robotPeriodicAllocation},
      m_controllerPeriod{controllerPeriod} {}

void TimesliceRobot::Schedule(std::function<void()> func,
                              wpi::units::seconds<> allocation) {
  if (m_nextOffset + allocation > m_controllerPeriod) {
    throw WPILIB_MakeError(err::Error,
                           "Function scheduled at offset {} with allocation {} "
                           "exceeded controller period of {}\n",
                           m_nextOffset, allocation, m_controllerPeriod);
  }

  AddPeriodic(func, m_controllerPeriod, m_nextOffset);
  m_nextOffset += allocation;
}
