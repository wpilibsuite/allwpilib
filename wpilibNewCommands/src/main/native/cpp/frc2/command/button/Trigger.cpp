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
  m_loop->Bind(
      [condition = m_condition, previous = m_condition(), command]() mutable {
        bool current = condition();

        if (!previous && current) {
          command->Schedule();
        }

        previous = current;
      });
  return *this;
}

Trigger Trigger::OnTrue(CommandPtr&& command) {
  m_loop->Bind([condition = m_condition, previous = m_condition(),
                command = std::move(command)]() mutable {
    bool current = condition();

    if (!previous && current) {
      command.Schedule();
    }

    previous = current;
  });
  return *this;
}

Trigger Trigger::OnFalse(Command* command) {
  m_loop->Bind(
      [condition = m_condition, previous = m_condition(), command]() mutable {
        bool current = condition();

        if (previous && !current) {
          command->Schedule();
        }

        previous = current;
      });
  return *this;
}

Trigger Trigger::OnFalse(CommandPtr&& command) {
  m_loop->Bind([condition = m_condition, previous = m_condition(),
                command = std::move(command)]() mutable {
    bool current = condition();

    if (previous && !current) {
      command.Schedule();
    }

    previous = current;
  });
  return *this;
}

Trigger Trigger::WhileTrue(Command* command) {
  m_loop->Bind(
      [condition = m_condition, previous = m_condition(), command]() mutable {
        bool current = condition();

        if (!previous && current) {
          command->Schedule();
        } else if (previous && !current) {
          command->Cancel();
        }

        previous = current;
      });
  return *this;
}

Trigger Trigger::WhileTrue(CommandPtr&& command) {
  m_loop->Bind([condition = m_condition, previous = m_condition(),
                command = std::move(command)]() mutable {
    bool current = condition();

    if (!previous && current) {
      command.Schedule();
    } else if (previous && !current) {
      command.Cancel();
    }

    previous = current;
  });
  return *this;
}

Trigger Trigger::WhileFalse(Command* command) {
  m_loop->Bind(
      [condition = m_condition, previous = m_condition(), command]() mutable {
        bool current = condition();

        if (previous && !current) {
          command->Schedule();
        } else if (!previous && current) {
          command->Cancel();
        }

        previous = current;
      });
  return *this;
}

Trigger Trigger::WhileFalse(CommandPtr&& command) {
  m_loop->Bind([condition = m_condition, previous = m_condition(),
                command = std::move(command)]() mutable {
    bool current = condition();

    if (!previous && current) {
      command.Schedule();
    } else if (previous && !current) {
      command.Cancel();
    }

    previous = current;
  });
  return *this;
}

Trigger Trigger::ToggleOnTrue(Command* command) {
  m_loop->Bind([condition = m_condition, previous = m_condition(),
                command = command]() mutable {
    bool current = condition();

    if (!previous && current) {
      if (command->IsScheduled()) {
        command->Cancel();
      } else {
        command->Schedule();
      }
    }

    previous = current;
  });
  return *this;
}

Trigger Trigger::ToggleOnTrue(CommandPtr&& command) {
  m_loop->Bind([condition = m_condition, previous = m_condition(),
                command = std::move(command)]() mutable {
    bool current = condition();

    if (!previous && current) {
      if (command.IsScheduled()) {
        command.Cancel();
      } else {
        command.Schedule();
      }
    }

    previous = current;
  });
  return *this;
}

Trigger Trigger::ToggleOnFalse(Command* command) {
  m_loop->Bind([condition = m_condition, previous = m_condition(),
                command = command]() mutable {
    bool current = condition();

    if (previous && !current) {
      if (command->IsScheduled()) {
        command->Cancel();
      } else {
        command->Schedule();
      }
    }

    previous = current;
  });
  return *this;
}

Trigger Trigger::ToggleOnFalse(CommandPtr&& command) {
  m_loop->Bind([condition = m_condition, previous = m_condition(),
                command = std::move(command)]() mutable {
    bool current = condition();

    if (previous && !current) {
      if (command.IsScheduled()) {
        command.Cancel();
      } else {
        command.Schedule();
      }
    }

    previous = current;
  });
  return *this;
}

WPI_IGNORE_DEPRECATED
Trigger Trigger::WhenActive(Command* command) {
  return OnTrue(command);
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
  m_loop->Bind([condition = m_condition, previous = m_condition(),
                command = std::move(command)]() mutable {
    bool current = condition();

    if (current) {
      command->Schedule();
    } else if (previous && !current) {
      command->Cancel();
    }

    previous = current;
  });
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
  m_loop->Bind(
      [condition = m_condition, previous = m_condition(), command]() mutable {
        bool current = condition();

        if (!previous && current) {
          command->Schedule();
        } else if (previous && !current) {
          command->Cancel();
        }

        previous = current;
      });
  return *this;
}

Trigger Trigger::WhenInactive(Command* command) {
  m_loop->Bind(
      [condition = m_condition, previous = m_condition(), command]() mutable {
        bool current = condition();

        if (previous && !current) {
          command->Schedule();
        }

        previous = current;
      });
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
  m_loop->Bind([condition = m_condition, previous = m_condition(),
                command = command]() mutable {
    bool current = condition();

    if (!previous && current) {
      if (command->IsScheduled()) {
        command->Cancel();
      } else {
        command->Schedule();
      }
    }

    previous = current;
  });
  return *this;
}

Trigger Trigger::CancelWhenActive(Command* command) {
  m_loop->Bind([condition = m_condition, previous = m_condition(),
                command = std::move(command)]() mutable {
    bool current = condition();

    if (!previous && current) {
      command->Cancel();
    }

    previous = current;
  });
  return *this;
}
WPI_UNIGNORE_DEPRECATED

Trigger Trigger::Debounce(units::second_t debounceTime,
                          frc::Debouncer::DebounceType type) {
  return Trigger(m_loop, [debouncer = frc::Debouncer(debounceTime, type),
                          condition = m_condition]() mutable {
    return debouncer.Calculate(condition());
  });
}
