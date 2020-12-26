// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
