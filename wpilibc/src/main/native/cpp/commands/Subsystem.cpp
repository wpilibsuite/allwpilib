/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/commands/Subsystem.h"

#include "frc/WPIErrors.h"
#include "frc/commands/Command.h"
#include "frc/commands/Scheduler.h"
#include "frc/livewindow/LiveWindow.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

Subsystem::Subsystem(const wpi::Twine& name) {
  SetName(name, name);
  Scheduler::GetInstance()->RegisterSubsystem(this);
}

void Subsystem::SetDefaultCommand(Command* command) {
  if (command == nullptr) {
    m_defaultCommand = nullptr;
  } else {
    const auto& reqs = command->GetRequirements();
    if (std::find(reqs.begin(), reqs.end(), this) == reqs.end()) {
      wpi_setWPIErrorWithContext(
          CommandIllegalUse, "A default command must require the subsystem");
      return;
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

wpi::StringRef Subsystem::GetDefaultCommandName() {
  Command* defaultCommand = GetDefaultCommand();
  if (defaultCommand) {
    return defaultCommand->GetName();
  } else {
    return wpi::StringRef();
  }
}

void Subsystem::SetCurrentCommand(Command* command) {
  m_currentCommand = command;
  m_currentCommandChanged = true;
}

Command* Subsystem::GetCurrentCommand() const { return m_currentCommand; }

wpi::StringRef Subsystem::GetCurrentCommandName() const {
  Command* currentCommand = GetCurrentCommand();
  if (currentCommand) {
    return currentCommand->GetName();
  } else {
    return wpi::StringRef();
  }
}

void Subsystem::Periodic() {}

void Subsystem::InitDefaultCommand() {}

void Subsystem::AddChild(const wpi::Twine& name,
                         std::shared_ptr<Sendable> child) {
  AddChild(name, *child);
}

void Subsystem::AddChild(const wpi::Twine& name, Sendable* child) {
  AddChild(name, *child);
}

void Subsystem::AddChild(const wpi::Twine& name, Sendable& child) {
  child.SetName(GetSubsystem(), name);
  LiveWindow::GetInstance()->Add(&child);
}

void Subsystem::AddChild(std::shared_ptr<Sendable> child) { AddChild(*child); }

void Subsystem::AddChild(Sendable* child) { AddChild(*child); }

void Subsystem::AddChild(Sendable& child) {
  child.SetSubsystem(GetSubsystem());
  LiveWindow::GetInstance()->Add(&child);
}

void Subsystem::ConfirmCommand() {
  if (m_currentCommandChanged) m_currentCommandChanged = false;
}

void Subsystem::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Subsystem");

  builder.AddBooleanProperty(
      ".hasDefault", [=]() { return m_defaultCommand != nullptr; }, nullptr);
  builder.AddStringProperty(".default",
                            [=]() { return GetDefaultCommandName(); }, nullptr);

  builder.AddBooleanProperty(
      ".hasCommand", [=]() { return m_currentCommand != nullptr; }, nullptr);
  builder.AddStringProperty(".command",
                            [=]() { return GetCurrentCommandName(); }, nullptr);
}
