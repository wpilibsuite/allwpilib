/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/FunctionalCommand.h"

using namespace frc2;

FunctionalCommand::FunctionalCommand(
    std::function<void()> onInit, std::function<void()> onExecute,
    std::function<void(bool)> onEnd, std::function<bool()> isFinished,
    std::initializer_list<Subsystem*> requirements)
    : m_onInit{std::move(onInit)},
      m_onExecute{std::move(onExecute)},
      m_onEnd{std::move(onEnd)},
      m_isFinished{std::move(isFinished)} {
  AddRequirements(requirements);
}

FunctionalCommand::FunctionalCommand(std::function<void()> onInit,
                                     std::function<void()> onExecute,
                                     std::function<void(bool)> onEnd,
                                     std::function<bool()> isFinished,
                                     wpi::ArrayRef<Subsystem*> requirements)
    : m_onInit{std::move(onInit)},
      m_onExecute{std::move(onExecute)},
      m_onEnd{std::move(onEnd)},
      m_isFinished{std::move(isFinished)} {
  AddRequirements(requirements);
}

void FunctionalCommand::Initialize() { m_onInit(); }

void FunctionalCommand::Execute() { m_onExecute(); }

void FunctionalCommand::End(bool interrupted) { m_onEnd(interrupted); }

bool FunctionalCommand::IsFinished() { return m_isFinished(); }
