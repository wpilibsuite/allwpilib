/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/NotifierCommand.h"

using namespace frc2;

NotifierCommand::NotifierCommand(std::function<void()> toRun,
                                 units::second_t period,
                                 std::initializer_list<Subsystem*> requirements)
    : m_toRun(toRun), m_notifier{std::move(toRun)}, m_period{period} {
  AddRequirements(requirements);
}

NotifierCommand::NotifierCommand(std::function<void()> toRun,
                                 units::second_t period,
                                 wpi::ArrayRef<Subsystem*> requirements)
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

void NotifierCommand::Initialize() { m_notifier.StartPeriodic(m_period); }

void NotifierCommand::End(bool interrupted) { m_notifier.Stop(); }
