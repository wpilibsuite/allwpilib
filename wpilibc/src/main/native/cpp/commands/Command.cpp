/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/commands/Command.h"

#include <typeinfo>

#include "frc/RobotState.h"
#include "frc/Timer.h"
#include "frc/WPIErrors.h"
#include "frc/commands/CommandGroup.h"
#include "frc/commands/Scheduler.h"
#include "frc/livewindow/LiveWindow.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

int Command::m_commandCounter = 0;

Command::Command() : Command("", -1.0) {}

Command::Command(const wpi::Twine& name) : Command(name, -1.0) {}

Command::Command(double timeout) : Command("", timeout) {}

Command::Command(Subsystem& subsystem) : Command("", -1.0) {
  Requires(&subsystem);
}

Command::Command(const wpi::Twine& name, double timeout) : SendableBase(false) {
  // We use -1.0 to indicate no timeout.
  if (timeout < 0.0 && timeout != -1.0)
    wpi_setWPIErrorWithContext(ParameterOutOfRange, "timeout < 0.0");

  m_timeout = timeout;

  // If name contains an empty string
  if (name.isTriviallyEmpty() ||
      (name.isSingleStringRef() && name.getSingleStringRef().empty())) {
    SetName("Command_" + wpi::Twine(typeid(*this).name()));
  } else {
    SetName(name);
  }
}

Command::Command(const wpi::Twine& name, Subsystem& subsystem)
    : Command(name, -1.0) {
  Requires(&subsystem);
}

Command::Command(double timeout, Subsystem& subsystem) : Command("", timeout) {
  Requires(&subsystem);
}

Command::Command(const wpi::Twine& name, double timeout, Subsystem& subsystem)
    : Command(name, timeout) {
  Requires(&subsystem);
}

double Command::TimeSinceInitialized() const {
  if (m_startTime < 0.0)
    return 0.0;
  else
    return Timer::GetFPGATimestamp() - m_startTime;
}

void Command::Requires(Subsystem* subsystem) {
  if (!AssertUnlocked("Can not add new requirement to command")) return;

  if (subsystem != nullptr)
    m_requirements.insert(subsystem);
  else
    wpi_setWPIErrorWithContext(NullParameter, "subsystem");
}

void Command::Start() {
  LockChanges();
  if (m_parent != nullptr)
    wpi_setWPIErrorWithContext(
        CommandIllegalUse,
        "Can not start a command that is part of a command group");

  m_completed = false;
  Scheduler::GetInstance()->AddCommand(this);
}

bool Command::Run() {
  if (!m_runWhenDisabled && m_parent == nullptr && RobotState::IsDisabled())
    Cancel();

  if (IsCanceled()) return false;

  if (!m_initialized) {
    m_initialized = true;
    StartTiming();
    _Initialize();
    Initialize();
  }
  _Execute();
  Execute();
  return !IsFinished();
}

void Command::Cancel() {
  if (m_parent != nullptr)
    wpi_setWPIErrorWithContext(
        CommandIllegalUse,
        "Can not cancel a command that is part of a command group");

  _Cancel();
}

bool Command::IsRunning() const { return m_running; }

bool Command::IsInitialized() const { return m_initialized; }

bool Command::IsCompleted() const { return m_completed; }

bool Command::IsCanceled() const { return m_canceled; }

bool Command::IsInterruptible() const { return m_interruptible; }

void Command::SetInterruptible(bool interruptible) {
  m_interruptible = interruptible;
}

bool Command::DoesRequire(Subsystem* system) const {
  return m_requirements.count(system) > 0;
}

const Command::SubsystemSet& Command::GetRequirements() const {
  return m_requirements;
}

CommandGroup* Command::GetGroup() const { return m_parent; }

void Command::SetRunWhenDisabled(bool run) { m_runWhenDisabled = run; }

bool Command::WillRunWhenDisabled() const { return m_runWhenDisabled; }

int Command::GetID() const { return m_commandID; }

void Command::SetTimeout(double timeout) {
  if (timeout < 0.0)
    wpi_setWPIErrorWithContext(ParameterOutOfRange, "timeout < 0.0");
  else
    m_timeout = timeout;
}

bool Command::IsTimedOut() const {
  return m_timeout != -1 && TimeSinceInitialized() >= m_timeout;
}

bool Command::AssertUnlocked(const std::string& message) {
  if (m_locked) {
    std::string buf =
        message + " after being started or being added to a command group";
    wpi_setWPIErrorWithContext(CommandIllegalUse, buf);
    return false;
  }
  return true;
}

void Command::SetParent(CommandGroup* parent) {
  if (parent == nullptr) {
    wpi_setWPIErrorWithContext(NullParameter, "parent");
  } else if (m_parent != nullptr) {
    wpi_setWPIErrorWithContext(CommandIllegalUse,
                               "Can not give command to a command group after "
                               "already being put in a command group");
  } else {
    LockChanges();
    m_parent = parent;
  }
}

bool Command::IsParented() const { return m_parent != nullptr; }

void Command::ClearRequirements() { m_requirements.clear(); }

void Command::Initialize() {}

void Command::Execute() {}

void Command::End() {}

void Command::Interrupted() { End(); }

void Command::_Initialize() { m_completed = false; }

void Command::_Interrupted() { m_completed = true; }

void Command::_Execute() {}

void Command::_End() { m_completed = true; }

void Command::_Cancel() {
  if (IsRunning()) m_canceled = true;
}

void Command::LockChanges() { m_locked = true; }

void Command::Removed() {
  if (m_initialized) {
    if (IsCanceled()) {
      Interrupted();
      _Interrupted();
    } else {
      End();
      _End();
    }
  }
  m_initialized = false;
  m_canceled = false;
  m_running = false;
  m_completed = true;
}

void Command::StartRunning() {
  m_running = true;
  m_startTime = -1;
  m_completed = false;
}

void Command::StartTiming() { m_startTime = Timer::GetFPGATimestamp(); }

void Command::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Command");
  builder.AddStringProperty(".name", [=]() { return GetName(); }, nullptr);
  builder.AddBooleanProperty("running", [=]() { return IsRunning(); },
                             [=](bool value) {
                               if (value) {
                                 if (!IsRunning()) Start();
                               } else {
                                 if (IsRunning()) Cancel();
                               }
                             });
  builder.AddBooleanProperty(".isParented", [=]() { return IsParented(); },
                             nullptr);
}
