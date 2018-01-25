/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Commands/Subsystem.h"

#include "Commands/Command.h"
#include "Commands/Scheduler.h"
#include "LiveWindow/LiveWindow.h"
#include "SmartDashboard/SendableBuilder.h"
#include "WPIErrors.h"

using namespace frc;

/**
 * Creates a subsystem with the given name.
 *
 * @param name the name of the subsystem
 */
Subsystem::Subsystem(const llvm::Twine& name) {
  SetName(name, name);
  Scheduler::GetInstance()->RegisterSubsystem(this);
}

/**
 * Initialize the default command for this subsystem.
 *
 * This is meant to be the place to call SetDefaultCommand in a subsystem and
 * will be called on all the subsystems by the CommandBase method before the
 * program starts running by using the list of all registered Subsystems inside
 * the Scheduler.
 *
 * This should be overridden by a Subsystem that has a default Command
 */
void Subsystem::InitDefaultCommand() {}

/**
 * Sets the default command.  If this is not called or is called with null,
 * then there will be no default command for the subsystem.
 *
 * <b>WARNING:</b> This should <b>NOT</b> be called in a constructor if the
 * subsystem is a singleton.
 *
 * @param command the default command (or null if there should be none)
 */
void Subsystem::SetDefaultCommand(Command* command) {
  if (command == nullptr) {
    m_defaultCommand = nullptr;
  } else {
    bool found = false;
    Command::SubsystemSet requirements = command->GetRequirements();
    for (auto iter = requirements.begin(); iter != requirements.end(); iter++) {
      if (*iter == this) {
        found = true;
        break;
      }
    }

    if (!found) {
      wpi_setWPIErrorWithContext(
          CommandIllegalUse, "A default command must require the subsystem");
      return;
    }

    m_defaultCommand = command;
  }
}

/**
 * Returns the default command (or null if there is none).
 *
 * @return the default command
 */
Command* Subsystem::GetDefaultCommand() {
  if (!m_initializedDefaultCommand) {
    m_initializedDefaultCommand = true;
    InitDefaultCommand();
  }
  return m_defaultCommand;
}

/**
 * Returns the default command name, or empty string is there is none.
 *
 * @return the default command name
 */
llvm::StringRef Subsystem::GetDefaultCommandName() {
  Command* defaultCommand = GetDefaultCommand();
  if (defaultCommand) {
    return defaultCommand->GetName();
  } else {
    return llvm::StringRef();
  }
}

/**
 * Sets the current command.
 *
 * @param command the new current command
 */
void Subsystem::SetCurrentCommand(Command* command) {
  m_currentCommand = command;
  m_currentCommandChanged = true;
}

/**
 * Returns the command which currently claims this subsystem.
 *
 * @return the command which currently claims this subsystem
 */
Command* Subsystem::GetCurrentCommand() const { return m_currentCommand; }

/**
 * Returns the current command name, or empty string if no current command.
 *
 * @return the current command name
 */
llvm::StringRef Subsystem::GetCurrentCommandName() const {
  Command* currentCommand = GetCurrentCommand();
  if (currentCommand) {
    return currentCommand->GetName();
  } else {
    return llvm::StringRef();
  }
}

/**
 * When the run method of the scheduler is called this method will be called.
 */
void Subsystem::Periodic() {}

/**
 * Call this to alert Subsystem that the current command is actually the
 * command.
 *
 * Sometimes, the Subsystem is told that it has no command while the Scheduler
 * is going through the loop, only to be soon after given a new one. This will
 * avoid that situation.
 */
void Subsystem::ConfirmCommand() {
  if (m_currentCommandChanged) m_currentCommandChanged = false;
}

/**
 * Associate a Sendable with this Subsystem.
 * Also update the child's name.
 *
 * @param name name to give child
 * @param child sendable
 */
void Subsystem::AddChild(const llvm::Twine& name,
                         std::shared_ptr<Sendable> child) {
  AddChild(name, *child);
}

/**
 * Associate a Sendable with this Subsystem.
 * Also update the child's name.
 *
 * @param name name to give child
 * @param child sendable
 */
void Subsystem::AddChild(const llvm::Twine& name, Sendable* child) {
  AddChild(name, *child);
}

/**
 * Associate a Sendable with this Subsystem.
 * Also update the child's name.
 *
 * @param name name to give child
 * @param child sendable
 */
void Subsystem::AddChild(const llvm::Twine& name, Sendable& child) {
  child.SetName(GetSubsystem(), name);
  LiveWindow::GetInstance()->Add(&child);
}

/**
 * Associate a {@link Sendable} with this Subsystem.
 *
 * @param child sendable
 */
void Subsystem::AddChild(std::shared_ptr<Sendable> child) { AddChild(*child); }

/**
 * Associate a {@link Sendable} with this Subsystem.
 *
 * @param child sendable
 */
void Subsystem::AddChild(Sendable* child) { AddChild(*child); }

/**
 * Associate a {@link Sendable} with this Subsystem.
 *
 * @param child sendable
 */
void Subsystem::AddChild(Sendable& child) {
  child.SetSubsystem(GetSubsystem());
  LiveWindow::GetInstance()->Add(&child);
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
