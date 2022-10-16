// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/Trigger.h"

#include <frc/filter/Debouncer.h>

#include "frc2/command/InstantCommand.h"

using namespace frc;
using namespace frc2;

Trigger::Trigger(const Trigger& other) = default;

Trigger Trigger::OnTrue(Command* command) {
  m_event.Rising().IfHigh([command] { command->Schedule(); });
  return *this;
}

Trigger Trigger::OnTrue(CommandPtr&& command) {
  m_event.Rising().IfHigh(
      [command = std::move(command)] { command.Schedule(); });
  return *this;
}

Trigger Trigger::OnFalse(Command* command) {
  m_event.Falling().IfHigh([command] { command->Schedule(); });
  return *this;
}

Trigger Trigger::OnFalse(CommandPtr&& command) {
  m_event.Falling().IfHigh(
      [command = std::move(command)] { command.Schedule(); });
  return *this;
}

Trigger Trigger::WhileTrue(Command* command) {
  m_event.Rising().IfHigh([command] { command->Schedule(); });
  m_event.Falling().IfHigh([command] { command->Cancel(); });
  return *this;
}

Trigger Trigger::WhileTrue(CommandPtr&& command) {
  auto ptr = std::make_shared<CommandPtr>(std::move(command));
  m_event.Rising().IfHigh([ptr] { ptr->Schedule(); });
  m_event.Falling().IfHigh([ptr] { ptr->Cancel(); });
  return *this;
}

Trigger Trigger::WhileFalse(Command* command) {
  m_event.Falling().IfHigh([command] { command->Schedule(); });
  m_event.Rising().IfHigh([command] { command->Cancel(); });
  return *this;
}

Trigger Trigger::WhileFalse(CommandPtr&& command) {
  auto ptr = std::make_shared<CommandPtr>(std::move(command));
  m_event.Falling().IfHigh([ptr] { ptr->Schedule(); });
  m_event.Rising().IfHigh([ptr] { ptr->Cancel(); });
  return *this;
}

Trigger Trigger::ToggleOnTrue(Command* command) {
  m_event.Rising().IfHigh([command] {
    if (command->IsScheduled()) {
      command->Cancel();
    } else {
      command->Schedule();
    }
  });
  return *this;
}

Trigger Trigger::ToggleOnTrue(CommandPtr&& command) {
  m_event.Rising().IfHigh([command = std::move(command)] {
    if (command.IsScheduled()) {
      command.Cancel();
    } else {
      command.Schedule();
    }
  });
  return *this;
}

Trigger Trigger::ToggleOnFalse(Command* command) {
  m_event.Falling().IfHigh([command] {
    if (command->IsScheduled()) {
      command->Cancel();
    } else {
      command->Schedule();
    }
  });
  return *this;
}

Trigger Trigger::ToggleOnFalse(CommandPtr&& command) {
  m_event.Falling().IfHigh([command = std::move(command)] {
    if (command.IsScheduled()) {
      command.Cancel();
    } else {
      command.Schedule();
    }
  });
  return *this;
}

Trigger Trigger::WhenActive(Command* command) {
  m_event.Rising().IfHigh([command] { command->Schedule(); });
  return *this;
}

Trigger Trigger::WhenActive(std::function<void()> toRun,
                            std::initializer_list<Subsystem*> requirements) {
  return WhenActive(std::move(toRun),
                    {requirements.begin(), requirements.end()});
}

Trigger Trigger::WhenActive(std::function<void()> toRun,
                            std::span<Subsystem* const> requirements) {
  return WhenActive(InstantCommand(std::move(toRun), requirements));
}

Trigger Trigger::WhileActiveContinous(Command* command) {
  m_event.IfHigh([command] { command->Schedule(); });
  m_event.Falling().IfHigh([command] { command->Cancel(); });
  return *this;
}

Trigger Trigger::WhileActiveContinous(
    std::function<void()> toRun,
    std::initializer_list<Subsystem*> requirements) {
  return WhileActiveContinous(std::move(toRun),
                              {requirements.begin(), requirements.end()});
}

Trigger Trigger::WhileActiveContinous(
    std::function<void()> toRun, std::span<Subsystem* const> requirements) {
  return WhileActiveContinous(InstantCommand(std::move(toRun), requirements));
}

Trigger Trigger::WhileActiveOnce(Command* command) {
  m_event.Rising().IfHigh([command] { command->Schedule(); });
  m_event.Falling().IfHigh([command] { command->Cancel(); });
  return *this;
}

Trigger Trigger::WhenInactive(Command* command) {
  m_event.Falling().IfHigh([command] { command->Schedule(); });
  return *this;
}

Trigger Trigger::WhenInactive(std::function<void()> toRun,
                              std::initializer_list<Subsystem*> requirements) {
  return WhenInactive(std::move(toRun),
                      {requirements.begin(), requirements.end()});
}

Trigger Trigger::WhenInactive(std::function<void()> toRun,
                              std::span<Subsystem* const> requirements) {
  return WhenInactive(InstantCommand(std::move(toRun), requirements));
}

Trigger Trigger::ToggleWhenActive(Command* command) {
  m_event.Rising().IfHigh([command] {
    if (command->IsScheduled()) {
      command->Cancel();
    } else {
      command->Schedule();
    }
  });
  return *this;
}

Trigger Trigger::CancelWhenActive(Command* command) {
  m_event.Rising().IfHigh([command] { command->Cancel(); });
  return *this;
}

BooleanEvent Trigger::GetEvent() const {
  return m_event;
}
