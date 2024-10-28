// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/Subsystem.h"

#include <string>
#include <utility>

#include <wpi/Demangle.h>

#include "frc2/command/CommandPtr.h"
#include "frc2/command/Commands.h"

using namespace frc2;
Subsystem::~Subsystem() {
  CommandScheduler::GetInstance().UnregisterSubsystem(this);
}

void Subsystem::Periodic() {}

void Subsystem::SimulationPeriodic() {}

std::string Subsystem::GetName() const {
  return wpi::GetTypeName(*this);
}

void Subsystem::SetDefaultCommand(CommandPtr&& defaultCommand) {
  CommandScheduler::GetInstance().SetDefaultCommand(this,
                                                    std::move(defaultCommand));
}

void Subsystem::RemoveDefaultCommand() {
  CommandScheduler::GetInstance().RemoveDefaultCommand(this);
}

Command* Subsystem::GetDefaultCommand() const {
  return CommandScheduler::GetInstance().GetDefaultCommand(this);
}

Command* Subsystem::GetCurrentCommand() const {
  return CommandScheduler::GetInstance().Requiring(this);
}

void Subsystem::Register() {
  return CommandScheduler::GetInstance().RegisterSubsystem(this);
}

CommandPtr Subsystem::RunOnce(std::function<void()> action) {
  return cmd::RunOnce(std::move(action), {this});
}

CommandPtr Subsystem::Run(std::function<void()> action) {
  return cmd::Run(std::move(action), {this});
}

CommandPtr Subsystem::StartEnd(std::function<void()> start,
                               std::function<void()> end) {
  return cmd::StartEnd(std::move(start), std::move(end), {this});
}

CommandPtr Subsystem::RunEnd(std::function<void()> run,
                             std::function<void()> end) {
  return cmd::RunEnd(std::move(run), std::move(end), {this});
}

CommandPtr Subsystem::StartRun(std::function<void()> start,
                               std::function<void()> run) {
  return cmd::StartRun(std::move(start), std::move(run), {this});
}

CommandPtr Subsystem::Defer(wpi::unique_function<CommandPtr()> supplier) {
  return cmd::Defer(std::move(supplier), {this});
}
