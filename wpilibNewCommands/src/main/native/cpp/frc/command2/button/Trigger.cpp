// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/command2/button/Trigger.h"

#include <frc/filter/Debouncer.h>

#include "frc/command2/InstantCommand.h"

using namespace frc;

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

Trigger Trigger::Debounce(units::second_t debounceTime,
                          Debouncer::DebounceType type) {
  return Trigger(m_loop, [debouncer = Debouncer(debounceTime, type),
                          condition = m_condition]() mutable {
    return debouncer.Calculate(condition());
  });
}
