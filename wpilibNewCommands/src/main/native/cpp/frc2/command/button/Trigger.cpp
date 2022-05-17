// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/Trigger.h"

#include <frc/filter/Debouncer.h>

#include "frc2/command/InstantCommand.h"

using namespace frc2;

Trigger::Trigger(const Trigger& other) = default;

Trigger Trigger::WhenActive(Command* command, bool interruptible) {
  CommandScheduler::GetInstance().AddButton(
      [pressedLast = m_isActive(), *this, command, interruptible]() mutable {
        bool pressed = m_isActive();

        if (!pressedLast && pressed) {
          command->Schedule(interruptible);
        }

        pressedLast = pressed;
      });

  return *this;
}

Trigger Trigger::WhenActive(std::function<void()> toRun,
                            std::initializer_list<void*> requirements) {
  return WhenActive(std::move(toRun),
                    {requirements.begin(), requirements.end()});
}

Trigger Trigger::WhenActive(std::function<void()> toRun,
                            wpi::span<void* const> requirements) {
  return WhenActive(InstantCommand(std::move(toRun), requirements));
}

Trigger Trigger::WhileActiveContinous(Command* command, bool interruptible) {
  CommandScheduler::GetInstance().AddButton(
      [pressedLast = m_isActive(), *this, command, interruptible]() mutable {
        bool pressed = m_isActive();

        if (pressed) {
          command->Schedule(interruptible);
        } else if (pressedLast && !pressed) {
          command->Cancel();
        }

        pressedLast = pressed;
      });
  return *this;
}

Trigger Trigger::WhileActiveContinous(
    std::function<void()> toRun,
    std::initializer_list<void*> requirements) {
  return WhileActiveContinous(std::move(toRun),
                              {requirements.begin(), requirements.end()});
}

Trigger Trigger::WhileActiveContinous(
    std::function<void()> toRun, wpi::span<void* const> requirements) {
  return WhileActiveContinous(InstantCommand(std::move(toRun), requirements));
}

Trigger Trigger::WhileActiveOnce(Command* command, bool interruptible) {
  CommandScheduler::GetInstance().AddButton(
      [pressedLast = m_isActive(), *this, command, interruptible]() mutable {
        bool pressed = m_isActive();

        if (!pressedLast && pressed) {
          command->Schedule(interruptible);
        } else if (pressedLast && !pressed) {
          command->Cancel();
        }

        pressedLast = pressed;
      });
  return *this;
}

Trigger Trigger::WhenInactive(Command* command, bool interruptible) {
  CommandScheduler::GetInstance().AddButton(
      [pressedLast = m_isActive(), *this, command, interruptible]() mutable {
        bool pressed = m_isActive();

        if (pressedLast && !pressed) {
          command->Schedule(interruptible);
        }

        pressedLast = pressed;
      });
  return *this;
}

Trigger Trigger::WhenInactive(std::function<void()> toRun,
                              std::initializer_list<void*> requirements) {
  return WhenInactive(std::move(toRun),
                      {requirements.begin(), requirements.end()});
}

Trigger Trigger::WhenInactive(std::function<void()> toRun,
                              wpi::span<void* const> requirements) {
  return WhenInactive(InstantCommand(std::move(toRun), requirements));
}

Trigger Trigger::ToggleWhenActive(Command* command, bool interruptible) {
  CommandScheduler::GetInstance().AddButton(
      [pressedLast = m_isActive(), *this, command, interruptible]() mutable {
        bool pressed = m_isActive();

        if (!pressedLast && pressed) {
          if (command->IsScheduled()) {
            command->Cancel();
          } else {
            command->Schedule(interruptible);
          }
        }

        pressedLast = pressed;
      });
  return *this;
}

Trigger Trigger::CancelWhenActive(Command* command) {
  CommandScheduler::GetInstance().AddButton(
      [pressedLast = m_isActive(), *this, command]() mutable {
        bool pressed = m_isActive();

        if (!pressedLast && pressed) {
          command->Cancel();
        }

        pressedLast = pressed;
      });
  return *this;
}

Trigger Trigger::Debounce(units::second_t debounceTime,
                          frc::Debouncer::DebounceType type) {
  return Trigger(
      [debouncer = frc::Debouncer(debounceTime, type), *this]() mutable {
        return debouncer.Calculate(m_isActive());
      });
}
