// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/commands/InstantCommand.h"

#include <utility>

using namespace frc;

InstantCommand::InstantCommand(std::string_view name) : Command(name) {}

InstantCommand::InstantCommand(Subsystem& subsystem) : Command(subsystem) {}

InstantCommand::InstantCommand(std::string_view name, Subsystem& subsystem)
    : Command(name, subsystem) {}

InstantCommand::InstantCommand(std::function<void()> func)
    : m_func(std::move(func)) {}

InstantCommand::InstantCommand(Subsystem& subsystem, std::function<void()> func)
    : InstantCommand(subsystem) {
  m_func = func;
}

InstantCommand::InstantCommand(std::string_view name,
                               std::function<void()> func)
    : InstantCommand(name) {
  m_func = func;
}

InstantCommand::InstantCommand(std::string_view name, Subsystem& subsystem,
                               std::function<void()> func)
    : InstantCommand(name, subsystem) {
  m_func = func;
}

void InstantCommand::_Initialize() {
  Command::_Initialize();
  if (m_func) {
    m_func();
  }
}

bool InstantCommand::IsFinished() {
  return true;
}
