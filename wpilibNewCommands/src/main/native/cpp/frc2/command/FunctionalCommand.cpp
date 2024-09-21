// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/FunctionalCommand.h"

#include <utility>

using namespace frc2;

FunctionalCommand::FunctionalCommand(std::function<void()> onInit,
                                     std::function<void()> onExecute,
                                     std::function<void(bool)> onEnd,
                                     std::function<bool()> isFinished,
                                     Requirements requirements)
    : m_onInit{std::move(onInit)},
      m_onExecute{std::move(onExecute)},
      m_onEnd{std::move(onEnd)},
      m_isFinished{std::move(isFinished)} {
  AddRequirements(requirements);
}

void FunctionalCommand::Initialize() {
  m_onInit();
}

void FunctionalCommand::Execute() {
  m_onExecute();
}

void FunctionalCommand::End(bool interrupted) {
  m_onEnd(interrupted);
}

bool FunctionalCommand::IsFinished() {
  return m_isFinished();
}
