// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/commands/Subsystem.h"

#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"
#include "frc/commands/Command.h"
#include "frc/commands/Scheduler.h"
#include "frc/livewindow/LiveWindow.h"

using namespace frc;

Subsystem::Subsystem(std::string_view name) {
  wpi::SendableRegistry::AddLW(this, name, name);
  Scheduler::GetInstance()->RegisterSubsystem(this);
}

void Subsystem::SetDefaultCommand(Command* command) {
  if (command == nullptr) {
    m_defaultCommand = nullptr;
  } else {
    const auto& reqs = command->GetRequirements();
    if (std::find(reqs.begin(), reqs.end(), this) == reqs.end()) {
      throw FRC_MakeError(err::CommandIllegalUse, "{}",
                          "A default command must require the subsystem");
    }

    m_defaultCommand = command;
  }
}

Command* Subsystem::GetDefaultCommand() {
  if (!m_initializedDefaultCommand) {
    m_initializedDefaultCommand = true;
    InitDefaultCommand();
  }
  return m_defaultCommand;
}

std::string Subsystem::GetDefaultCommandName() {
  Command* defaultCommand = GetDefaultCommand();
  if (defaultCommand) {
    return wpi::SendableRegistry::GetName(defaultCommand);
  } else {
    return {};
  }
}

void Subsystem::SetCurrentCommand(Command* command) {
  m_currentCommand = command;
  m_currentCommandChanged = true;
}

Command* Subsystem::GetCurrentCommand() const {
  return m_currentCommand;
}

std::string Subsystem::GetCurrentCommandName() const {
  Command* currentCommand = GetCurrentCommand();
  if (currentCommand) {
    return wpi::SendableRegistry::GetName(currentCommand);
  } else {
    return {};
  }
}

void Subsystem::Periodic() {}

void Subsystem::InitDefaultCommand() {}

std::string Subsystem::GetName() const {
  return wpi::SendableRegistry::GetName(this);
}

void Subsystem::SetName(std::string_view name) {
  wpi::SendableRegistry::SetName(this, name);
}

std::string Subsystem::GetSubsystem() const {
  return wpi::SendableRegistry::GetSubsystem(this);
}

void Subsystem::SetSubsystem(std::string_view name) {
  wpi::SendableRegistry::SetSubsystem(this, name);
}

void Subsystem::AddChild(std::string_view name,
                         std::shared_ptr<Sendable> child) {
  AddChild(name, *child);
}

void Subsystem::AddChild(std::string_view name, wpi::Sendable* child) {
  AddChild(name, *child);
}

void Subsystem::AddChild(std::string_view name, wpi::Sendable& child) {
  wpi::SendableRegistry::AddLW(&child,
                               wpi::SendableRegistry::GetSubsystem(this), name);
}

void Subsystem::AddChild(std::shared_ptr<wpi::Sendable> child) {
  AddChild(*child);
}

void Subsystem::AddChild(wpi::Sendable* child) {
  AddChild(*child);
}

void Subsystem::AddChild(wpi::Sendable& child) {
  wpi::SendableRegistry::SetSubsystem(
      &child, wpi::SendableRegistry::GetSubsystem(this));
  wpi::SendableRegistry::EnableLiveWindow(&child);
}

void Subsystem::ConfirmCommand() {
  if (m_currentCommandChanged) {
    m_currentCommandChanged = false;
  }
}

void Subsystem::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Subsystem");

  builder.AddBooleanProperty(
      ".hasDefault", [=] { return m_defaultCommand != nullptr; }, nullptr);
  builder.AddStringProperty(
      ".default", [=] { return GetDefaultCommandName(); }, nullptr);

  builder.AddBooleanProperty(
      ".hasCommand", [=] { return m_currentCommand != nullptr; }, nullptr);
  builder.AddStringProperty(
      ".command", [=] { return GetCurrentCommandName(); }, nullptr);
}
