// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/NotifierCommand.h"

#include <utility>

using namespace frc2;

NotifierCommand::NotifierCommand(std::function<void()> toRun,
                                 units::second_t period,
                                 Requirements requirements)
    : m_toRun(toRun), m_notifier{std::move(toRun)}, m_period{period} {
  AddRequirements(requirements);
}

NotifierCommand::NotifierCommand(NotifierCommand&& other)
    : CommandHelper(std::move(other)),
      m_toRun(other.m_toRun),
      m_notifier(other.m_toRun),
      m_period(other.m_period) {}

NotifierCommand::NotifierCommand(const NotifierCommand& other)
    : CommandHelper(other),
      m_toRun(other.m_toRun),
      m_notifier(frc::Notifier(other.m_toRun)),
      m_period(other.m_period) {}

void NotifierCommand::Initialize() {
  m_notifier.StartPeriodic(m_period);
}

void NotifierCommand::End(bool interrupted) {
  m_notifier.Stop();
}
