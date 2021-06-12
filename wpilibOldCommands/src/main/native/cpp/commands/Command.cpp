// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/commands/Command.h"

#include <typeinfo>

#include "frc/Errors.h"
#include "frc/RobotState.h"
#include "frc/Timer.h"
#include "frc/commands/CommandGroup.h"
#include "frc/commands/Scheduler.h"
#include "frc/livewindow/LiveWindow.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

int Command::m_commandCounter = 0;

Command::Command() : Command("", -1_s) {}

Command::Command(std::string_view name) : Command(name, -1_s) {}

Command::Command(units::second_t timeout) : Command("", timeout) {}

Command::Command(Subsystem& subsystem) : Command("", -1_s) {
  Requires(&subsystem);
}

Command::Command(std::string_view name, units::second_t timeout) {
  // We use -1.0 to indicate no timeout.
  if (timeout < 0_s && timeout != -1_s) {
    throw FRC_MakeError(err::ParameterOutOfRange, "timeout {} < 0 s",
                        timeout.to<double>());
  }

  m_timeout = timeout;

  // If name contains an empty string
  if (name.empty()) {
    SendableRegistry::GetInstance().Add(
        this, fmt::format("Command_{}", typeid(*this).name()));
  } else {
    SendableRegistry::GetInstance().Add(this, name);
  }
}

Command::Command(std::string_view name, Subsystem& subsystem)
    : Command(name, -1_s) {
  Requires(&subsystem);
}

Command::Command(units::second_t timeout, Subsystem& subsystem)
    : Command("", timeout) {
  Requires(&subsystem);
}

Command::Command(std::string_view name, units::second_t timeout,
                 Subsystem& subsystem)
    : Command(name, timeout) {
  Requires(&subsystem);
}

units::second_t Command::TimeSinceInitialized() const {
  if (m_startTime < 0_s) {
    return 0_s;
  } else {
    return Timer::GetFPGATimestamp() - m_startTime;
  }
}

void Command::Requires(Subsystem* subsystem) {
  if (!AssertUnlocked("Can not add new requirement to command")) {
    return;
  }

  if (subsystem != nullptr) {
    m_requirements.insert(subsystem);
  } else {
    throw FRC_MakeError(err::NullParameter, "{}", "subsystem");
  }
}

void Command::Start() {
  LockChanges();
  if (m_parent != nullptr) {
    throw FRC_MakeError(
        err::CommandIllegalUse, "{}",
        "Can not start a command that is part of a command group");
  }

  m_completed = false;
  Scheduler::GetInstance()->AddCommand(this);
}

bool Command::Run() {
  if (!m_runWhenDisabled && m_parent == nullptr && RobotState::IsDisabled()) {
    Cancel();
  }

  if (IsCanceled()) {
    return false;
  }

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
  if (m_parent != nullptr) {
    throw FRC_MakeError(
        err::CommandIllegalUse, "{}",
        "Can not cancel a command that is part of a command group");
  }

  _Cancel();
}

bool Command::IsRunning() const {
  return m_running;
}

bool Command::IsInitialized() const {
  return m_initialized;
}

bool Command::IsCompleted() const {
  return m_completed;
}

bool Command::IsCanceled() const {
  return m_canceled;
}

bool Command::IsInterruptible() const {
  return m_interruptible;
}

void Command::SetInterruptible(bool interruptible) {
  m_interruptible = interruptible;
}

bool Command::DoesRequire(Subsystem* system) const {
  return m_requirements.count(system) > 0;
}

const Command::SubsystemSet& Command::GetRequirements() const {
  return m_requirements;
}

CommandGroup* Command::GetGroup() const {
  return m_parent;
}

void Command::SetRunWhenDisabled(bool run) {
  m_runWhenDisabled = run;
}

bool Command::WillRunWhenDisabled() const {
  return m_runWhenDisabled;
}

int Command::GetID() const {
  return m_commandID;
}

void Command::SetTimeout(units::second_t timeout) {
  if (timeout < 0_s) {
    throw FRC_MakeError(err::ParameterOutOfRange, "timeout {} < 0 s",
                        timeout.to<double>());
  } else {
    m_timeout = timeout;
  }
}

bool Command::IsTimedOut() const {
  return m_timeout != -1_s && TimeSinceInitialized() >= m_timeout;
}

bool Command::AssertUnlocked(std::string_view message) {
  if (m_locked) {
    throw FRC_MakeError(
        err::CommandIllegalUse,
        "{} after being started or being added to a command group", message);
  }
  return true;
}

void Command::SetParent(CommandGroup* parent) {
  if (parent == nullptr) {
    throw FRC_MakeError(err::NullParameter, "{}", "parent");
  } else if (m_parent != nullptr) {
    throw FRC_MakeError(err::CommandIllegalUse, "{}",
                        "Can not give command to a command group after "
                        "already being put in a command group");
  } else {
    LockChanges();
    m_parent = parent;
  }
}

bool Command::IsParented() const {
  return m_parent != nullptr;
}

void Command::ClearRequirements() {
  m_requirements.clear();
}

void Command::Initialize() {}

void Command::Execute() {}

void Command::End() {}

void Command::Interrupted() {
  End();
}

void Command::_Initialize() {
  m_completed = false;
}

void Command::_Interrupted() {
  m_completed = true;
}

void Command::_Execute() {}

void Command::_End() {
  m_completed = true;
}

void Command::_Cancel() {
  if (IsRunning()) {
    m_canceled = true;
  }
}

void Command::LockChanges() {
  m_locked = true;
}

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
  m_startTime = -1_s;
  m_completed = false;
}

void Command::StartTiming() {
  m_startTime = Timer::GetFPGATimestamp();
}

std::string Command::GetName() const {
  return SendableRegistry::GetInstance().GetName(this);
}

void Command::SetName(std::string_view name) {
  SendableRegistry::GetInstance().SetName(this, name);
}

std::string Command::GetSubsystem() const {
  return SendableRegistry::GetInstance().GetSubsystem(this);
}

void Command::SetSubsystem(std::string_view name) {
  SendableRegistry::GetInstance().SetSubsystem(this, name);
}

void Command::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Command");
  builder.AddStringProperty(
      ".name", [=] { return SendableRegistry::GetInstance().GetName(this); },
      nullptr);
  builder.AddBooleanProperty(
      "running", [=] { return IsRunning(); },
      [=](bool value) {
        if (value) {
          if (!IsRunning()) {
            Start();
          }
        } else {
          if (IsRunning()) {
            Cancel();
          }
        }
      });
  builder.AddBooleanProperty(
      ".isParented", [=] { return IsParented(); }, nullptr);
}
