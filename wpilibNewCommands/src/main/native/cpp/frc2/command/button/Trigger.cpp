// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/Trigger.h"

#include <utility>

#include <frc/filter/Debouncer.h>

#include "frc2/command/CommandPtr.h"

using namespace frc;
using namespace frc2;

Trigger::Trigger(const Trigger& other) = default;

void Trigger::AddBinding(wpi::unique_function<void(bool, bool)>&& body,
                         InitialState initialState) {
  m_loop->Bind([condition = m_condition,
                previous = GetInitialState(initialState),
                body = std::move(body)]() mutable {
    bool current = condition();

    body(previous, current);

    previous = current;
  });
}

Trigger Trigger::OnChange(Command* command, InitialState initialState) {
  AddBinding(
      [command](bool previous, bool current) {
        if (previous != current) {
          command->Schedule();
        }
      },
      initialState);
  return *this;
}

Trigger Trigger::OnChange(CommandPtr&& command, InitialState initialState) {
  AddBinding(
      [command = std::move(command)](bool previous, bool current) {
        if (previous != current) {
          command.Schedule();
        }
      },
      initialState);
  return *this;
}

Trigger Trigger::OnTrue(Command* command, InitialState initialState) {
  AddBinding(
      [command](bool previous, bool current) {
        if (!previous && current) {
          command->Schedule();
        }
      },
      initialState);
  return *this;
}

Trigger Trigger::OnTrue(CommandPtr&& command, InitialState initialState) {
  AddBinding(
      [command = std::move(command)](bool previous, bool current) {
        if (!previous && current) {
          command.Schedule();
        }
      },
      initialState);
  return *this;
}

Trigger Trigger::OnFalse(Command* command, InitialState initialState) {
  AddBinding(
      [command](bool previous, bool current) {
        if (previous && !current) {
          command->Schedule();
        }
      },
      initialState);
  return *this;
}

Trigger Trigger::OnFalse(CommandPtr&& command, InitialState initialState) {
  AddBinding(
      [command = std::move(command)](bool previous, bool current) {
        if (previous && !current) {
          command.Schedule();
        }
      },
      initialState);
  return *this;
}

Trigger Trigger::WhileTrue(Command* command, InitialState initialState) {
  AddBinding(
      [command](bool previous, bool current) {
        if (!previous && current) {
          command->Schedule();
        } else if (previous && !current) {
          command->Cancel();
        }
      },
      initialState);
  return *this;
}

Trigger Trigger::WhileTrue(CommandPtr&& command, InitialState initialState) {
  AddBinding(
      [command = std::move(command)](bool previous, bool current) {
        if (!previous && current) {
          command.Schedule();
        } else if (previous && !current) {
          command.Cancel();
        }
      },
      initialState);
  return *this;
}

Trigger Trigger::WhileFalse(Command* command, InitialState initialState) {
  AddBinding(
      [command](bool previous, bool current) {
        if (previous && !current) {
          command->Schedule();
        } else if (!previous && current) {
          command->Cancel();
        }
      },
      initialState);
  return *this;
}

Trigger Trigger::WhileFalse(CommandPtr&& command, InitialState initialState) {
  AddBinding(
      [command = std::move(command)](bool previous, bool current) {
        if (!previous && current) {
          command.Schedule();
        } else if (previous && !current) {
          command.Cancel();
        }
      },
      initialState);
  return *this;
}

Trigger Trigger::ToggleOnTrue(Command* command, InitialState initialState) {
  AddBinding(
      [command](bool previous, bool current) {
        if (!previous && current) {
          if (command->IsScheduled()) {
            command->Cancel();
          } else {
            command->Schedule();
          }
        }
      },
      initialState);
  return *this;
}

Trigger Trigger::ToggleOnTrue(CommandPtr&& command, InitialState initialState) {
  AddBinding(
      [command = std::move(command)](bool previous, bool current) {
        if (!previous && current) {
          if (command.IsScheduled()) {
            command.Cancel();
          } else {
            command.Schedule();
          }
        }
      },
      initialState);
  return *this;
}

Trigger Trigger::ToggleOnFalse(Command* command, InitialState initialState) {
  AddBinding(
      [command](bool previous, bool current) {
        if (previous && !current) {
          if (command->IsScheduled()) {
            command->Cancel();
          } else {
            command->Schedule();
          }
        }
      },
      initialState);
  return *this;
}

Trigger Trigger::ToggleOnFalse(CommandPtr&& command,
                               InitialState initialState) {
  AddBinding(
      [command = std::move(command)](bool previous, bool current) {
        if (previous && !current) {
          if (command.IsScheduled()) {
            command.Cancel();
          } else {
            command.Schedule();
          }
        }
      },
      initialState);
  return *this;
}

Trigger Trigger::Debounce(units::second_t debounceTime,
                          frc::Debouncer::DebounceType type) {
  return Trigger(m_loop, [debouncer = frc::Debouncer(debounceTime, type),
                          condition = m_condition]() mutable {
    return debouncer.Calculate(condition());
  });
}

bool Trigger::Get() const {
  return m_condition();
}
