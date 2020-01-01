/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc2/command/StartEndCommand.h"

using namespace frc2;

StartEndCommand::StartEndCommand(std::function<void()> onInit,
                                 std::function<void()> onEnd,
                                 std::initializer_list<Subsystem*> requirements)
    : m_onInit{std::move(onInit)}, m_onEnd{std::move(onEnd)} {
  AddRequirements(requirements);
}

StartEndCommand::StartEndCommand(std::function<void()> onInit,
                                 std::function<void()> onEnd,
                                 wpi::ArrayRef<Subsystem*> requirements)
    : m_onInit{std::move(onInit)}, m_onEnd{std::move(onEnd)} {
  AddRequirements(requirements);
}

StartEndCommand::StartEndCommand(const StartEndCommand& other)
    : CommandHelper(other) {
  m_onInit = other.m_onInit;
  m_onEnd = other.m_onEnd;
}

void StartEndCommand::Initialize() { m_onInit(); }

void StartEndCommand::End(bool interrupted) { m_onEnd(); }
