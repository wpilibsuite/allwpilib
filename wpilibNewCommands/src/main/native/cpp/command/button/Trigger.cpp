// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/command/button/Trigger.h"

#include <frc/filter/Debouncer.h>

#include "frc/command/InstantCommand.h"

using namespace frc;
using namespace frc;

Trigger::Trigger(const Trigger& other) = default;

Trigger Trigger::WhenActive(Command* command) {
  this->Rising().IfHigh([command] { command->Schedule(); });
  return *this;
}

Trigger Trigger::WhenActive(std::function<void()> toRun,
                            std::initializer_list<Subsystem*> requirements) {
  return WhenActive(std::move(toRun),
                    {requirements.begin(), requirements.end()});
}

Trigger Trigger::WhenActive(std::function<void()> toRun,
                            wpi::span<Subsystem* const> requirements) {
  return WhenActive(InstantCommand(std::move(toRun), requirements));
}

Trigger Trigger::WhileActiveContinous(Command* command) {
  this->IfHigh([command] { command->Schedule(); });
  this->Falling().IfHigh([command] { command->Cancel(); });
  return *this;
}

Trigger Trigger::WhileActiveContinous(
    std::function<void()> toRun,
    std::initializer_list<Subsystem*> requirements) {
  return WhileActiveContinous(std::move(toRun),
                              {requirements.begin(), requirements.end()});
}

Trigger Trigger::WhileActiveContinous(
    std::function<void()> toRun, wpi::span<Subsystem* const> requirements) {
  return WhileActiveContinous(InstantCommand(std::move(toRun), requirements));
}

Trigger Trigger::WhileActiveOnce(Command* command) {
  this->Rising().IfHigh([command] { command->Schedule(); });
  this->Falling().IfHigh([command] { command->Cancel(); });
  return *this;
}

Trigger Trigger::WhenInactive(Command* command) {
  this->Falling().IfHigh([command] { command->Schedule(); });
  return *this;
}

Trigger Trigger::WhenInactive(std::function<void()> toRun,
                              std::initializer_list<Subsystem*> requirements) {
  return WhenInactive(std::move(toRun),
                      {requirements.begin(), requirements.end()});
}

Trigger Trigger::WhenInactive(std::function<void()> toRun,
                              wpi::span<Subsystem* const> requirements) {
  return WhenInactive(InstantCommand(std::move(toRun), requirements));
}

Trigger Trigger::ToggleWhenActive(Command* command) {
  this->Rising().IfHigh([command] {
    if (command->IsScheduled()) {
      command->Cancel();
    } else {
      command->Schedule();
    }
  });
  return *this;
}

Trigger Trigger::CancelWhenActive(Command* command) {
  this->Rising().IfHigh([command] { command->Cancel(); });
  return *this;
}
